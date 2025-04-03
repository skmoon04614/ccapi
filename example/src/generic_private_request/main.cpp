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
  if (UtilSystem::getEnvAsString("BINANCE_API_KEY").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_KEY" << std::endl;
    return EXIT_FAILURE;
  }
  if (UtilSystem::getEnvAsString("BINANCE_API_SECRET").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_SECRET" << std::endl;
    return EXIT_FAILURE;
  }
  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
  Request request(Request::Operation::GENERIC_PRIVATE_REQUEST, "binance", "", "Get Account Info");
  request.appendParam({
      {"HTTP_METHOD", "GET"},
      {"HTTP_PATH", "/api/v3/account"},
  });
  session.sendRequest(request);
  std::this_thread::sleep_for(std::chrono::seconds(2));
  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
