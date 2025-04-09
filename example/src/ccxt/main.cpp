#include <any>
#include <ctime>
#include <string>
#include <iostream>
#include <stdexcept>
#include <unordered_map>


using Params = std::unordered_map<std::string, std::any>;

class Binance {
public:
  auto cancelOrder(const int id, const std::string& symbol, const Params& params = {})
  {
    if (symbol.empty())
    {
      throw std::invalid_argument("invalid symbol: " + symbol);
    }

    std::string method = "privateDeleteOrder";
    const std::string marketType = getMarketType(symbol, params);
    if (marketType == "usdt_m")
    {
      if (!isPortfolioMargin())
      {
        method = "fapiPrivateDeleteOrder";
      }
      else if (params.find("is_conditional") == params.end())
      {
        throw std::invalid_argument("cancel_order[portfolio, usdt_m]: missing is_conditional parameter");
      }
      else if (std::any_cast<bool>(params.at("is_conditional")))
      {
        method = "papiPrivateDeleteUmConditionalOrder";
      }
      else
      {
        method = "papiPrivateDeleteUmOrder";
      }
    }
    else if (marketType == "coin_m")
    {
      if (!isPortfolioMargin())
      {
        method = "dapiPrivateDeleteOrder";
      }
      else if (params.find("is_conditional") == params.end())
      {
        throw std::invalid_argument("cancel_order[portfolio, coin_m]: missing is_conditional parameter");
      }
      else if (std::any_cast<bool>(params.at("is_conditional")))
      {
        method = "papiPrivateDeleteCmConditionalOrder";
      }
      else
      {
        method = "papiPrivateDeleteCmOrder";
      }
    }
    else if (marketType == "margin")
    {
      if (!isPortfolioMargin())
      {
        method = "sapiDeleteMarginOrder";
      }
      else
      {
        throw std::runtime_error("portfolio_margin margin not implemented");
      }
    }
    else if (marketType == "option")
    {
      method = "vapiDeleteOrder";
    }

    Params reqParams = cancelOrderRequestParams(id, marketType, params);
    if (method.find("Conditional") != std::string::npos)
    {
      if (reqParams.find("orderId") != params.end())
      {
        reqParams["strategyId"] = reqParams["orderId"];
        reqParams.erase("orderId");
      }

      if (reqParams.find("origClientOrderId") != params.end())
      {
        reqParams["newClientStrategyId"] = reqParams["origClientOrderId"];
        reqParams.erase("origClientOrderId");
      }
    }

    if (marketType == "margin")
    {
      if (!isPortfolioMargin())
        {
          std::string margin_mode = getMarginOrderMode(params);
          reqParams["isIsolated"] = (margin_mode == "ISOLATED");
        }
    }

    Params response;
    try
    {
      response = callAPI(method, reqParams);
    }
    catch (const std::exception &e)
    {
      if (last_http_response.find("UNKNOWN_ORDER") != std::string::npos)
      {
        throw std::runtime_error("OrderNotFound: cancelOrder() error: " + last_http_response);
      }
      throw;
    }
    return response;
  }

  auto fetchOrderBookUsingApiKey(const std::string& symbol, const Params& params = {})
  {
    std::string method = "publicGetDepth";
    const std::string marketType = getMarketType(symbol, params);
    if (marketType == "usdt_m")
    {
      method = "fapiPublicGetDepth";
    }
    else if (marketType == "coin_m")
    {
      method = "dapiPublicGetDepth";
    }
    else if (marketType == "option")
    {
      method = "vapiPublicGetDepth";
    }

    unsigned int limit = 20;
    auto it = params.find("_limit");
    if (it != params.end()) {
      try
      {
        limit =  std::any_cast<int>(it->second);
      } catch (const std::bad_any_cast&)
      {
      }
    }

    if (limit <= 5)
    {
      limit = 5;
    }
    else if (limit <= 10)
    {
      limit = 10;
    }
    else if (limit <= 20)
    {
      limit = 20;
    }
    else if (limit <= 50)
    {
      limit = 50;
    }
    else if (limit <= 100)
    {
      limit = 100;
    }
    else if (limit <= 500)
    {
      limit = 500;
    }
    else if (limit <= 1000)
    {
      limit = 1000;
    }
    else
    {
      limit = 5000;
    }

    Params reqParams = {
      {"symbol", rawSymbol(symbol)},
      {"limit", limit},
    };

    Params response = callAPI(method, reqParams);
    if (marketType == "option")
    {
      response = std::any_cast<Params>(response.at("data"));
    }

    unsigned long long timestamp = std::time(nullptr); // std::any_cast<unsigned long long>(response.at("T"));
    Params orderbook = parseOrderBook(response, timestamp);
    orderbook["nonce"] = std::any_cast<int>(orderbook.at("lastUpdateId"));

    return orderbook;
  }

private:
  std::string last_http_response = "";

  bool isPortfolioMargin()
  {
    /* MOCK FUNCTION */
    return true;
  }

  std::string getMarginOrderMode(const Params& params)
  {
    /* MOCK FUNCTION */
    return "ISOLATED";
  }

  std::string getMarketType(const std::string& symbol, const Params& params)
  {
    /* MOCK FUNCTION */
    return "spot";
  }

  Params cancelOrderRequestParams(const int id, const std::string& symbol, const Params& params)
  {
    /* MOCK FUNCTION */
    Params reqParams = params;
    return reqParams;
  }

  Params parseOrderBook(const Params& response, unsigned long long timestamp)
  {
    /* MOCK FUNCTION */
    Params orderbook = response;
    orderbook["timestamp"] = timestamp;
    orderbook["lastUpdateId"] = 123456789;
    return orderbook;
  }

  std::string rawSymbol(const std::string& symbol)
  {
    /* MOCK FUNCTION */
    return "BTCUSDT";
  }

  Params callAPI(const std::string& method, const Params& reqParams) {
    /* MOCK FUNCTION */
    std::cout << "Calling API method: " << method << std::endl;
    return {{"a", "b"}, {"c", "d"}, {"text", "success"}};
  }
};

int main()
{
  /*
  TO RUN:
    `g++ -std=c++17 main.cpp; ./a.out`
  */

  Binance binance{};
  Params resp1 = binance.cancelOrder(123, "BTC/USDT");
  Params resp2 = binance.fetchOrderBookUsingApiKey("BTC/USDT");
  return 0;
}