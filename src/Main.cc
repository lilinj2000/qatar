// Copyright (c) 2010
// All rights reserved.

#include <memory>
#include "Server.hh"
#include "soil/Log.hh"
#include "soil/json.hh"
// #include "soil/Pause.hh"

int main(int argc, char* argv[]) {
  try {
    rapidjson::Document doc;
    soil::json::load_from_file(&doc, "qatar.json");
    soil::log::init(doc);

    std::unique_ptr<qatar::Server> server;
    server.reset(new qatar::Server(doc));
  } catch (std::exception& e) {
    SOIL_ERROR("Error: {}", e.what());

    return -1;
  }

  return 0;
}
