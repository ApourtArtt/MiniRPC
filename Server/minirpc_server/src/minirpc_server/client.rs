use tokio_stream::{Stream, StreamExt};
use tokio::net::TcpStream;
use tokio_util::codec::{Framed, LinesCodec, LinesCodecError};
use tokio::sync::mpsc;
use std::error::Error;
use tokio::sync::mpsc::error::SendError;
use tokio::sync::Mutex;
use std::sync::Arc;
use std::net::SocketAddr;
use std::pin::Pin;
use std::task::{Context, Poll};
use futures::SinkExt;

use crate::minirpc_server::Shared;

pub type TxClient = mpsc::UnboundedSender<String>;
pub type RxClient = mpsc::UnboundedReceiver<String>;

#[derive(Debug)]
pub enum Message {
    Network(String),
    Channel(String),
}

#[derive(Clone)]
pub struct ClientIdentity {
    pub address: SocketAddr,
    pub tx: TxClient,
    pub is_logged_in: bool
}

impl ClientIdentity {
    pub fn new(
        address: SocketAddr,
        tx: TxClient,
    ) -> Self {
        ClientIdentity {
            address,
            tx,
            is_logged_in: false,
        }
    }

    pub async fn send_channel(&self, packet: String) -> Result<(), SendError<String>> {
        self.tx.send(packet)
    }
}

pub struct Client {
    rx: RxClient,
    pub stream: Framed<TcpStream, LinesCodec>,
    pub client_identity: ClientIdentity,
}

impl Client {
    pub fn new(
        stream: Framed<TcpStream, LinesCodec>,
        address: SocketAddr,
    ) -> Self {
        let (tx, rx): (TxClient, RxClient) = mpsc::unbounded_channel();
        Client {
            rx,
            stream,
            client_identity: ClientIdentity::new(
                address,
                tx,
            )
        }
    }

    pub async fn send_network(&mut self, packet: String) -> Result<(), LinesCodecError> {
        self.stream.send(packet).await
    }
}

impl Stream for Client {
    type Item = Result<Message, LinesCodecError>;

    fn poll_next(mut self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Option<Self::Item>> {
        // Data on channel
        if let Poll::Ready(Some(v)) = Pin::new(&mut self.rx).poll_recv(cx) { // @TODO
            return Poll::Ready(Some(Ok(Message::Channel(v))));
        }

        // Data on network
        let result: Option<_> = futures::ready!(Pin::new(&mut self.stream).poll_next(cx));
        Poll::Ready(match result {
            Some(Ok(message)) => Some(Ok(Message::Network(message))),
            Some(Err(e)) => Some(Err(e)),
            None => None,
        })
    }
}

pub async fn handle_client(
    stream: TcpStream,
    address: SocketAddr,
    shared: Arc<Mutex<Shared>>,
) -> Result<i8, Box<dyn Error>> {
    let stream = Framed::new(stream, LinesCodec::new());
    let mut client = Client::new(stream, address);
    // If need to add client to serv, create client before entering this func

    while let Some(result) = client.next().await {
        match result {
            Ok(Message::Network(packet)) => {
                if !client.client_identity.is_logged_in && packet != shared.lock().await.password {
                    return Ok(-1);
                }
                client.client_identity.is_logged_in = true;

                match shared.lock().await.tx.send((client.client_identity.clone(), packet)) {
                    Ok(_) => {},
                    Err(e) => { return Err(Box::new(e)); }
                }
            },
            Ok(Message::Channel(packet)) => {
                match client.send_network(packet).await {
                    Ok(_) => {},
                    Err(e) => { return Err(Box::new(e)); }
                }
            },
            Err(e) => { return Err(Box::new(e)); }
        }
    }
    
    Ok(0)
}