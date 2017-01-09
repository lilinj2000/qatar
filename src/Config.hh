// Copyright (c) 2010
// All rights reserved.

#ifndef QATAR_CONFIG_HH
#define QATAR_CONFIG_HH

#include <string>
#include <memory>
#include "soil/Config.hh"

namespace qatar {

namespace po = boost::program_options;

class Options : public soil::Options {
 public:
  Options();

  virtual ~Options();

  virtual po::options_description* configOptions();

  std::string connection_string;

  std::string push_addr;

  // subject
  std::string filter;
  std::string sub_addr;

  std::string log_cfg;

 private:
  boost::program_options::options_description config_options_;
};

class Config {
 public:
  explicit Config(int argc = 0, char* argv[] = nullptr);
  ~Config();

  Options* options() {
    return options_.get();
  }

 private:
  std::unique_ptr<Options> options_;
};

}  // namespace qatar
#endif
