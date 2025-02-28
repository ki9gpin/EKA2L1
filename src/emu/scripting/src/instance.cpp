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

#include <mutex>
#include <scripting/instance.h>

namespace eka2l1::scripting {
    eka2l1::system *instance;
    std::mutex scripting_mut;

    void set_current_instance(eka2l1::system *sys) {
        std::lock_guard<std::mutex> guard(scripting_mut);
        instance = sys;
    }

    eka2l1::system *get_current_instance() {
        return instance;
    }
}