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
#include <cpu/arm_factory.h>
#include <common/platform.h>

#include <cpu/dyncom/arm_dyncom.h>

#if EKA2L1_ARCH(ARM)
#include <cpu/12l1r/arm_12l1r.h>
#else
#include <cpu/arm_dynarmic.h>
#endif

#include <cpu/12l1r/exclusive_monitor.h>

namespace eka2l1::arm {
    core_instance create_core(exclusive_monitor *monitor, arm_emulator_type arm_type) {
        switch (arm_type) {
        case arm_emulator_type::unicorn:
            return nullptr;

#if EKA2L1_ARCH(ARM)
        case arm_emulator_type::r12l1:
            return std::make_unique<r12l1_core>(monitor, 12);
#else
        case arm_emulator_type::dynarmic:
            return std::make_unique<dynarmic_core>(monitor);
#endif

        case arm_emulator_type::dyncom:
            return std::make_unique<dyncom_core>(monitor, 12);

        default:
            break;
        }

        return nullptr;
    }

    exclusive_monitor_instance create_exclusive_monitor(arm_emulator_type arm_type, const std::size_t core_count) {
        switch (arm_type) {
        case arm_emulator_type::unicorn:
            return nullptr;

        case arm_emulator_type::dyncom:
            return std::make_unique<r12l1::exclusive_monitor>(core_count);

#if EKA2L1_ARCH(ARM)
        case arm_emulator_type::r12l1:
            return std::make_unique<r12l1::exclusive_monitor>(core_count);
#else
        case arm_emulator_type::dynarmic:
            return std::make_unique<dynarmic_exclusive_monitor>(core_count);
#endif

        default:
            break;
        }

        return nullptr;
    }
}