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

#include "get_pyra_request_message.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace kitsu;
using namespace rapidjson;

get_pyra_request_message::get_pyra_request_message(struct author author, struct channel channel, struct guild guild, string command, string arguments) noexcept
        : author(move(author)), channel(move(channel)), guild(move(guild)), command(move(command)), arguments(move(arguments)) {}

unique_ptr<get_pyra_request_message> get_pyra_request_message::deserialize(string const &data) {
    if(data.empty() || data.length() < 4) {
        spdlog::warn("[get_pyra_request_message] deserialize encountered empty buffer");
        return nullptr;
    }

    Document d;
    d.Parse(data.c_str());
    spdlog::trace("[get_pyra_request_message] parsing \"{}\"", data);

    if (d.HasParseError() || !d.IsObject() || !d.HasMember("author") || !d.HasMember("channel") || !d.HasMember("guild") || !d.HasMember("command") || !d.HasMember("arguments")) {
        spdlog::warn("[get_pyra_request_message] deserialize failed");
        return nullptr;
    }

    struct author auth;
    auth.id = d["author"]["id"].GetString();
    auth.username = d["author"]["username"].GetString();
    auth.name = d["author"]["name"].GetString();

    struct channel ch;
    ch.id = d["channel"]["id"].GetString();
    ch.name = d["channel"].HasMember("name") && d["channel"]["name"].IsString() ? d["channel"]["name"].GetString() : "";
    ch.type = d["channel"]["type"].GetString();

    struct guild g;
    if(d.HasMember("guild") && d["guild"].IsObject()) {
        g.id = d["guild"]["id"].GetString();
        g.name = d["guild"]["name"].GetString();
    }

    string command = d["command"].GetString();
    string arguments = d["arguments"].GetString();

    return make_unique<get_pyra_request_message>(auth, ch, g, command, arguments);
}