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
  db_.reset(new cppdb::session(dbconn_str));

  queue_.reset(new soil::ReaderWriterQueue<std::string>(this));

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

  std::string data(
      reinterpret_cast<const char*>(msg->data()),
      msg->len());
  pushMsg(data);
}

void Server::parseDoc(
    const rapidjson::Document& doc) {
  SOIL_FUNC_TRACE;

  try {
    auto itr = doc.MemberBegin();
    std::string key = itr->name.GetString();

    boost::regex re_field("^CThostFtdc(.*)Field$");
    boost::smatch mat;
    if (boost::regex_match(key, mat, re_field)) {
      std::string t_name = mat[1];
      const rapidjson::Value& f_data = doc[key];

      auto i_iter = sqls_.find(t_name);
      if (i_iter != sqls_.end()) {
        doInsert(i_iter->second, f_data);
        return;
      }

      std::string create_sql;
      std::string insert_sql;
      sqlString(t_name, f_data, &create_sql, &insert_sql);

      SOIL_DEBUG_PRINT(create_sql);
      SOIL_DEBUG_PRINT(insert_sql);

      (*db_) <<create_sql <<cppdb::exec;

      cppdb::statement stat = db_->prepare(insert_sql);
      doInsert(stat, f_data);

      sqls_[t_name] = stat;
      // (*db_) <<insert_sql <<cppdb::exec;
    }
  } catch (std::exception const &e) {
    SOIL_ERROR("Error: {}", e.what());
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

void Server::doInsert(
    cppdb::statement stat,
    const rapidjson::Value& data) {
  SOIL_FUNC_TRACE;

  stat.reset();
  for (auto itr = data.MemberBegin();
       itr != data.MemberEnd(); ++itr) {
    auto& value = itr->value;

    if (value.IsString()) {
      stat <<value.GetString();
    } else if (value.IsDouble()) {
      stat <<value.GetDouble();
    } else if (value.IsNumber()
               || value.IsBool()) {
      if (value.IsInt()) {
        stat <<value.GetInt();
      } else if (value.IsUint()) {
        stat <<value.GetUint();
      } else if (value.IsInt64()) {
        stat <<value.GetInt64();
      } else if (value.IsUint64()) {
        stat <<value.GetUint64();
      } else if (value.IsTrue()) {
        stat <<1;
      } else {
        stat <<0;
      }
    }
  }
  stat.exec();
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
    (*insert_sql) += "?";
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
