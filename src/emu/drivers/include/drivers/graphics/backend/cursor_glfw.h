/*
 * Copyright (c) 2021 EKA2L1 Team.
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

#include <drivers/graphics/cursor.h>
#include <GLFW/glfw3.h>

namespace eka2l1::drivers {
    class cursor_glfw: public cursor {
        GLFWcursor *cursor_;

    public:
        explicit cursor_glfw(GLFWcursor *cursor);
        ~cursor_glfw() override;

        void *raw_handle() override {
            return cursor_;
        }
    };

    class cursor_controller_glfw: public cursor_controller {
    public:
        std::unique_ptr<cursor> create(const cursor_type type) override;
    };
}
