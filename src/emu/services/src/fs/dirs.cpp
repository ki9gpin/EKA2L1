/*
 * Copyright (c) 2019 EKA2L1 Team
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

#include <services/fs/fs.h>
#include <services/fs/std.h>

#include <system/epoc.h>
#include <kernel/kernel.h>
#include <vfs/vfs.h>

#include <common/algorithm.h>
#include <common/cvt.h>
#include <common/log.h>
#include <common/path.h>

#include <utils/err.h>

namespace eka2l1 {
    void fs_server_client::open_dir(service::ipc_context *ctx) {
        auto dir = ctx->get_argument_value<std::u16string>(0);
        std::optional<epoc::uid_type> utype = ctx->get_argument_data_from_descriptor<epoc::uid_type>(2);

        LOG_TRACE(SERVICE_EFSRV, "Opening directory: {}", common::ucs2_to_utf8(*dir));

        if (!dir || !utype.has_value()) {
            ctx->complete(epoc::error_argument);
            return;
        }

        const std::int32_t attrib_raw = *ctx->get_argument_value<std::int32_t>(1);
        std::uint32_t attrib = io_attrib_include_file | io_attrib_include_dir;

        bool check_other_flag = false;
        bool is_exclude = false;

        if (attrib_raw & epoc::fs::entry_att_match_exclude) {
            is_exclude = true;
            check_other_flag = true;
        } else if (attrib_raw & epoc::fs::entry_att_match_exclusive) {
            check_other_flag = true;
        }

        if (check_other_flag) {
            if (attrib_raw & epoc::fs::entry_att_archive) {
                attrib = is_exclude ? (attrib & ~io_attrib_include_file) : (attrib | io_attrib_include_file);
            }

            if (attrib_raw & epoc::fs::entry_att_dir) {
                attrib = is_exclude ? (attrib & ~io_attrib_include_dir) : (attrib | io_attrib_include_dir);
            }
        }

        if (attrib_raw & epoc::fs::entry_att_allow_uid) {
            attrib |= io_attrib_allow_uid;
            attrib &= ~io_attrib_include_dir;
        }

        fs_node *node = server<fs_server>()->make_new<fs_node>();
        node->vfs_node = ctx->sys->get_io_system()->open_dir(*dir, utype.value(), attrib);

        if (!node->vfs_node) {
            ctx->complete(epoc::error_path_not_found);
            server<fs_server>()->remove(node);
            return;
        }

        size_t dir_handle = obj_table_.add(node);
        int dir_handle_i = static_cast<int>(dir_handle);

        LOG_TRACE(SERVICE_EFSRV, "UID requested: 0x{:X}, 0x{:X}, 0x{:X}", utype->uid1, utype->uid2, utype->uid3);

        ctx->write_data_to_descriptor_argument<int>(3, dir_handle_i);
        ctx->complete(epoc::error_none);
    }

    void fs_server_client::close_dir(service::ipc_context *ctx) {
        std::optional<std::int32_t> handle_res = ctx->get_argument_value<std::int32_t>(3);

        if (!handle_res) {
            ctx->complete(epoc::error_argument);
            return;
        }

        fs_node *node = get_file_node(*handle_res);

        if (node == nullptr || node->vfs_node->type != io_component_type::dir) {
            ctx->complete(epoc::error_bad_handle);
            return;
        }

        obj_table_.remove(*handle_res);
        ctx->complete(epoc::error_none);
    }

    void fs_server_client::read_dir(service::ipc_context *ctx) {
        std::optional<std::int32_t> handle = ctx->get_argument_value<std::int32_t>(3);

        if (!handle) {
            ctx->complete(epoc::error_argument);
            return;
        }

        fs_node *dir_node = obj_table_.get<fs_node>(*handle);

        if (!dir_node || dir_node->vfs_node->type != io_component_type::dir) {
            ctx->complete(epoc::error_bad_handle);
            return;
        }

        io_system *io = ctx->sys->get_io_system();
        directory *dir = reinterpret_cast<directory *>(dir_node->vfs_node.get());

        epoc::fs::entry entry;
        std::optional<entry_info> info = dir->get_next_entry();

        if (!info) {
            ctx->complete(epoc::error_eof);
            return;
        }

        epoc::fs::build_symbian_entry_from_emulator_entry(io, info.value(), entry);

        ctx->write_data_to_descriptor_argument<epoc::fs::entry>(0, entry);
        ctx->complete(epoc::error_none);
    }

    void fs_server_client::read_dir_packed(service::ipc_context *ctx) {
        std::optional<std::int32_t> handle = ctx->get_argument_value<std::int32_t>(3);
        std::optional<std::int32_t> entry_arr_vir_ptr = ctx->get_argument_value<std::int32_t>(0);

        if (!handle || !entry_arr_vir_ptr) {
            ctx->complete(epoc::error_argument);
            return;
        }

        fs_node *dir_node = obj_table_.get<fs_node>(*handle);

        if (!dir_node || dir_node->vfs_node->type != io_component_type::dir) {
            ctx->complete(epoc::error_bad_handle);
            return;
        }

        directory *dir = reinterpret_cast<directory *>(dir_node->vfs_node.get());

        kernel::process *own_pr = ctx->msg->own_thr->owning_process();

        epoc::des8 *entry_arr = ptr<epoc::des8>(*entry_arr_vir_ptr).get(own_pr);
        epoc::buf_des<char> *entry_arr_buf = reinterpret_cast<epoc::buf_des<char> *>(entry_arr);

        std::uint8_t *entry_buf = reinterpret_cast<std::uint8_t *>(entry_arr->get_pointer(own_pr));
        std::uint8_t *entry_buf_end = entry_buf + entry_arr_buf->max_length;
        std::uint8_t *entry_buf_org = entry_buf;

        size_t queried_entries = 0;

        // 4 is for info (length + descriptor type)
        size_t entry_no_name_size = epoc::fs::entry_standard_size + 4 + 8;

        kernel_system *kern = ctx->sys->get_kernel_system();
        io_system *io = ctx->sys->get_io_system();

        const bool should_support_64bit_size = kern->get_epoc_version() >= epocver::epoc95;

        while (entry_buf < entry_buf_end) {
            epoc::fs::entry entry;
            std::optional<entry_info> info = dir->peek_next_entry();

            if (!info) {
                entry_arr->set_length(own_pr, static_cast<std::uint32_t>(entry_buf - entry_buf_org));
                LOG_TRACE(SERVICE_EFSRV, "Queried entries: 0x{:x}", queried_entries);
                ctx->complete(epoc::error_eof);

                return;
            }

            if (entry_buf + entry_no_name_size + common::align(common::utf8_to_ucs2(info->name).length() * 2, 4) + 4 > entry_buf_end) {
                break;
            }

            epoc::fs::build_symbian_entry_from_emulator_entry(io, info.value(), entry);
            const std::uint32_t entry_write_size = epoc::fs::entry_standard_size + 4;

            memcpy(entry_buf, &entry, entry_write_size);
            entry_buf += entry_write_size;

            memcpy(entry_buf, &entry.name.data[0], info->name.length() * 2);
            entry_buf += common::align(info->name.length() * 2, 4);

            if (should_support_64bit_size) {
                // Epoc10 uses two reserved bytes
                memcpy(entry_buf, &entry.size_high, 8);
                entry_buf += 8;
            }

            queried_entries += 1;
            dir->get_next_entry();
        }

        entry_arr->set_length(own_pr, static_cast<std::uint32_t>(entry_buf - entry_buf_org));

        LOG_TRACE(SERVICE_EFSRV, "Queried entries: 0x{:x}", queried_entries);

        ctx->complete(epoc::error_none);
    }
}
