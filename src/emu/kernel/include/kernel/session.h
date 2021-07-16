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

#pragma once

#include <kernel/kernel_obj.h>
#include <kernel/thread.h>

#include <kernel/ipc.h>

#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace eka2l1 {
    using thread_ptr = kernel::thread *;

    namespace service {
        class server;
    }

    using server_ptr = service::server *;

    namespace service {
        /*! \brief An IPC session 
		 *
         *  A session is a bridge between server and client.
         *  We can send message to the server and receive result.
         *  Server can be in different process and different thread.
        */
        class session : public kernel::kernel_obj {
            friend class server;

            server_ptr svr;

            std::vector<std::pair<bool, ipc_msg_ptr>> msgs_pool;
            ipc_msg_ptr disconnect_msg_;

            common::roundabout in_progress_msgs_; 

            kernel::address cookie_address;
            kernel::handle associated_handle;

            bool headless_;
            share_mode shmode_;

        protected:
            int send(ipc_msg_ptr msg);
            ipc_msg_ptr get_free_msg();

            bool eligible_to_send(kernel::thread *thr);
            void send_destruct();

        public:
            explicit session(kernel_system *kern, server_ptr svr, int async_slot_count);
            ~session() override;

            server_ptr get_server() {
                return svr;
            }

            void destroy() override;
            void detatch(const int code);

            int send_receive_sync(const int function, const ipc_arg &args, eka2l1::ptr<epoc::request_status> request_sts);
            int send_receive(const int function, const ipc_arg &args, eka2l1::ptr<epoc::request_status> request_sts);

            void set_cookie_address(const uint32_t addr) {
                cookie_address = addr;
            }

            void set_associated_handle(const kernel::handle h) {
                associated_handle = h;
            }

            const share_mode get_share_mode() const {
                return shmode_;
            }

            void set_share_mode(const share_mode shmode);

            const kernel::handle get_associated_handle() const {
                return associated_handle;
            }

            bool is_server_terminated() const {
                return svr == nullptr;
            }

            void set_slot_free(ipc_msg *msg);
        };
    }
}