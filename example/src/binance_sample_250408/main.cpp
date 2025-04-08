#include "ccapi_cpp/ccapi_session.h"
namespace ccapi {
Logger* Logger::logger = nullptr;  // This line is needed.
class MyEventHandler : public EventHandler {
 public:
  bool processEvent(const Event& event, Session* session) override {
    std::cout << "Received an event:\n" + event.toStringPretty(2, 2) << std::endl;
    return true;
  }
};
} /* namespace ccapi */
using ::ccapi::ExecutionManagementService;
using ::ccapi::MyEventHandler;
using ::ccapi::Request;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::UtilSystem;
using ::ccapi::UtilTime;
int main(int argc, char** argv) {
#ifdef USE_ENV_FOR_SECRET
  if (UtilSystem::getEnvAsString("BINANCE_API_KEY").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_KEY" << std::endl;
    return EXIT_FAILURE;
  }
  if (UtilSystem::getEnvAsString("BINANCE_API_SECRET").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_SECRET" << std::endl;
    return EXIT_FAILURE;
  }
#endif

  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
#ifndef USE_ENV_FOR_SECRET
  sessionConfigs.setCredential({
      {"BINANCE_API_KEY", "..."},     // TODO: replace with actual API KEY
      {"BINANCE_API_SECRET", "..."},  // TODO: replace with actual API SECRET
  });
#endif

  Session session(sessionOptions, sessionConfigs, &eventHandler);

  std::string exchange = "binance";
  std::string symbol = "BTCUSDT";

#if 0  // Orderbook
  // Request::Operation::GET_MARKET_DEPTH 사용 시 response 파싱하는 단계에서 "rapidjson internal assertion error" 발생
  // Request request(Request::Operation::GET_MARKET_DEPTH, "binance", "BTCUSDT);

  Request request(Request::Operation::GENERIC_PUBLIC_REQUEST, exchange);
  request.appendParam({
      {"HTTP_METHOD", "GET"},
      {"HTTP_PATH", "/api/v3/depth"},
      {"HTTP_QUERY_STRING", "symbol=" + symbol + "&limit=20"},
  });
#else
  std::string orderId = "...";  // TODO: replace with numerical orderId
  Request request(Request::Operation::CANCEL_ORDER, exchange, symbol);
  request.appendParam({
      {"recvWindow", "60000"},
      {"orderId", orderId},
  });
#endif

  session.sendRequest(request);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
