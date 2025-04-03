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
using ::ccapi::Subscription;
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
  Subscription subscription("binance", "", "GENERIC_PUBLIC_SUBSCRIPTION", R"({"method": "SUBSCRIBE", "params": ["btcusdt@depth20@100ms"], "id": 0})");
  session.subscribe(subscription);
  std::this_thread::sleep_for(std::chrono::seconds(15));
  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
