mod hub;
mod depots;
mod config;


#[tokio::main]
async fn main() {
    let hub = hub::arrera_hub::ArreraHub::new().await.unwrap();
}
