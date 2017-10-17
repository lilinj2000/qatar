// Copyright (c) 2010
// All rights reserved.

#include <memory>
#include "gtest/gtest.h"
#include "src/DBService.hh"
#include "soil/Log.hh"

namespace qatar {

TEST(DBServiceTest, perfTest) {
  try {
    rapidjson::Document doc;
    soil::json::load_from_file(&doc, "utest.json");
    soil::log::init(doc);

    std::string db;
    soil::json::get_item_value(
        &db,
        doc,
        "/utest/db");
    std::string dbconn_str = fmt::format(
      "sqlite3:db={};@pool_size=16",
      db);

    int batch_size;
    soil::json::get_item_value(
        &batch_size,
        doc,
        "/utest/batch_size");
    SOIL_INFO("batch_size: {}", batch_size);

    std::string data_file;
    soil::json::get_item_value(
        &data_file,
        doc,
        "/utest/data_file");
    rapidjson::Document data_doc;
    soil::json::load_from_file(&data_doc, data_file);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    data_doc.Accept(writer);
    std::string data = buffer.GetString();

    int iterations;
    soil::json::get_item_value(
        &iterations,
        doc,
        "/utest/iterations");
    SOIL_INFO("iterations: {}", iterations);

    DBService service(dbconn_str, batch_size);
    for (int i = 0; i < iterations; ++i) {
      service.pushMsg(data);
    }
  } catch(std::exception& e) {
    SOIL_ERROR("Error: {}", e.what());
    FAIL();

    return;
  }

  SUCCEED();
}


}  // namespace qatar
