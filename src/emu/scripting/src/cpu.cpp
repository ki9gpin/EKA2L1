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

#include <scripting/cpu.h>
#include <scripting/instance.h>

#include <cpu/arm_interface.h>
#include <system/epoc.h>

namespace eka2l1::scripting {
    uint32_t cpu::get_register(const int index) {
        return get_current_instance()->get_cpu()->get_reg(index);
    }

    void cpu::set_register(const int index, const std::uint32_t value) {
        get_current_instance()->get_cpu()->set_reg(index, value);
    }

    uint32_t cpu::get_cpsr() {
        return get_current_instance()->get_cpu()->get_cpsr();
    }

    uint32_t cpu::get_pc() {
        return get_current_instance()->get_cpu()->get_pc();
    }

    uint32_t cpu::get_sp() {
        return get_current_instance()->get_cpu()->get_sp();
    }

    uint32_t cpu::get_lr() {
        return get_current_instance()->get_cpu()->get_lr();
    }
}

extern "C" {
    EKA2L1_EXPORT std::uint32_t symemu_cpu_get_reg(const int idx) {
        return eka2l1::scripting::cpu::get_register(idx);
    }

    EKA2L1_EXPORT std::uint32_t symemu_cpu_get_cpsr() {
        return eka2l1::scripting::cpu::get_cpsr();
    }

    EKA2L1_EXPORT std::uint32_t symemu_cpu_get_pc() {
        return eka2l1::scripting::cpu::get_pc();
    }

    EKA2L1_EXPORT std::uint32_t symemu_cpu_get_sp() {
        return eka2l1::scripting::cpu::get_sp();
    }

    EKA2L1_EXPORT std::uint32_t symemu_cpu_get_lr() {
        return eka2l1::scripting::cpu::get_lr();
    }

    EKA2L1_EXPORT void symemu_cpu_set_reg(const int idx, std::uint32_t value) {
        eka2l1::scripting::cpu::set_register(idx, value);
    }
}