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

#include "pyra_response_message.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace kitsu;
using namespace rapidjson;

pyra_response_message::pyra_response_message(vector<response> responses) noexcept
        : responses(move(responses)) {}

string const pyra_response_message::serialize() const {
    StringBuffer sb;
    Writer<StringBuffer> writer(sb);

    writer.StartArray();

    for(auto& response : responses) {
        writer.StartObject();

        writer.String("message");
        writer.String(response.message.c_str(), response.message.size());

        if(!response.image_url.empty()) {
            writer.String("imageUrl");
            writer.String(response.image_url.c_str(), response.image_url.size());
        }

        writer.EndObject();
    }

    writer.EndArray();
    return sb.GetString();
}