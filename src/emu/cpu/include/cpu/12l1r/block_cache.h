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

#include <common/algorithm.h>
#include <common/armemitter.h>
#include <cpu/12l1r/common.h>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <vector>

namespace eka2l1::arm::r12l1 {
    struct block_link {
        std::vector<common::armgen::fixup_branch> needs_;

        bool linked_;
        vaddress to_;

        std::uint32_t *value_;

        explicit block_link();
    };

    struct translated_block {
        using hash_type = std::uint64_t;

        hash_type hash_;

        std::uint32_t size_;
        std::uint32_t last_inst_size_;

        const std::uint8_t *translated_code_;

        std::size_t translated_size_;
        std::uint32_t inst_count_;

        bool thumb_;

        std::vector<block_link> links_;

        vaddress start_address() const {
            return static_cast<vaddress>(hash_);
        }

        asid address_space() const {
            return static_cast<asid>((hash_ >> 32) & 0xFFFF);
        }

        vaddress current_address() const {
            return static_cast<vaddress>(hash_) + size_;
        }

        vaddress current_aligned_address() const {
            return common::align(current_address(), 4, 0);
        }

        explicit translated_block(const vaddress start_addr, const asid aid);
        block_link &get_or_add_link(const vaddress addr, const int link_pri = -1);
    };

    using on_block_invalidate_callback_type = std::function<void(translated_block *)>;

    class block_cache {
        using translated_block_inst = std::unique_ptr<translated_block>;
        using translated_block_key = std::pair<vaddress, asid>;

        std::map<translated_block_key, translated_block_inst> blocks_;
        on_block_invalidate_callback_type invalidate_callback_;

    public:
        explicit block_cache();

        bool add_block(const vaddress start_addr, const asid aid);

        // The block that is returned by this is consistent in memory
        translated_block *lookup_block(const vaddress start_addr, const asid aid);

        void flush_range(const vaddress range_start, const vaddress range_end, const asid aid);
        void flush_all();

        void set_on_block_invalidate_callback(on_block_invalidate_callback_type cb) {
            invalidate_callback_ = cb;
        }
    };

    translated_block::hash_type make_block_hash(const vaddress start_addr, const asid aid);
}