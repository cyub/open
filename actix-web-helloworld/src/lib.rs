use actix_web::{get, middleware, web, App as ActixApp, HttpRequest, HttpServer, Result};
use serde::Serialize;

#[derive(Debug)]
pub struct App {
    port: u16,
}

impl App {
    pub fn new(port: u16) -> Self {
        App { port }
    }

    pub async fn run(&self) -> std::io::Result<()> {
        println!("Starting http server: 127.0.0.1:{}", self.port);

        HttpServer::new(|| {
            ActixApp::new()
                .wrap(middleware::Logger::default())
                .service(index)
        })
        .bind(("127.0.0.1", self.port))?
        .run()
        .await
    }
}

#[derive(Serialize)]
struct IndexResponse {
    message: String,
}

#[get("/")]
async fn index(_req: HttpRequest) -> Result<web::Json<IndexResponse>> {
    Ok(web::Json(IndexResponse {
        message: String::from("hello, world"),
    }))
}
