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

#include "kitsu_response_message.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

using namespace kitsu;
using namespace rapidjson;

kitsu_response_message::kitsu_response_message(vector<anime> animes) noexcept
        : animes(move(animes)) {}

unique_ptr<kitsu_response_message> kitsu_response_message::deserialize(string const &data) {
    if(data.empty() || data.length() < 4) {
        spdlog::warn("[kitsu_response_message] deserialize encountered empty buffer");
        return nullptr;
    }

    Document d;
    d.Parse(data.c_str());

    if (d.HasParseError() || !d.IsObject() || !d.HasMember("data") || !d["data"].IsArray()) {
        spdlog::warn("[kitsu_response_message] deserialize failed");
        return nullptr;
    }

    auto& kitsu_data = d["data"];

    vector<anime> kitsu_animes;

    for(SizeType i = 0; i < kitsu_data.Size(); i++) {
        anime animuh;
        spdlog::trace("Parsing listing {}", i);
        auto& kitsu_animuh = kitsu_data[i]["attributes"];

        if(!kitsu_animuh.IsObject()) {
            spdlog::error("listing {} is not an object", i);
            return nullptr;
        }

        if(kitsu_animuh.HasMember("titles")) {
            animuh.en_title = kitsu_animuh["titles"].HasMember("en") && kitsu_animuh["titles"]["en"].IsString() ? kitsu_animuh["titles"]["en"].GetString() : "";
            if(animuh.en_title.empty()) {
                animuh.en_title = kitsu_animuh["titles"].HasMember("en_jp") && kitsu_animuh["titles"]["en_jp"].IsString() ? kitsu_animuh["titles"]["en_jp"].GetString() : "unknown";
            }
            animuh.jp_title = kitsu_animuh["titles"].HasMember("ja_jp") && kitsu_animuh["titles"]["ja_jp"].IsString() ? kitsu_animuh["titles"]["ja_jp"].GetString() : "unknown";
        }
        animuh.avg_rating = kitsu_animuh.HasMember("averageRating") && kitsu_animuh["averageRating"].IsString() ?  kitsu_animuh["averageRating"].GetString() : "unknown";
        animuh.start_date = kitsu_animuh.HasMember("startDate") && kitsu_animuh["startDate"].IsString() ? kitsu_animuh["startDate"].GetString() : "unknown";
        animuh.end_date = kitsu_animuh.HasMember("endDate") && kitsu_animuh["endDate"].IsString() ? kitsu_animuh["endDate"].GetString() : "unknown";
        animuh.slug = kitsu_animuh.HasMember("slug") && kitsu_animuh["slug"].IsString() ? kitsu_animuh["slug"].GetString() : "unknown";
        animuh.episodes = kitsu_animuh.HasMember("episodeCount") && kitsu_animuh["episodeCount"].IsInt() ? kitsu_animuh["episodeCount"].GetInt() : 0;
        animuh.episode_length = kitsu_animuh.HasMember("episodeLength") && kitsu_animuh["episodeLength"].IsInt() ? kitsu_animuh["episodeLength"].GetInt() : 0;
        if(kitsu_animuh.HasMember("coverImage") && kitsu_animuh["coverImage"].IsObject()) {
            animuh.image_url = kitsu_animuh["coverImage"]["small"].GetString();
        } else if(kitsu_animuh.HasMember("posterImage") && kitsu_animuh["posterImage"].IsObject()) {
            animuh.image_url = kitsu_animuh["posterImage"]["small"].GetString();
        } else {
            animuh.image_url = "";
        }
        kitsu_animes.emplace_back(move(animuh));
    }

    return make_unique<kitsu_response_message>(kitsu_animes);
}