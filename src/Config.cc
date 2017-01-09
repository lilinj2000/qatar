// Copyright (c) 2010
// All rights reserved.

#include <fstream>
#include <iostream>
#include "boost/program_options.hpp"

#include "Config.hh"
#include "Log.hh"

namespace qatar {

Options::Options():
    config_options_("QatarConfigOptions") {
  namespace po = boost::program_options;

  config_options_.add_options()
      ("qatar.connection_string", po::value<std::string>(&connection_string),
       "db connection string")
      ("qatar.push_addr", po::value<std::string>(&push_addr),
       "push address")
      ("subject.filter", po::value<std::string>(&filter),
       "filter")
      ("subject.sub_addr", po::value<std::string>(&sub_addr),
       "sub address")

      ("qatar.log_cfg", po::value<std::string>(&log_cfg),
       "log config file");

  return;
}

Options::~Options() {
}

po::options_description* Options::configOptions() {
  return &config_options_;
}

Config::Config(int argc, char* argv[]) {
  options_.reset(new Options());

  std::unique_ptr<soil::Config> config(soil::Config::create());
  config->registerOptions(options_.get());

  config->configFile() = "qatar.cfg";
  config->loadConfig(argc, argv);

  // init the log
  QATAR_LOG_INIT(options_->log_cfg);

  return;
}

Config::~Config() {
}

};  // namespace qatar
