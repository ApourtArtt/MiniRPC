use tokio::sync::Mutex;
use std::sync::Arc;
use std::sync::mpsc;

mod credentials;
mod minirpc_server;

#[tokio::main]
async fn main() {
    let creds = credentials::get_credentials();

    let (tx, rx): (minirpc_server::RpcTx, minirpc_server::RpcRx) = mpsc::channel();

    let rpc_server = Arc::new(Mutex::new(minirpc_server::MiniRPCServer::new(
        creds.rpc_creds.clone(),
        tx.clone(),
    )));

    tokio::spawn(async move {
        minirpc_server::run(rpc_server).await.expect("Failed starting MiniRPC Server");
    });

    loop {
        match rx.recv() {
            Ok((mut client_ident, packet)) => {
                if !client_ident.is_logged_in && packet != creds.rpc_creds.password {
                    client_ident.send_channel(String::from("0")).await; // @TODO
                }
                client_ident.is_logged_in = true;
                println!("packet : {}", packet);
            },
            Err(e) => {
                println!("Error while reading packet : {}", e);
            }
        }
    }
}
