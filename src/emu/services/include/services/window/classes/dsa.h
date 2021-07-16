/*
 * Copyright (c) 2020 EKA2L1 Team
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

#include <services/window/classes/wsobj.h>
#include <utils/reqsts.h>

#include <queue>
#include <string>

namespace eka2l1::kernel {
    class msg_queue;
    class thread;
}

namespace eka2l1::epoc {
    struct screen;
    struct window_user;

    struct dsa : public window_client_obj {
        window_user *husband_; ///< Mmmhhh
        kernel::msg_queue *dsa_must_abort_queue_; ///< Queue to notify clients that DSA operation must be stopped.
        kernel::msg_queue *dsa_complete_queue_; ///< Queue for client to report completion of a DSA operation.

        kernel::thread *sync_thread_;           ///< Thread used for syncing with window operations.
        eka2l1::ptr<epoc::request_status> sync_status_;

        epoc::notify_info dsa_must_stop_notify_;

        enum state {
            state_none,
            state_prepare,
            state_running,
            state_completed
        } state_;

        explicit dsa(window_server_client_ptr client);
        ~dsa() override;

        void do_cancel();

        void get_sync_info(service::ipc_context &ctx, ws_cmd &cmd);
        void request_access(eka2l1::service::ipc_context &ctx, eka2l1::ws_cmd &cmd);
        void get_region(eka2l1::service::ipc_context &ctx, eka2l1::ws_cmd &cmd);
        void cancel(eka2l1::service::ipc_context &ctx, eka2l1::ws_cmd &cmd);
        void free(eka2l1::service::ipc_context &context, eka2l1::ws_cmd &cmd);

        bool execute_command(eka2l1::service::ipc_context &ctx, eka2l1::ws_cmd &cmd) override;
    };
}
