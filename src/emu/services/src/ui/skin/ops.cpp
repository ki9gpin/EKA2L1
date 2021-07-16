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

#include <services/ui/skin/ops.h>

namespace eka2l1::epoc {
    const char *akn_skin_server_opcode_to_str(const akn_skin_server_opcode op) {
        switch (op) {
#define OPCODE(op)             \
    case akn_skin_server_##op: \
        return "akn_server::" #op;
#define OPCODE2(op, num)       \
    case akn_skin_server_##op: \
        return "akn_server::" #op;
#include <services/ui/skin/ops.def>
#undef OPCODE
#undef OPCODE2

        default:
            break;
        }

        return nullptr;
    }
}
