// Copyright (c) 2010
// All rights reserved.

#ifndef QATAR_SERVER_HH
#define QATAR_SERVER_HH

#include <string>
#include <set>
#include "Config.hh"
#include "json/json.hh"
#include "cppdb/frontend.h"
#include "subject/Service.hh"
#include "soil/STimer.hh"
#include "zod/PushService.hh"

namespace qatar {

class Server : public subject::ServiceCallback {
 public:
  Server(int argc, char* argv[]);

  virtual ~Server();

  virtual void onMessage(const std::string& msg);

protected:
  void sqlString(const std::string& name,
                 json::Value& data,
                 std::string& create_sql,
                 std::string& insert_sql);

  void go();
  
  void queryExchange();

  void queryProduct();

  void queryInstrument();

  void fetchInstrus();

  void queryExchangeMarginRate();

  void queryExchangeMarginRateAdjust();

  void queryInstruMarginRate();

  void queryInstruCommissionRate();

  void queryInvestor();

  void queryAccount();

  void queryTradingCode();

  void queryOrder();

  void queryTrade();

  void queryPosition();

  void wait(int mill_second = -1) {
    cond_->wait(mill_second);
  }

  void notify() {
    cond_->notifyAll();
  }


 private:
  std::unique_ptr<qatar::Config> config_;

  std::unique_ptr<cppdb::session> db_;

  std::unique_ptr<subject::Service> subject_service_;

  std::unique_ptr<zod::PushService> push_service_;

  std::unique_ptr<soil::STimer> cond_;

  std::set<std::string> instrus_;
  
  std::set<std::string> prod_instrus_;  // one instru each product
};

};  // namespace qatar

#endif
