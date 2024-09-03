#include <iostream>

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"

int main() {
  // RESTful 接口的 URL
  std::string url = "http://jsonplaceholder.typicode.com/posts/1";

  // 创建 URI 对象
  Poco::URI uri(url);

  // 创建 HTTPClientSession 对象
  Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());

  try {
    Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET,
                                   uri.getPathAndQuery(),
                                   Poco::Net::HTTPMessage::HTTP_1_1);

    request.setHost(uri.getHost());
    request.add("Accept", "application/json");

    // 发送请求
    session.sendRequest(request);

    // 接收响应
    Poco::Net::HTTPResponse response;
    std::istream& resStream = session.receiveResponse(response);

    // 检查响应状态
    if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK) {
      std::cout << "Response status: " << response.getStatus() << std::endl;
      std::cout << "Response reason: " << response.getReason() << std::endl;
      Poco::StreamCopier::copyStream(resStream, std::cout);
    } else {
      // 处理错误响应
      std::cerr << "Error: " << response.getStatus() << " "
                << response.getReason() << std::endl;
    }
  } catch (const Poco::Exception& exc) {
    // 处理异常
    std::cerr << "Exception: " << exc.displayText() << std::endl;
  }

  return 0;
}