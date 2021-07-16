/*
 * Copyright (c) 2018 EKA2L1 Team
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

#include <common/types.h>

#include <kernel/server.h>
#include <utils/dll.h>

namespace eka2l1 {
    const std::string get_loader_server_name_through_epocver(const epocver ver);

    class loader_server : public service::server {
        /*! \brief Parse a E32 Image/ Rom Image, and use informations from parsing to spawn a new process.
         * 
         * arg0: TLdrInfo contains spawn request
         * arg1: Contains path to the image.
         * arg2: Contains extra argument for the process.
         *
         * request_status: Error code.
        */
        void load_process(service::ipc_context &context);

        /*! \brief Parse a E32 Image / Rom Image, and use informations from parsing to spawn a new library.
         * 
         * arg0: TLdrInfo contains spawn request
         * arg1: Contains library name.
         * arg2: Contains library path.
         *
         * request_status: New Process handle.
        */
        void load_library(service::ipc_context &context);

        /*! \brief Get library/executable info (uids, security layer, capatibilities, etc..). 
         *
         * arg0: TLdrInfo that will contain library/executable general info
         * arg1: Name
         * arg2: Buffer contains external info
         */
        void get_info(service::ipc_context &context);

        void get_info_from_header(service::ipc_context &context);

        void delete_loader(service::ipc_context &context);

        void check_library_hash(service::ipc_context &context);

    public:
        explicit loader_server(system *sys);
    };
}
