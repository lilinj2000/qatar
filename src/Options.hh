// Copyright (c) 2010
// All rights reserved.

#ifndef QATAR_OPTIONS_HH
#define QATAR_OPTIONS_HH

#include <string>
#include "soil/json.hh"

namespace qatar {

class Options {
 public:
  explicit Options(
      const rapidjson::Document& doc);

  std::string dbconn_str;
  std::string sub_addr;
  std::string push_addr;
};

}  // namespace qatar

#endif
