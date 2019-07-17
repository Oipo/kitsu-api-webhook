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

using namespace std;

namespace kitsu {
    struct author {
        string id;
        string username;
        string name;
    };

    struct channel {
        string id;
        string name;
        string type;
    };

    struct guild {
        string id;
        string name;
    };

    class get_pyra_request_message {
    public:
        get_pyra_request_message(struct author author, struct channel channel, struct guild guild, string command, string arguments) noexcept;

        ~get_pyra_request_message() noexcept = default;

        static unique_ptr<get_pyra_request_message> deserialize(string const &data);

        struct author author;
        struct channel channel;
        struct guild guild;
        string command;
        string arguments;

    };
}