/*
 * Copyright (c) 2021 EKA2L1 Team
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

#include <services/internet/protocols/udp/udp.h>
#include <common/log.h>

namespace eka2l1::epoc::internet {
    udp_host_resolver::udp_host_resolver(udp_protocol *papa)
        : papa_(papa) {
    }

    std::u16string udp_host_resolver::host_name() const {
        // I don't think this has much meaning
        return u"";
    }

    bool udp_host_resolver::host_name(const std::u16string &name) {
        return true;
    }
    
    bool udp_host_resolver::get_by_address(epoc::socket::saddress &addr, epoc::socket::name_entry &result) {
        LOG_WARN(SERVICE_BLUETOOTH, "Get host by address stubbed to not found");
        return false;
    }

    bool udp_host_resolver::get_by_name(epoc::socket::name_entry &supply_and_result) {
        LOG_WARN(SERVICE_BLUETOOTH, "Get host by name stubbed to not found");
        return false;
    }
}