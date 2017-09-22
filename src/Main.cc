// Copyright (c) 2010
// All rights reserved.

#include <memory>
#include "Server.hh"
#include "soil/Log.hh"
#include "soil/json.hh"
// #include "soil/Pause.hh"

int main(int argc, char* argv[]) {
  rapidjson::Document doc;
  soil::json::load_from_file(&doc, "qatar.json");
  soil::log::init(doc);

  std::unique_ptr<qatar::Server> server;
  server.reset(new qatar::Server(doc));

  // std::unique_ptr<soil::Pause> pause(soil::Pause::create());

  return 0;
}
