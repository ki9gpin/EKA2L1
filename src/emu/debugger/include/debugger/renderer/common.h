/*
 * Copyright (c) 2019 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project.
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

#include <drivers/graphics/common.h>
#include <string>

namespace eka2l1::drivers {
    class graphics_driver;
    class graphics_command_list_builder;
}

namespace eka2l1::renderer {
    drivers::handle load_texture_from_file(drivers::graphics_driver *driver, drivers::graphics_command_list_builder *builder,
        const std::string &path, const bool as_bitmap = false, int *width = nullptr, int *height = nullptr);

    // Load with standalone builder
    drivers::handle load_texture_from_file_standalone(drivers::graphics_driver *driver, const std::string &path, const bool as_bitmap = false, int *width = nullptr,
        int *height = nullptr);
}