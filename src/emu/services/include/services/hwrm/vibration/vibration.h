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

#include <services/hwrm/resource.h>
#include <drivers/hwrm/vibration.h>

namespace eka2l1 {
    class kernel_system;
}

namespace eka2l1::epoc {
    /**
     * \brief The vibration resource.
     * 
     * The class manages Symbian's vibration high-levely by mapping vibration to the host.
     */
    struct vibration_resource : public resource_interface {
        std::unique_ptr<drivers::hwrm::vibrator> viber_;
        kernel_system *kern_;

        void vibrate_with_default_intensity(service::ipc_context &ctx);
        void vibrate_cleanup(service::ipc_context &ctx);

        explicit vibration_resource(kernel_system *kern);
        void execute_command(service::ipc_context &ctx) override;
    };
}