// Copyright (c) 2010
// All rights reserved.

#ifndef QATAR_DBSERVICE_HH
#define QATAR_DBSERVICE_HH

#include <string>
#include <vector>
#include <map>

#include "cppdb/frontend.h"
#include "soil/json.hh"
#include "soil/ReaderWriterQueue.hh"

namespace qatar {

typedef std::vector<std::string> InstrusType;

class DBService :
      public soil::MsgCallback<std::string> {
 public:
  explicit DBService(
      const std::string& dbconn_str,
      int32_t batch_size = 1);

  virtual ~DBService();

  virtual void onMsg(
      std::shared_ptr<std::string> msg);

  virtual void onStart();

  virtual void onStop();

  void pushMsg(const std::string& msg) {
    if (!msg.empty()) {
      std::shared_ptr<std::string> p(new std::string(msg));
      queue_->pushMsg(p);
    }
  }

  void fetchInstrus(
      InstrusType* instrus);

 protected:
  void sqlString(
      const std::string& t_name,
      const rapidjson::Value& data,
      std::string* create_sql,
      std::string* insert_sql);

  void parseDoc(
      const rapidjson::Document& doc);

  void fieldType(
    const rapidjson::Value& data,
    std::string* type);

  void doInsert(
      cppdb::statement stat,
      const rapidjson::Value& data);

 private:
  std::unique_ptr<cppdb::session> db_;
  std::unique_ptr<soil::ReaderWriterQueue<std::string> > queue_;

  std::map<std::string,
           cppdb::statement> sqls_;

  int32_t batch_size_;
  int32_t cur_records_;
};

};  // namespace qatar

#endif
