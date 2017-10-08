// Copyright (c) 2010
// All rights reserved.

#include "Options.hh"

namespace qatar {

using soil::json::get_item_value;

Options::Options(
    const rapidjson::Document& doc) {
  get_item_value(&dbconn_str, doc, "/qatar/dbconn_str");
  get_item_value(&sub_addr, doc, "/qatar/sub_addr");
  return;
}

};  // namespace qatar
