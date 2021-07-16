/*
 * Copyright (c) 2018 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project 
 * (see bentokun.github.com/EKA2L1).
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <common/types.h>

#include <loader/sis_fields.h>
#include <loader/sis_old.h>

#include <string>
#include <vector>

#include <cstdint>
#include <optional>

namespace eka2l1 {
    namespace loader {
        enum sis_type {
            sis_type_old,
            sis_type_new,
            sis_type_new_stub
        };

        // Identify epoc version from the SIS
        std::optional<sis_type> identify_sis_type(const std::string &path);
        sis_contents parse_sis(const std::string &path, const bool is_stub);
    }
}
