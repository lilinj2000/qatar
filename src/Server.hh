// Copyright (c) 2010
// All rights reserved.

#ifndef QATAR_SERVER_HH
#define QATAR_SERVER_HH

#include <string>
#include <set>
#include "Options.hh"
#include "soil/json.hh"
#include "cppdb/frontend.h"
#include "soil/STimer.hh"
#include "soil/MsgQueue.hh"
#include "cata/TraderService.hh"

namespace qatar {

class Server :
      public cata::TraderCallback {
 public:
  explicit Server(
      const rapidjson::Document& doc);

  virtual ~Server();

  void msgCallback(const std::string* msg);

  // from cata::TraderCallback
  virtual void onRspQryExchange(
      const std::string& theExchange,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theExchange);
    notify(bIsLast);
  }

  virtual void onRspQryProduct(
      const std::string& theProduct,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theProduct);
    notify(bIsLast);
  }

  virtual void onRspQryInstrument(
      const std::string& theInstrument,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theInstrument);
    notify(bIsLast);
  }

  virtual void onRspQryExchangeMarginRate(
      const std::string& theExchangeMarginRate,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theExchangeMarginRate);
    notify(bIsLast);
  }

  virtual void onRspQryExchangeMarginRateAdjust(
      const std::string& theExchangeMarginRateAdjust,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theExchangeMarginRateAdjust);
    notify(bIsLast);
  }

  virtual void onRspQryInstrumentMarginRate(
      const std::string& theInstrumentMarginRate,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theInstrumentMarginRate);
    notify(bIsLast);
  }

  virtual void onRspQryInstrumentCommissionRate(
      const std::string& theInstrumentCommissionRate,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theInstrumentCommissionRate);
    notify(bIsLast);
  }

  virtual void onRspQryInvestor(
      const std::string& theInvestor,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theInvestor);
    notify(bIsLast);
  }

  virtual void onRspQryTradingAccount(
      const std::string& theTradingAccount,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theTradingAccount);
    notify(bIsLast);
  }

  virtual void onRspQryTradingCode(
      const std::string& theTradingCode,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theTradingCode);
    notify(bIsLast);
  }

  virtual void onRspQryOrder(
      const std::string& theOrder,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theOrder);
    notify(bIsLast);
  }

  virtual void onRspQryTrade(
      const std::string& theTrade,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theTrade);
    notify(bIsLast);
  }

  
  virtual void onRspQryInvestorPosition(
      const std::string& theInvestorPosition,
      const std::string& theRspInfo,
      int nRequestID,
      bool bIsLast) {
    SOIL_FUNC_TRACE;

    pushMsg(theInvestorPosition);
    notify(bIsLast);
  }

 protected:
  void sqlString(
      const std::string& t_name,
      rapidjson::Value& data,
      std::string& create_sql,
      std::string& insert_sql);

  void go();
  
  void wait(int mill_second = -1) {
    cond_->wait(mill_second);
  }

  void notify(bool is_last) {
    if (is_last) {
      cond_->notifyAll();
    }
  }

  void pushMsg(const std::string& msg) {
    if (!msg.empty()) {
      msg_queue_->pushMsg(
          new std::string(msg));
    }
  }

 private:
  std::unique_ptr<Options> options_;

  std::unique_ptr<soil::STimer> cond_;

  std::set<std::string> instrus_;
  std::set<std::string> prod_instrus_;  // one instru each product

  std::unique_ptr<soil::MsgQueue<std::string, Server> > msg_queue_;

  std::unique_ptr<cppdb::session> db_;

  cata::TraderService* trader_service_;
};

};  // namespace qatar

#endif
