/*
    kitsu_api
    Copyright (C) 2019  Michael de Lang

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

#pragma once

#include <string>
#include <memory>
#include <vector>

using namespace std;

namespace kitsu {
    struct anime {
        string en_title;
        string jp_title;
        string avg_rating;
        string start_date;
        string end_date;
        string slug;
        int episodes;
        int episode_length;
        string image_url;
    };

    class kitsu_response_message {
    public:
        explicit kitsu_response_message(vector<anime> animes) noexcept;

        ~kitsu_response_message() noexcept = default;

        static unique_ptr<kitsu_response_message> deserialize(string const &data);

        vector<anime> animes;
    };
}