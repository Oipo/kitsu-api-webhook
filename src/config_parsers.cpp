/*
    kitsu_api
    Copyright (C) 2019 Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config_parsers.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using namespace std;
using namespace kitsu;
using json = nlohmann::json;

optional<config> kitsu::parse_env_file() {
    string env_contents;
    ifstream env("config.json");

    if(!env) {
        spdlog::error("[main] no config.json file found. Please make one.");
        return {};
    }

    env.seekg(0, ios::end);
    env_contents.resize(env.tellg());
    env.seekg(0, ios::beg);
    env.read(&env_contents[0], env_contents.size());
    env.close();

    spdlog::trace(R"([main] config.json file contents: {})", env_contents);

    auto env_json = json::parse(env_contents);
    config config;

    try {
        config.debug_level = env_json["DEBUG_LEVEL"];
    } catch (const exception& e) {
        spdlog::error("[main] DEBUG_LEVEL missing in config.json file.");
        return {};
    }

    try {
        config.address = env_json["ADDRESS"];
    } catch (const exception& e) {
        spdlog::error("[main] CLIENT_ID missing in config.json file.");
        return {};
    }

    try {
        config.port = env_json["PORT"];
    } catch (const exception& e) {
        spdlog::error("[main] CLIENT_ID missing in config.json file.");
        return {};
    }

    try {
        config.client_id = env_json["CLIENT_ID"];
    } catch (const exception& e) {
        spdlog::error("[main] CLIENT_ID missing in config.json file.");
        return {};
    }

    try {
        config.client_secret = env_json["CLIENT_SECRET"];
    } catch (const exception& e) {
        spdlog::error("[main] CLIENT_SECRET missing in config.json file.");
        return {};
    }

    try {
        config.threads = env_json["THREADS"];
    } catch (const exception& e) {
        spdlog::error("[main] THREADS missing in config.json file.");
        return {};
    }

    return config;
}