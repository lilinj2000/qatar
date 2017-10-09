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

  sub_service_.reset(
      zod::SubService::create(
          options_->sub_addr,
          this));

  sub_service_->setSubscribe("");

  push_service_.reset(
      zod::PushService::create(
          options_->push_addr));

  queue_.reset(new soil::ReaderWriterQueue<std::string>(this));

  trader_service_.reset(
      cata::TraderService::create(
          doc,
          this));

  go();

  pushInstrus();
}

Server::~Server() {
  SOIL_FUNC_TRACE;
}

void Server::msgCallback(
    std::shared_ptr<std::string> msg) {
  SOIL_FUNC_TRACE;
  SOIL_DEBUG_PRINT(*msg);

  rapidjson::Document doc;
  if (doc.Parse(*msg).HasParseError()) {
    SOIL_DEBUG_PRINT(soil::json::get_parse_error(doc, *msg));
    return;
  }

  parseDoc(doc);
}

void Server::msgCallback(
    std::shared_ptr<zod::Msg> msg) {
  SOIL_FUNC_TRACE;

  rapidjson::Document doc;
  if (doc.Parse(
          reinterpret_cast<const char*>(msg->data()),
          msg->len()).HasParseError()) {
    SOIL_DEBUG_PRINT(
        soil::json::get_parse_error(doc));
    return;
  }

  parseDoc(doc);
}

void Server::parseDoc(
    const rapidjson::Document& doc) {
  SOIL_FUNC_TRACE;

  auto itr = doc.MemberBegin();
  std::string key = itr->name.GetString();

  boost::regex re_field("^CThostFtdc(.*)Field$");
  boost::smatch mat;
  if (boost::regex_match(key, mat, re_field)) {
    std::string t_name = mat[1];
    const rapidjson::Value& f_data = doc[key];

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

void Server::fieldType(
    const rapidjson::Value& data,
    std::string* type,
    std::string* value) {
  if (data.IsString()) {
    (*type) = "TEXT";
    (*value).append("'");
    (*value) += data.GetString();
    (*value).append("'");
  } else if (data.IsDouble()) {
    (*type) = "REAL";
    (*value) = std::to_string(data.GetDouble());
  } else if (data.IsNumber()
             || data.IsBool()) {
    (*type) = "INTEGER";
    if (data.IsInt()) {
      (*value) = std::to_string(data.GetInt());
    } else if (data.IsUint()) {
      (*value) = std::to_string(data.GetUint());
    } else if (data.IsInt64()) {
      (*value) = std::to_string(data.GetInt64());
    } else if (data.IsUint64()) {
      (*value) = std::to_string(data.GetUint64());
    } else if (data.IsTrue()) {
      (*value) = "1";
    } else {
      (*value) = "0";
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

    std::string type, value;
    fieldType(itr->value, &type, &value);
    (*create_sql) += itr->name.GetString();
    (*create_sql) += " " + type;
    (*insert_sql) += value;
  }
  (*create_sql) += ");";
  (*insert_sql) += ");";

  return;
}

void Server::pushInstrus() {
  SOIL_FUNC_TRACE;

  try {
    std::string sql = "SELECT DISTINCT InstrumentID FROM Instrument";

    cppdb::result res = (*db_) <<sql;

    while (res.next()) {
      std::string instru;
      res >> instru;
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
  }
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

};  // namespace qatar
