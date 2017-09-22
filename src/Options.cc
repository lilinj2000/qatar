// Copyright (c) 2010
// All rights reserved.

#include "Options.hh"

namespace qatar {

using soil::json::get_item_value;

Options::Options(
    const rapidjson::Document& doc) {
  get_item_value(&dbconn_str, doc, "/qatar/dbconn_str");

  return;
}

};  // namespace qatar
