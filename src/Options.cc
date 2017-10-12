// Copyright (c) 2010
// All rights reserved.

#include "Options.hh"

namespace qatar {

using soil::json::get_item_value;

Options::Options(
    const rapidjson::Document& doc) {
  get_item_value(&db, doc, "/qatar/db");
  get_item_value(&sub_addr, doc, "/qatar/sub_addr");
  get_item_value(&push_addr, doc, "/qatar/push_addr");
  return;
}

};  // namespace qatar
