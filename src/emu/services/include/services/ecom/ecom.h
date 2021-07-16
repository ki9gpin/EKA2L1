/*
 * Copyright (c) 2019 EKA2L1 Team.
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

#pragma once

#include <services/ecom/plugin.h>
#include <services/framework.h>
#include <common/uid.h>

#include <string>
#include <vector>

namespace eka2l1 {
    class io_system;

    namespace epoc::fs {
        struct entry;
    }

    enum ecom_opcodes {
        ecom_notify_on_change,
        ecom_cancel_notify_on_change,
        ecom_list_implementations,
        ecom_list_resolved_implementations,
        ecom_list_custom_resolved_implementations,
        ecom_collect_implementations_list,
        ecom_get_implementation_creation_method,
        ecom_get_resolved_creation_method,
        ecom_get_custom_resolved_creation_method,
        ecom_destroyed_implementation,
        ecom_enable_implementation,
        ecom_list_extended_interfaces,
        ecom_set_get_params
    };

    struct ecom_list_impl_param {
        std::int32_t match_type;
        std::int32_t buffer_size;
        std::int32_t cap_check;
    };

    struct ecom_resolver_params {
        std::string match_string_;
        std::vector<epoc::uid> extended_interfaces_;
    };

    struct ecom_session : public service::typical_session {
        enum {
            FLAG_ECOM_OLD_ABI = 1 << 0
        };

        std::vector<ecom_implementation_info_ptr> collected_impls_;
        std::uint32_t flags_;

    protected:
        void do_get_resolved_impl_creation_method(service::ipc_context *ctx);

        bool get_implementation_buffer(std::uint8_t *buf, const std::size_t buf_size,
            const bool support_extended_interface);

        void list_implementations(service::ipc_context *ctx);
        void get_implementation_creation_method(service::ipc_context *ctx);
        void collect_implementation_list(service::ipc_context *ctx);

        bool get_resolver_params(service::ipc_context *ctx, const int index, ecom_resolver_params &params);
        bool unpack_match_str_and_extended_interfaces(std::string &data, std::string &match_str,
            std::vector<std::uint32_t> &extended_interfaces);

    public:
        explicit ecom_session(service::typical_server *svr, kernel::uid client_ss_uid, epoc::version client_ver);
        void fetch(service::ipc_context *ctx) override;

        const bool is_using_old_ecom_abi() const {
            return flags_ & FLAG_ECOM_OLD_ABI;
        }

        void set_using_old_ecom_abi(const bool result) {
            flags_ &= ~FLAG_ECOM_OLD_ABI;

            if (result) {
                flags_ |= FLAG_ECOM_OLD_ABI;
            }
        }
    };

    class ecom_server : public service::typical_server {
        std::unordered_map<std::uint32_t, ecom_interface_info> interfaces;

        // Storing implementations pointer for implementation look-up only. Costly
        // to use unordered_map.
        // We may need to reconsider this.
        std::vector<ecom_implementation_info_ptr> implementations;

        bool init{ false };

    protected:
        bool register_implementation(const std::uint32_t interface_uid, ecom_implementation_info_ptr &impl);

        bool load_plugins(eka2l1::io_system *io);
        bool load_and_install_plugin_from_buffer(const std::u16string &name, std::uint8_t *buf, const std::size_t size,
            const drive_number drv);

        bool load_plugin_on_drive(eka2l1::io_system *io, const drive_number drv);

        /*
         * \brief Search the ROM and ROFS for an archive of plugins.
         *
         * Archive are SPI file. They usually has pattern of ecom-*-*.spi or
         * ecom-*-*.sXX where XX is language code.
         * 
         * This searchs these files on Z:\Private\10009d8f
         * 
         * \returns A vector contains all canidates.
         */
        std::vector<std::string> get_ecom_plugin_archives(eka2l1::io_system *io);

        /*! \brief Load archives
         */
        bool load_archives(eka2l1::io_system *io);

        void connect(service::ipc_context &ctx) override;

    public:
        explicit ecom_server(eka2l1::system *sys);

        /**
         * \brief Get interface info of a given UID.
         * \returns The pointer to the interface info, null means not found.
         */
        ecom_interface_info *get_interface(const epoc::uid interface_uid);

        bool get_resolved_implementations(std::vector<ecom_implementation_info_ptr> &collect_vector, const epoc::uid interface_uid, const ecom_resolver_params &params,
            const bool generic_wildcard_match);
        
        bool construct_impl_creation_method(kernel::thread *requester, ecom_implementation_info *info, epoc::fs::entry &dll_entry,
            epoc::uid &dtor_key, std::int32_t *err, const bool check_cap_comp = true);

        bool get_implementation_dll_info(kernel::thread *requester, const epoc::uid interface_uid,
            const epoc::uid impl_uid, epoc::fs::entry &dll_entry, epoc::uid &dtor_key, std::int32_t *err,
            const bool check_cap_comp = true);

        bool get_resolved_impl_dll_info(kernel::thread *requester, const epoc::uid interface_uid, const ecom_resolver_params &params, const bool match_type,
            epoc::fs::entry &dll_entry, epoc::uid &dtor_key, std::int32_t *err, const bool check_cap_comp = true);
    };
}
