use tokio::net::TcpListener;
use tokio::sync::Mutex;
use std::error::Error;
use std::sync::Arc;
use std::sync::mpsc::{Sender, Receiver};

use crate::credentials::MiniRPCCredentials;

pub mod client;

pub type RpcTx = Sender<(client::ClientIdentity, String)>;
pub type RpcRx = Receiver<(client::ClientIdentity, String)>;

pub struct Shared {
    pub password: String,
    pub tx: RpcTx,
}

pub struct MiniRPCServer {
    creds: MiniRPCCredentials,
    pub shared: Arc<Mutex<Shared>>,
    // Clients
    // Vector of message (queue)
    // Channel (thread)
}

impl MiniRPCServer {
    pub fn new(
        creds: MiniRPCCredentials,
        tx: RpcTx,
    ) -> Self {
        MiniRPCServer {
            creds: creds.clone(),
            shared: Arc::new(Mutex::new(Shared{
                password: creds.password,
                tx,
            })),
        }
    }
}

pub async fn run(server: Arc<Mutex<MiniRPCServer>>) -> Result<(), Box<dyn Error>> {
    let tcp: TcpListener;
    {
        let server = server.lock().await;
        tcp = TcpListener::bind(&server.creds.address).await?;
        println!("MiniRPCServer address: {}", &server.creds.address);
    }

    loop {
        let (stream, addr) = tcp.accept().await?;
        let shared: Arc<Mutex<Shared>>;
        {
            let server_copy = server.lock().await;
            shared = server_copy.shared.clone();
        }
        tokio::spawn(async move {
            if let Err(e) = client::handle_client(stream, addr, shared).await {
                let _ = e;
            }
        });
    }
}