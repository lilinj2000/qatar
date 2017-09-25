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

  db_.reset(new cppdb::session(
      options_->dbconn_str));

  queue_.reset(new soil::ReaderWriterQueue<std::string>(this));

  trader_service_.reset(
      cata::TraderService::create(
          doc,
          this));

  go();
}

Server::~Server() {
  SOIL_FUNC_TRACE;
}

void Server::msgCallback(
    std::shared_ptr<std::string> msg) {
  SOIL_FUNC_TRACE;
  // SOIL_DEBUG_IF_PRINT(msg);

  std::string escape_msg = soil::json::escape_string(*msg);
  rapidjson::Document doc;
  if (doc.Parse(escape_msg).HasParseError()) {
    SOIL_DEBUG_PRINT(soil::json::get_parse_error(doc, escape_msg));
    return;
  }

  auto itr = doc.MemberBegin();
  std::string key = itr->name.GetString();

  boost::regex re_field("^CThostFtdc(.*)Field$");
  boost::smatch mat;
  if (boost::regex_match(key, mat, re_field)) {
    std::string t_name = mat[1];
    rapidjson::Value& f_data = doc[key];

    std::string create_sql;
    std::string insert_sql;
    sqlString(t_name, f_data, &create_sql, &insert_sql);

    SOIL_DEBUG_PRINT(create_sql);
    SOIL_DEBUG_PRINT(insert_sql);

    try {
      (*db_) <<create_sql <<cppdb::exec;
      (*db_) <<insert_sql <<cppdb::exec;
    } catch (std::exception const &e) {
      SOIL_ERROR("db error: {}", e.what());
    }
  }
}

void Server::sqlString(
    const std::string& t_name,
    const rapidjson::Value& data,
    std::string* create_sql,
    std::string* insert_sql) {
  SOIL_FUNC_TRACE;

  (*create_sql) = fmt::format("CREATE TABLE IF NOT EXISTS {} (",
                              t_name);


  (*insert_sql) = fmt::format("INSERT INTO {} VALUES(",
                              t_name);

  bool first = true;
  for (auto itr = data.MemberBegin();
       itr != data.MemberEnd(); ++itr) {
    if (!first) {
      (*create_sql) += ", ";
      (*insert_sql) += ", ";
    } else {
      first = false;
    }
    (*create_sql) += itr->name.GetString();
    (*create_sql) += " TEXT";

    (*insert_sql) += "'";
    (*insert_sql) += itr->value.GetString();
    (*insert_sql) += "'";
  }
  (*create_sql) += ");";
  (*insert_sql) += ");";

  return;
}

void Server::go() {
  SOIL_FUNC_TRACE;

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

  // fetchInstrus();

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
}

// void Server::fetchInstrus() {
//   SOIL_FUNC_TRACE;

//   try {
//     std::string sql = "SELECT DISTINCT InstrumentID FROM Instrument";
//     QATAR_DEBUG <<sql;

//     cppdb::result res = (*db_) <<sql;

//     while(res.next()) {
//       std::string instru;
//       res >> instru;
//       instrus_.insert(instru);
//     }
//   } catch (std::exception const &e) {
//     QATAR_ERROR << "ERROR: " << e.what() << std::endl;
//   }

//   std::set<std::string> prods;
//   for (auto instru : instrus_) {
//     std::string prod;
//     boost::regex re_prod("^(\\D+)\\d+$");
//     boost::smatch mat;
//     if (boost::regex_match(instru, mat, re_prod)) {
//       prod = mat[1];
//     }

//     if (prods.count(prod) > 0) {
//       continue;
//     }
//     prods.insert(prod);

//     prod_instrus_.insert(instru);
//   }
// }


};  // namespace qatar
