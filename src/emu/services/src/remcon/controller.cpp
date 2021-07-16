/*
 * Copyright (c) 2020 EKA2L1 Team.
 * 
 * This file is part of EKA2L1 project
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

#include <services/context.h>
#include <services/remcon/controller.h>
#include <utils/err.h>

#include <common/log.h>

namespace eka2l1::epoc::remcon {
    controller_session::~controller_session() {
    }

    void controller_session::register_interested_apis(service::ipc_context *ctx) {
        LOG_TRACE(SERVICE_REMCON, "Register interested APIs for controller session stubbed");
        ctx->complete(epoc::error_none);
    }
}