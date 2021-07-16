/*
 * Copyright (c) 2019 EKA2L1 Team
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

#include <services/hwrm/resource.h>

namespace eka2l1 {
    class kernel_system;
}

namespace eka2l1::epoc {
    /**
     * \brief Class manages emulated device's lighting.
     */
    struct light_resource : public resource_interface {
        kernel_system *kern_;

        explicit light_resource(kernel_system *kern);

        void turn_on(service::ipc_context &ctx);
        void get_supported_targets(service::ipc_context &ctx);
        void cleanup(service::ipc_context &ctx);
        void execute_command(service::ipc_context &ctx) override;
    };
}