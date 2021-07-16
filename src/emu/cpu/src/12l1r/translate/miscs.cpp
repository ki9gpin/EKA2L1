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

#include <cpu/12l1r/arm_visitor.h>
#include <cpu/12l1r/block_gen.h>
#include <cpu/12l1r/thumb_visitor.h>
#include <cpu/12l1r/visit_session.h>

namespace eka2l1::arm::r12l1 {
    bool arm_translate_visitor::arm_CLZ(common::cc_flags cond, reg_index d, reg_index m) {
        if (!condition_passed(cond)) {
            return false;
        }

        common::armgen::arm_reg source_real = reg_index_to_gpr(m);
        common::armgen::arm_reg dest_real = reg_index_to_gpr(d);

        common::armgen::arm_reg source_mapped = reg_supplier_.map(source_real, 0);
        common::armgen::arm_reg dest_mapped = reg_supplier_.map(dest_real, ALLOCATE_FLAG_DIRTY);

        big_block_->CLZ(dest_mapped, source_mapped);
        return true;
    }

    bool arm_translate_visitor::arm_UXTH(common::cc_flags cond, reg_index d, std::uint8_t rotate_base_8, reg_index m) {
        if (!condition_passed(cond)) {
            return false;
        }
        common::armgen::arm_reg source_real = reg_index_to_gpr(m);
        common::armgen::arm_reg dest_real = reg_index_to_gpr(d);

        common::armgen::arm_reg source_mapped = reg_supplier_.map(source_real, 0);
        common::armgen::arm_reg dest_mapped = reg_supplier_.map(dest_real, ALLOCATE_FLAG_DIRTY);

        big_block_->UXTH(dest_mapped, source_mapped, rotate_base_8);
        return true;
    }

    bool arm_translate_visitor::arm_SEL(common::cc_flags cond, reg_index n, reg_index d, reg_index m) {
        if (!condition_passed(cond)) {
            return false;
        }
        common::armgen::arm_reg dest_real = reg_index_to_gpr(d);
        common::armgen::arm_reg source1_real = reg_index_to_gpr(n);
        common::armgen::arm_reg source2_real = reg_index_to_gpr(m);

        common::armgen::arm_reg source1_mapped = reg_supplier_.map(source1_real, 0);
        common::armgen::arm_reg source2_mapped = reg_supplier_.map(source2_real, 0);
        common::armgen::arm_reg dest_mapped = reg_supplier_.map(dest_real, ALLOCATE_FLAG_DIRTY);

        big_block_->SEL(dest_mapped, source1_mapped, source2_mapped);
        return true;
    }

    bool arm_translate_visitor::arm_PKHBT(common::cc_flags cond, reg_index n, reg_index d, std::uint8_t imm5, reg_index m) {
        if (!condition_passed(cond)) {
            return false;
        }
        common::armgen::arm_reg dest_real = reg_index_to_gpr(d);
        common::armgen::arm_reg source1_real = reg_index_to_gpr(n);
        common::armgen::arm_reg source2_real = reg_index_to_gpr(m);

        common::armgen::arm_reg source1_mapped = reg_supplier_.map(source1_real, 0);
        common::armgen::arm_reg source2_mapped = reg_supplier_.map(source2_real, 0);
        common::armgen::arm_reg dest_mapped = reg_supplier_.map(dest_real, ALLOCATE_FLAG_DIRTY);

        big_block_->PKHBT(dest_mapped, source1_mapped, source2_mapped, imm5);
        return true;
    }

    bool arm_translate_visitor::arm_PKHTB(common::cc_flags cond, reg_index n, reg_index d, std::uint8_t imm5, reg_index m) {
        if (!condition_passed(cond)) {
            return false;
        }
        common::armgen::arm_reg dest_real = reg_index_to_gpr(d);
        common::armgen::arm_reg source1_real = reg_index_to_gpr(n);
        common::armgen::arm_reg source2_real = reg_index_to_gpr(m);

        common::armgen::arm_reg source1_mapped = reg_supplier_.map(source1_real, 0);
        common::armgen::arm_reg source2_mapped = reg_supplier_.map(source2_real, 0);
        common::armgen::arm_reg dest_mapped = reg_supplier_.map(dest_real, ALLOCATE_FLAG_DIRTY);

        big_block_->PKHTB(dest_mapped, source1_mapped, source2_mapped, imm5);
        return true;
    }
}