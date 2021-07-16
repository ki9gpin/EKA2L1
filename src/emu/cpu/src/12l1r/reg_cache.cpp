/*
 * Copyright (c) 2020 EKA2L1 Team.
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

#include <cpu/12l1r/block_gen.h>
#include <cpu/12l1r/core_state.h>
#include <cpu/12l1r/reg_cache.h>

#include <common/log.h>

namespace eka2l1::arm::r12l1 {
    // A lot of these code algorithm are heavily revised from PPSSPP. Thank you! <3 T_T
    static std::uint32_t get_offset_to_reg_in_core_state(const std::uint32_t index) {
        const std::size_t gpr_offset = offsetof(core_state, gprs_[0]);
        return gpr_offset + index * sizeof(std::uint32_t);
    }

    reg_cache::reg_cache(dashixiong_block *bblock)
        : time_(0)
        , big_block_(bblock) {
    }

    void reg_cache::copy_state(common::armgen::arm_reg state_reg) {
        // Save the CPSR
        big_block_->STR(CPSR_REG, state_reg, offsetof(core_state, cpsr_));

        // Save GPRs
        for (common::armgen::arm_reg reg = common::armgen::R0; reg < common::armgen::R15; reg = static_cast<common::armgen::arm_reg>(reg + 1)) {
            guest_register_info &info = guest_gpr_infos_[reg];
            const std::uint32_t offset_gpr_mem = get_offset_to_reg_in_core_state(reg - common::armgen::R0);

            switch (info.curr_location_) {
            case GUEST_REGISTER_LOC_HOST_REG:
                big_block_->STR(info.host_reg_, state_reg, offset_gpr_mem);
                break;

            case GUEST_REGISTER_LOC_MEM: {
                big_block_->LDR(ALWAYS_SCRATCH1, CORE_STATE_REG, offset_gpr_mem);
                big_block_->STR(ALWAYS_SCRATCH1, state_reg, offset_gpr_mem);
                break;
            }

            case GUEST_REGISTER_LOC_IMM:
                big_block_->MOVI2R(ALWAYS_SCRATCH1, info.imm_);
                big_block_->STR(ALWAYS_SCRATCH1, state_reg, offset_gpr_mem);
                break;

            default:
                LOG_ERROR(CPU_12L1R, "No method to copy register R{} to another state", reg - common::armgen::R0);
                break;
            }
        }

        // TODO: Store FPCSR and ExtRegs too!
    }

    bool reg_cache::load_gpr_to_host(common::armgen::arm_reg dest_reg, common::armgen::arm_reg source_guest_reg) {
        if ((dest_reg < common::armgen::R0) || (dest_reg >= common::armgen::R15)) {
            LOG_ERROR(CPU_12L1R, "Invalid register to load from!");
            return false;
        }

        guest_register_info &info = guest_gpr_infos_[source_guest_reg];

        switch (info.curr_location_) {
        case GUEST_REGISTER_LOC_HOST_REG:
            big_block_->MOV(dest_reg, info.host_reg_);
            break;

        case GUEST_REGISTER_LOC_MEM: {
            big_block_->LDR(dest_reg, CORE_STATE_REG, get_offset_to_reg_in_core_state(source_guest_reg - common::armgen::R0));
            break;
        }

        case GUEST_REGISTER_LOC_IMM:
            big_block_->MOVI2R(dest_reg, info.imm_);
            break;

        default:
            return false;
        }

        return true;
    }

    void reg_cache::flush_gpr(const common::armgen::arm_reg mee) {
        if ((mee < common::armgen::R0) || (mee >= common::armgen::R15)) {
            LOG_ERROR(CPU_12L1R, "Invalid register to flush!");
            return;
        }

        guest_register_info &info = guest_gpr_infos_[mee];
        switch (info.curr_location_) {
        case GUEST_REGISTER_LOC_IMM:
            // Store it? Hmmmm
            big_block_->MOVI2R(ALWAYS_SCRATCH1, info.imm_);
            big_block_->STR(ALWAYS_SCRATCH1, CORE_STATE_REG, get_offset_to_reg_in_core_state(mee - common::armgen::R0));

            break;

        case GUEST_REGISTER_LOC_IMM_AND_HOST_REG:
        case GUEST_REGISTER_LOC_HOST_REG:
            // Just safe check... May it ever happen?
            if (!host_gpr_infos_[info.host_reg_].scratch_ && host_gpr_infos_[info.host_reg_].dirty_) {
                big_block_->STR(info.host_reg_, CORE_STATE_REG, get_offset_to_reg_in_core_state(mee - common::armgen::R0));
            }

            if (!info.spill_lock_) {
                host_gpr_infos_[info.host_reg_].guest_mapped_reg_ = common::armgen::INVALID_REG;
            }

            host_gpr_infos_[info.host_reg_].dirty_ = false;
            break;

        case GUEST_REGISTER_LOC_MEM:
            break;

        default:
            LOG_ERROR(CPU_12L1R, "Invalid current register for guest reg: {}", static_cast<int>(mee));
            return;
        }

        info.curr_location_ = GUEST_REGISTER_LOC_MEM;
        info.host_reg_ = common::armgen::INVALID_REG;
        info.last_use_ = 0;
        info.imm_ = 0;
    }

    void reg_cache::flush(const common::armgen::arm_reg guest_mee) {
        if (guest_mee >= common::armgen::R0 && guest_mee < common::armgen::R15) {
            flush_gpr(guest_mee);
        } else {
            LOG_ERROR(CPU_12L1R, "Unsupported register flush type!");
        }
    }

    void reg_cache::flush_all() {
        // Intentionally missing out R15. It's not something that needs flush lmaoooo
        for (int i = 0; i < 15; i++) {
            flush(static_cast<common::armgen::arm_reg>(common::armgen::R0 + i));
        }
    }

    void reg_cache::flush_host_gpr(const common::armgen::arm_reg mee) {
        if ((mee < common::armgen::R0) || (mee >= common::armgen::R15)) {
            LOG_ERROR(CPU_12L1R, "Invalid register to flush!");
            return;
        }

        if ((!host_gpr_infos_[mee].scratch_) && (host_gpr_infos_[mee].dirty_) && (host_gpr_infos_[mee].guest_mapped_reg_ != common::armgen::INVALID_REG)) {
            const common::armgen::arm_reg guest_reg = host_gpr_infos_[mee].guest_mapped_reg_;
            if (guest_gpr_infos_[guest_reg].host_reg_ != mee) {
                LOG_ERROR(CPU_12L1R, "Host and guest register out of sync (reg=R{})", static_cast<int>(mee));
                return;
            }

            // Store it
            big_block_->STR(mee, CORE_STATE_REG, get_offset_to_reg_in_core_state(guest_reg - common::armgen::R0));

            guest_gpr_infos_[guest_reg].curr_location_ = GUEST_REGISTER_LOC_MEM;
            guest_gpr_infos_[guest_reg].host_reg_ = common::armgen::INVALID_REG;
            guest_gpr_infos_[guest_reg].imm_ = 0;
            guest_gpr_infos_[guest_reg].last_use_ = 0;

            host_gpr_infos_[mee].guest_mapped_reg_ = common::armgen::INVALID_REG;
        }
    }

    void reg_cache::flush_host_reg(const common::armgen::arm_reg host_reg) {
        if ((host_reg >= common::armgen::R0) && (host_reg < common::armgen::R15)) {
            flush_host_gpr(host_reg);
        } else {
            LOG_WARN(CPU_12L1R, "Invalid or unsupported host register to flush!");
        }
    }

    void reg_cache::flush_host_regs_for_host_call() {
        flush_host_reg(common::armgen::R1);
        flush_host_reg(common::armgen::R2);
        flush_host_reg(common::armgen::R3);
        flush_host_reg(common::armgen::R12);
    }

    common::armgen::arm_reg reg_cache::allocate_or_spill(const std::uint32_t flags) {
        time_++;

        const common::armgen::arm_reg *allocation_order = ALLOCATEABLE_GPRS;
        std::size_t allocation_order_length = sizeof(ALLOCATEABLE_GPRS) / sizeof(common::armgen::arm_reg);

        guest_register_info *guest_rf_arr = guest_gpr_infos_;
        host_register_info *host_rf_arr = host_gpr_infos_;

        // Check if these host registers currently have yet a guest register occupied
        // And it must not be scratch... Because the register may currently got occupied
        for (std::size_t i = 0; i < allocation_order_length; i++) {
            if ((host_rf_arr[allocation_order[i]].guest_mapped_reg_ == common::armgen::INVALID_REG)
                && !host_rf_arr[allocation_order[i]].scratch_) {
                if (flags & ALLOCATE_FLAG_SCRATCH) {
                    host_rf_arr[allocation_order[i]].scratch_ = true;
                }

                if (flags & ALLOCATE_FLAG_DIRTY) {
                    host_rf_arr[allocation_order[i]].dirty_ = true;
                }

                // It's free real estate
                return allocation_order[i];
            }
        }

        // There seems to be nothing unfortunately
        // Try to discard some host register being used by a guest arm register
        // This already exclude the PC register
        std::uint32_t least_use = 0xFFFFFFFF;
        std::uint32_t least_use_index = 0xFFFFFFFF;

        for (std::uint32_t i = 0; i < 15; i++) {
            if ((least_use > guest_rf_arr[i].last_use_) && (guest_rf_arr[i].curr_location_ == GUEST_REGISTER_LOC_HOST_REG) && !guest_rf_arr[i].spill_lock_) {
                least_use = guest_rf_arr[i].last_use_;
                least_use_index = i;
            }
        }

        if (least_use_index == 0xFFFFFFFF) {
            LOG_ERROR(CPU_12L1R, "Can't find anything to spill on");
            return common::armgen::INVALID_REG;
        }

        common::armgen::arm_reg result_reg = common::armgen::INVALID_REG;
        common::armgen::arm_reg host_result_reg = guest_rf_arr[least_use_index].host_reg_;

        // Flush the register down
        result_reg = static_cast<common::armgen::arm_reg>(common::armgen::R0 + least_use_index);
        flush(result_reg);

        if (flags & ALLOCATE_FLAG_SCRATCH) {
            host_rf_arr[host_result_reg].scratch_ = true;
        }

        if (flags & ALLOCATE_FLAG_DIRTY) {
            host_rf_arr[host_result_reg].dirty_ = true;
        }

        return host_result_reg;
    }

    common::armgen::arm_reg reg_cache::map(const common::armgen::arm_reg mee, const std::uint32_t allocate_flags) {
        guest_register_info *guest_rf_arr = guest_gpr_infos_;
        host_register_info *host_rf_arr = host_gpr_infos_;

        if (guest_rf_arr[mee].curr_location_ & GUEST_REGISTER_LOC_HOST_REG) {
            // Wait.. we also want to check if it's synced
            common::armgen::arm_reg host_reg = guest_rf_arr[mee].host_reg_;

            if (host_rf_arr[host_reg].guest_mapped_reg_ != mee) {
                LOG_ERROR(CPU_12L1R, "Host and guest register out of sync!");
            }

            // TODO: Handle IMM and Reg
            if (allocate_flags & ALLOCATE_FLAG_DIRTY) {
                host_rf_arr[host_reg].dirty_ = true;
            }

            guest_rf_arr[mee].curr_location_ = GUEST_REGISTER_LOC_HOST_REG;

            // Increase the use count
            guest_rf_arr[mee].last_use_ = time_++;
            return host_reg;
        }

        common::armgen::arm_reg new_baby = allocate_or_spill(allocate_flags);

        if (new_baby == common::armgen::INVALID_REG) {
            return new_baby;
        }

        // Load the register data to it
        if (!load_gpr_to_host(new_baby, mee)) {
            LOG_WARN(CPU_12L1R, "Loading register value to mapped failed!");
        }

        // Update the info about the register
        host_rf_arr[new_baby].guest_mapped_reg_ = mee;

        guest_rf_arr[mee].curr_location_ = GUEST_REGISTER_LOC_HOST_REG;
        guest_rf_arr[mee].host_reg_ = new_baby;
        guest_rf_arr[mee].last_use_ = time_;

        return new_baby;
    }

    common::armgen::arm_reg reg_cache::scratch() {
        return allocate_or_spill(ALLOCATE_FLAG_SCRATCH);
    }

    void reg_cache::spill_lock(const common::armgen::arm_reg guest_reg) {
        if ((guest_reg >= common::armgen::R0) && (guest_reg < common::armgen::R15)) {
            guest_gpr_infos_[guest_reg].spill_lock_ = true;
        } else {
            LOG_ERROR(CPU_12L1R, "Invalid register to spill lock!");
        }
    }

    void reg_cache::spill_lock_all() {
        for (auto &gpr_info : guest_gpr_infos_) {
            gpr_info.spill_lock_ = true;
        }
    }

    void reg_cache::release_spill_lock(const common::armgen::arm_reg guest_reg) {
        if ((guest_reg >= common::armgen::R0) && (guest_reg < common::armgen::R15)) {
            guest_gpr_infos_[guest_reg].spill_lock_ = false;
        } else {
            LOG_ERROR(CPU_12L1R, "Invalid register to release spill lock!");
        }
    }

    void reg_cache::release_spill_lock_all() {
        for (auto &gpr_info : guest_gpr_infos_) {
            gpr_info.spill_lock_ = false;
        }
    }

    void reg_cache::done_scratching_this(common::armgen::arm_reg mee) {
        if ((mee >= common::armgen::R0) && (mee < common::armgen::R15)) {
            host_gpr_infos_[mee].scratch_ = false;
        } else if ((mee >= common::armgen::S0) && (mee <= common::armgen::S23)) {
            host_gpr_infos_[mee].scratch_ = false;
        } else {
            LOG_ERROR(CPU_12L1R, "Invalid register to release scratch!");
        }
    }

    void reg_cache::done_scratching() {
        for (auto &gpr_info : host_gpr_infos_) {
            gpr_info.scratch_ = false;
        }
    }

    void reg_cache::force_scratch(common::armgen::arm_reg mee) {
        if ((mee >= common::armgen::R0) && (mee <= common::armgen::R15)) {
            if (host_gpr_infos_[mee].guest_mapped_reg_ != common::armgen::INVALID_REG) {
                common::armgen::arm_reg guest_reg = host_gpr_infos_[mee].guest_mapped_reg_;
                if (guest_gpr_infos_[guest_reg].spill_lock_) {
                    LOG_ERROR(CPU_12L1R, "The guest register being used by R{} is currently being spill locked",
                        static_cast<int>(guest_reg));

                    assert(false);
                } else {
                    flush(host_gpr_infos_[mee].guest_mapped_reg_);
                }
            }

            host_gpr_infos_[mee].guest_mapped_reg_ = common::armgen::INVALID_REG;
            host_gpr_infos_[mee].scratch_ = true;
        } else {
            LOG_ERROR(CPU_12L1R, "Unsupported force scratch!");
        }
    }
}