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

#include <cstdint>

namespace eka2l1::epoc::hwrm::vibration {
    /**
     * \brief The status of vibration.
     */
    enum status {
        status_unk = 0, ///< Unknown status.
        status_not_allowed = 1, ///< Vibration is not allowed on the device currently.
        status_stopped = 2, ///< The vibration has stopped.
        status_on = 3 ///< The vibration is on.
    };

    static constexpr std::uint32_t VIBRATION_CONTROL_REPO_UID = 0x10200C8B;
    static constexpr std::uint32_t VIBRATION_CONTROL_ENABLE_KEY = 0x01;
    static constexpr std::uint32_t VIBRATION_STATUS_KEY = 0x1001; ///< Property key of the vibration status.
}