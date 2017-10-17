// Copyright (c) 2010
// All rights reserved.

#include "DBService.hh"
#include "soil/Log.hh"
#include "boost/regex.hpp"

namespace qatar {

DBService::DBService(
    const std::string& dbconn_str,
    int32_t batch_size) :
    batch_size_(batch_size),
    cur_records_(0) {
  SOIL_FUNC_TRACE;

  db_.reset(new cppdb::session(dbconn_str));

  queue_.reset(new soil::ReaderWriterQueue<std::string>(this));
}

DBService::~DBService() {
  SOIL_FUNC_TRACE;
}

void DBService::onMsg(
    std::shared_ptr<std::string> msg) {
  SOIL_FUNC_TRACE;
  SOIL_DEBUG_PRINT(*msg);

  rapidjson::Document doc;
  if (doc.Parse(*msg).HasParseError()) {
    SOIL_DEBUG_PRINT(soil::json::get_parse_error(doc, *msg));
    return;
  }

  if (cur_records_ == 0) {
    db_->begin();
  }

  parseDoc(doc);

  if (++cur_records_ >= batch_size_) {
    db_->commit();

    cur_records_ = 0;
  }
}

void DBService::onStart() {
}

void DBService::onStop() {
  if (cur_records_ != 0) {
    db_->commit();
  }
}

void DBService::parseDoc(
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

void DBService::fieldType(
    const rapidjson::Value& data,
    std::string* type) {
  if (data.IsString()) {
    (*type) = "TEXT";
  } else if (data.IsDouble()) {
    (*type) = "REAL";
  } else if (data.IsNumber()
             || data.IsBool()) {
    (*type) = "INTEGER";
  }
}

void DBService::doInsert(
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

void DBService::sqlString(
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

    std::string type;
    fieldType(itr->value, &type);
    (*create_sql) += itr->name.GetString();
    (*create_sql) += " " + type;
    (*insert_sql) += "?";
  }
  (*create_sql) += ");";
  (*insert_sql) += ");";

  return;
}

void DBService::fetchInstrus(
    InstrusType* instrus) {
  SOIL_FUNC_TRACE;

  try {
    std::string sql = "SELECT DISTINCT InstrumentID FROM Instrument";

    cppdb::result res = (*db_) <<sql;

    while (res.next()) {
      std::string instru;
      res >> instru;
      instrus->push_back(instru);
    }
  } catch (std::exception const &e) {
    SOIL_ERROR("ERROR: {}", e.what());

    throw;
  }
}

};  // namespace qatar
