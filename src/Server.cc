// Copyright (c) 2010
// All rights reserved.

#include "Server.hh"
#include "Log.hh"
#include "boost/regex.hpp"

namespace qatar {

Server::Server(int argc, char* argv[]) {
  QATAR_TRACE <<"Server::Server()";

  cond_.reset(soil::STimer::create());

  config_.reset(new Config(argc, argv));

  db_.reset(new cppdb::session(config_->options()->connection_string));

  subject::Options options {
    config_->options()->filter,
        config_->options()->sub_addr
        };

  subject_service_.reset(subject::Service::createService(options, this));

  push_service_.reset(zod::PushService::create(config_->options()->push_addr));

  go();
}

Server::~Server() {
  QATAR_TRACE <<"Server::~Server()";
}

void Server::onMessage(const std::string& msg) {
  QATAR_TRACE <<"Server::onMessage()";

  QATAR_DEBUG <<msg;
  json::Document doc;
  json::fromString(msg, &doc);

  auto itr = doc.MemberBegin();
  std::string key = itr->name.GetString();
  json::Value& data = doc[key.data()];

  // OnRspQryInvestor
  boost::regex re_qry("^OnRspQry(.*)$");
  boost::smatch mat;
  if (boost::regex_match(key, mat, re_qry)) {
    std::string t_name = mat[1];

    boost::regex re_field("^CThostFtdc(.*)Field$");
    for (auto itr = data.MemberBegin();
         itr != data.MemberEnd(); ++itr) {
      boost::smatch mat;
      std::string key = itr->name.GetString();
      if (boost::regex_match(key, mat, re_field)) {
        json::Value& f_data = data[key.data()];

        std::string create_sql;
        std::string insert_sql;
        sqlString(t_name, f_data, create_sql, insert_sql);
        // QATAR_DEBUG <<"create sql: " <<create_sql;
        // QATAR_DEBUG <<"insert sql: " <<insert_sql;

        try {
          (*db_) <<create_sql <<cppdb::exec;
          (*db_) <<insert_sql <<cppdb::exec;
        } catch (std::exception const &e) {
          QATAR_ERROR <<e.what();
        }
      }
    }

    if (data["is_last"].GetBool()) {
      notify();
    }
  }
}

void Server::sqlString(const std::string& name,
                       json::Value& data,
                       std::string& create_sql,
                       std::string& insert_sql) {
  QATAR_TRACE <<"Server::sqlString()";

  create_sql = "CREATE TABLE IF NOT EXISTS " + name + " (";

  insert_sql = "INSERT INTO " + name + " VALUES(";

  bool first = true;
  for (auto itr = data.MemberBegin();
       itr != data.MemberEnd(); ++itr) {
    if (!first) {
      create_sql += ", ";
      insert_sql += ", ";
    } else {
      first = false;
    }
    create_sql += itr->name.GetString();
    create_sql += " TEXT";

    insert_sql += "'";
    insert_sql += itr->value.GetString();
    insert_sql += "'";
  }
  create_sql += ");";
  insert_sql += ");";

  return;
}

void Server::go() {
  QATAR_TRACE <<"Server::go()";

  wait(1000);
  queryExchange();
  wait();

  wait(1000);
  queryInstrument();
  wait();

  wait(1000);
  queryInvestor();
  wait();

  wait(1000);
  queryAccount();
  wait();

  wait(1000);
  queryTradingCode();
  wait();

  wait(1000);
  queryOrder();
  wait();

  wait(1000);
  queryTrade();
  wait();

  wait(1000);
  queryPosition();
  wait();
}

void Server::queryExchange() {
  QATAR_TRACE <<"Server::queryExchange()";

  json::Document doc;

  json::Value v_queryExchange;
  json::addMember<const std::string&>(&v_queryExchange, "exchange", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryExchange", v_queryExchange);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryInstrument() {
  QATAR_TRACE <<"Server::queryInstrument()";

  json::Document doc;

  json::Value v_queryInstrument;
  json::addMember<const std::string&>(&v_queryInstrument, "instru", "", &doc);
  json::addMember<const std::string&>(&v_queryInstrument, "exchange", "", &doc);
  json::addMember<const std::string&>(&v_queryInstrument, "exchange_instru_id", "", &doc);
  json::addMember<const std::string&>(&v_queryInstrument, "product_id", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryInstrument", v_queryInstrument);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryInvestor() {
  QATAR_TRACE <<"Server::queryInvestor()";

  json::Document doc;

  json::Value v_queryInvestor;
  json::addMember<const json::Value&>(&doc, "queryInvestor", v_queryInvestor);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryAccount() {
  QATAR_TRACE <<"Server::queryAccount()";

  json::Document doc;

  json::Value v_queryAccount;
  json::addMember<const std::string&>(&v_queryAccount, "currency", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryAccount", v_queryAccount);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryTradingCode() {
  QATAR_TRACE <<"Server::queryTradingCode()";

  json::Document doc;

  json::Value v_queryTradingCode;
  json::addMember<const std::string&>(&v_queryTradingCode, "exchange", "", &doc);
  json::addMember<const std::string&>(&v_queryTradingCode, "client_id", "", &doc);
  json::addMember<const std::string&>(&v_queryTradingCode, "client_id_type", "1", &doc);

  json::addMember<const json::Value&>(&doc, "queryTradingCode", v_queryTradingCode);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryOrder() {
  QATAR_TRACE <<"Server::queryOrder()";

  json::Document doc;

  json::Value v_queryOrder;
  json::addMember<const std::string&>(&v_queryOrder, "instru", "", &doc);
  json::addMember<const std::string&>(&v_queryOrder, "exchange", "", &doc);
  json::addMember<const std::string&>(&v_queryOrder, "order_sys_id", "", &doc);
  json::addMember<const std::string&>(&v_queryOrder, "start_time", "", &doc);
  json::addMember<const std::string&>(&v_queryOrder, "stop_time", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryOrder", v_queryOrder);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryTrade() {
  QATAR_TRACE <<"Server::queryTrade()";

  json::Document doc;

  json::Value v_queryTrade;
  json::addMember<const std::string&>(&v_queryTrade, "instru", "", &doc);
  json::addMember<const std::string&>(&v_queryTrade, "exchange", "", &doc);
  json::addMember<const std::string&>(&v_queryTrade, "trade_id", "", &doc);
  json::addMember<const std::string&>(&v_queryTrade, "start_time", "", &doc);
  json::addMember<const std::string&>(&v_queryTrade, "stop_time", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryTrade", v_queryTrade);

  push_service_->sendMsg(json::toString(doc));
}

void Server::queryPosition() {
  QATAR_TRACE <<"Server::queryPosition()";

  json::Document doc;

  json::Value v_queryPosition;
  json::addMember<const std::string&>(&v_queryPosition, "instru", "", &doc);

  json::addMember<const json::Value&>(&doc, "queryPosition", v_queryPosition);

  push_service_->sendMsg(json::toString(doc));
}

};  // namespace qatar
