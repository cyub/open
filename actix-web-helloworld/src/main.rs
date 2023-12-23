#[actix_web::main]
async fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "debug");
    env_logger::init();

    let app = actix_web_helloworld::App::new(8000);
    app.run().await
}
