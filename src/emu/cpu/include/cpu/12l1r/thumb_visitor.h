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

#pragma once

#include <common/armemitter.h>
#include <cpu/12l1r/common.h>
#include <cpu/12l1r/visit_session.h>

namespace eka2l1::arm::r12l1 {
    class thumb_translate_visitor : public visit_session {
    public:
        using instruction_return_type = bool;

        explicit thumb_translate_visitor(dashixiong_block *bro, translated_block *crr);

        bool thumb16_UDF();

        // Data processing
        bool thumb16_MOV_imm(reg_index d, std::uint8_t imm8);
        bool thumb16_MOV_reg(bool d_hi, reg_index m, reg_index d_lo);
        bool thumb16_MVN_reg(reg_index m, reg_index d);
        bool thumb16_CMP_imm(reg_index n, std::uint8_t imm8);
        bool thumb16_CMP_reg_t1(reg_index m, reg_index n);
        bool thumb16_CMP_reg_t2(bool n_hi, reg_index m, reg_index n_lo);
        bool thumb16_CMN_reg(reg_index m, reg_index n);
        bool thumb16_ADD_imm_t1(std::uint8_t imm3, reg_index n, reg_index d);
        bool thumb16_ADD_imm_t2(reg_index d_n, std::uint8_t imm8);
        bool thumb16_ADD_reg_t1(reg_index m, reg_index n, reg_index d);
        bool thumb16_ADD_reg_t2(bool d_n_hi, reg_index m, reg_index d_n_lo);
        bool thumb16_ADD_sp_t1(reg_index d, std::uint8_t imm8);
        bool thumb16_ADD_sp_t2(std::uint8_t imm7);
        bool thumb16_SUB_imm_t1(std::uint8_t imm3, reg_index n, reg_index d);
        bool thumb16_SUB_imm_t2(reg_index d_n, std::uint8_t imm8);
        bool thumb16_SUB_sp(std::uint8_t imm7);
        bool thumb16_SUB_reg(reg_index m, reg_index n, reg_index d);
        bool thumb16_LSL_imm(std::uint8_t imm5, reg_index m, reg_index d);
        bool thumb16_LSR_imm(std::uint8_t imm5, reg_index m, reg_index d);
        bool thumb16_ASR_imm(std::uint8_t imm5, reg_index m, reg_index d);
        bool thumb16_AND_reg(reg_index m, reg_index d_n);
        bool thumb16_ORR_reg(reg_index m, reg_index d_n);
        bool thumb16_ADR(reg_index d, std::uint8_t imm8);
        bool thumb16_RSB_imm(reg_index n, reg_index d);
        bool thumb16_BIC_reg(reg_index m, reg_index d_n);
        bool thumb16_EOR_reg(reg_index m, reg_index d_n);
        bool thumb16_ASR_reg(reg_index m, reg_index d_n);
        bool thumb16_LSR_reg(reg_index m, reg_index d_n);
        bool thumb16_LSL_reg(reg_index m, reg_index d_n);
        bool thumb16_TST_reg(reg_index m, reg_index n);
        bool thumb16_ADC_reg(reg_index m, reg_index d_n);
        bool thumb16_SBC_reg(reg_index m, reg_index d_n);
        bool thumb16_ROR_reg(reg_index m, reg_index d_n);

        // Extension instructions
        bool thumb16_SXTH(reg_index m, reg_index d);
        bool thumb16_SXTB(reg_index m, reg_index d);
        bool thumb16_UXTH(reg_index m, reg_index d);
        bool thumb16_UXTB(reg_index m, reg_index d);

        bool thumb16_PUSH(bool m, reg_list list);
        bool thumb16_POP(bool p, reg_list list);
        bool thumb16_LDMIA(reg_index n, reg_list list);
        bool thumb16_STMIA(reg_index n, reg_list list);
        bool thumb16_LDR_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_LDR_literal(reg_index t, std::uint8_t imm8);
        bool thumb16_LDR_imm_t1(std::uint8_t imm5, reg_index n, reg_index t);
        bool thumb16_LDR_imm_t2(reg_index t, std::uint8_t imm8);
        bool thumb16_LDRB_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_LDRB_imm(std::uint8_t imm5, reg_index n, reg_index t);
        bool thumb16_LDRH_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_LDRH_imm(std::uint8_t imm5, reg_index n, reg_index t);
        bool thumb16_LDRSH_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_LDRSB_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_STR_imm_t1(std::uint8_t imm5, reg_index n, reg_index t);
        bool thumb16_STR_imm_t2(reg_index t, std::uint8_t imm8);
        bool thumb16_STRB_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_STRB_imm(std::uint8_t imm5, reg_index n, reg_index t);
        bool thumb16_STR_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_STRH_reg(reg_index m, reg_index n, reg_index t);
        bool thumb16_STRH_imm(std::uint8_t imm5, reg_index n, reg_index t);

        // Branch
        bool thumb16_B_t1(common::cc_flags cond, std::uint8_t imm8);
        bool thumb16_B_t2(std::uint16_t imm11);
        bool thumb16_BLX_reg(reg_index m);

        // Multiply
        bool thumb16_MUL_reg(reg_index n, reg_index d_m);

        // thumb32
        bool thumb32_BL_imm(std::uint16_t hi, std::uint16_t lo);
        bool thumb32_BLX_imm(std::uint16_t hi, std::uint16_t lo);
        bool thumb16_BX(reg_index m);

        bool thumb16_BKPT();
        bool thumb32_UDF();
    };
}