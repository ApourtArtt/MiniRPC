use std::env;

#[derive(Debug, Clone)]
pub struct Credentials {
    pub rpc_creds: MiniRPCCredentials,
}

#[derive(Debug, Clone)]
pub struct MiniRPCCredentials {
    pub address: String,
    pub password: String,
}

impl Credentials {
    fn new(
        rpc_address: String,
        rpc_password: String,
    ) -> Self {
        Credentials {
            rpc_creds: MiniRPCCredentials {
                address: rpc_address,
                password: rpc_password,
            },
        }
    }
}

pub fn get_credentials() -> Credentials {
    let rpc_address = env::var("RPC_ADDRESS")
                .expect("Failed retrieving TCP RPC Server address (RPC_ADDRESS)");

    let rpc_password = env::var("RPC_PASSWORD")
                .expect("Failed retrieving RPC Server connection password (RPC_PASSWORD)");

    Credentials::new(
        rpc_address,
        rpc_password,
    )
}