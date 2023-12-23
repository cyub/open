#[actix_web::main]
async fn main() -> std::io::Result<()> {
    let app = actix_web_helloworld::App::new(8000);
    app.run().await
}
