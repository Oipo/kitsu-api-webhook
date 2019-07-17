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

#define BANNED_ERROR_CODE -2
#define GENERIC_ERROR_CODE -1

using namespace std;

namespace kitsu {
    class error_response_message {
    public:
        error_response_message(int error_number, string error_str) noexcept;

        ~error_response_message() noexcept = default;

        string const serialize() const;
        static unique_ptr<error_response_message> deserialize(string const &data);

        int error_number;
        string error_str;
    };
}