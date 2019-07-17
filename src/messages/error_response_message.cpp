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

#include "error_response_message.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace kitsu;
using namespace rapidjson;

error_response_message::error_response_message(int error_number, string error_str) noexcept
        : error_number(error_number), error_str(move(error_str)) {}

string const error_response_message::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartObject();

    writer.String("error_number");
    writer.Int(error_number);

    writer.String("error_str");
    writer.String(error_str.c_str(), error_str.size());

    writer.EndObject();
    return sb.GetString();
}

unique_ptr<error_response_message> error_response_message::deserialize(string const &data) {
    if(data.empty() || data.length() < 4) {
        spdlog::warn("[error_response_message] deserialize encountered empty buffer");
        return nullptr;
    }

    Document d;
    d.Parse(data.c_str());

    if (d.HasParseError() || !d.IsObject() || !d.HasMember("error_number") || ! d.HasMember("error_str")) {
        spdlog::warn("[error_response_message] deserialize failed");
        return nullptr;
    }

    int error_number = d["error_number"].GetInt();
    string error_str = d["error_str"].GetString();

    return make_unique<error_response_message>(error_number, error_str);
}