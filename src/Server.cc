// Copyright (c) 2010
// All rights reserved.

#include "Server.hh"
#include "soil/Log.hh"
#include "boost/regex.hpp"

namespace qatar {

Server::Server(
    const rapidjson::Document& doc) :
    trader_service_(nullptr) {
  SOIL_FUNC_TRACE;

  cond_.reset(soil::STimer::create());
  options_.reset(new Options(doc));

  trader_service_.reset(
      cata::TraderService::create(
          doc,
          this));
  trading_day_ = trader_service_->tradingDay();
  SOIL_INFO("trading day: {}", trading_day_);

  std::string dbconn_str = fmt::format(
      "sqlite3:db={}_{};@pool_size=16",
      options_->db, trading_day_);
  dbservice_.reset(new DBService(
      dbconn_str,
      options_->batch_size));

  sub_service_.reset(
      zod::SubService::create(
          options_->sub_addr,
          this));

  sub_service_->setSubscribe("");

  push_service_.reset(
      zod::PushService::create(
          options_->push_addr));

  go();
}

Server::~Server() {
  SOIL_FUNC_TRACE;
}

void Server::onMsg(
    std::shared_ptr<zod::Msg> msg) {
  SOIL_FUNC_TRACE;

  std::string data(
      reinterpret_cast<const char*>(msg->data()),
      msg->len());
  pushMsg(data);
}

void Server::pushInstrus() {
  SOIL_FUNC_TRACE;

  try {
    InstrusType instrus;
    dbservice_->fetchInstrus(&instrus);
    SOIL_DEBUG_PRINT(instrus.size());

    for (auto& instru : instrus) {
      rapidjson::Document d;
      rapidjson::Pointer("/instru").Set(d, instru);

      rapidjson::StringBuffer buffer;
      rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
      d.Accept(writer);

      SOIL_DEBUG_PRINT(buffer.GetString());
      push_service_->sendMsg(buffer.GetString());
    }
  } catch (std::exception const &e) {
    SOIL_ERROR("ERROR: {}", e.what());
    throw;
  }
}

void Server::go() {
  SOIL_FUNC_TRACE;

  try {
    pushInstrus();

    return;
  } catch (...) {
    SOIL_INFO("New trading day - {} !!!", trading_day_);
  }

  wait(1000);
  trader_service_->queryExchange("");
  wait();

  wait(1000);
  trader_service_->queryProduct("");
  wait();

  wait(1000);
  trader_service_->queryInstrument(
      "",
      "",
      "",
      "");
  wait();

  wait(1000);
  trader_service_->queryExchangeMarginRate("");
  wait();

  wait(1000);
  trader_service_->queryExchangeMarginRateAdjust("");
  wait();

  wait(1000);
  trader_service_->queryInstruMarginRate("");
  wait();

  wait(1000);
  trader_service_->queryInstruCommissionRate("");
  wait();

  wait(1000);
  trader_service_->queryInvestor();
  wait();

  wait(1000);
  trader_service_->queryAccount("");
  wait();

  wait(1000);
  trader_service_->queryTradingCode("");
  wait();

  wait(1000);
  trader_service_->queryOrder(
      "",
      "",
      "",
      "",
      "");
  wait();

  wait(1000);
  trader_service_->queryTrade(
      "",
      "",
      "",
      "",
      "");
  wait();

  wait(1000);
  trader_service_->queryPosition("");
  wait();

  pushInstrus();
}

};  // namespace qatar
