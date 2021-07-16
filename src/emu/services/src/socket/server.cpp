/*
 * Copyright (c) 2020 EKA2L1 Team
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

#include <common/cvt.h>
#include <services/internet/protocols/overall.h>
#include <services/socket/agent/genconn.h>
#include <services/socket/connection.h>
#include <services/socket/host.h>
#include <services/socket/socket.h>
#include <services/socket/server.h>
#include <system/epoc.h>

#include <utils/err.h>

namespace eka2l1 {
    std::string get_socket_server_name_by_epocver(const epocver ver) {
        if (ver <= epocver::eka2) {
            return "SocketServer";
        }

        return "!SocketServer";
    }

    socket_server::socket_server(eka2l1::system *sys)
        : service::typical_server(sys, get_socket_server_name_by_epocver((sys->get_symbian_version_use()))) {
        agents_.push_back(std::make_unique<epoc::socket::generic_connect_agent>(this));
        epoc::internet::add_internet_stack_protocols(this, kern->is_eka1());
    }

    void socket_server::connect(service::ipc_context &context) {
        create_session<socket_client_session>(&context);
        context.complete(epoc::error_none);
    }

    inline std::uint64_t make_protocol_key_map(const std::uint32_t addr_family, const std::uint32_t pr_id) {
        return (static_cast<std::uint64_t>(addr_family) << 32) | pr_id;
    }

    epoc::socket::protocol *socket_server::find_protocol(const std::uint32_t addr_family, const std::uint32_t protocol_id) {
        auto fres = protocols_.find(make_protocol_key_map(addr_family, protocol_id));
        if (fres != protocols_.end()) {
            return fres->second.get();
        }

        return nullptr;
    }

    epoc::socket::protocol *socket_server::find_protocol_by_name(const std::u16string &name) {
        for (auto &pr: protocols_) {
            if (pr.second->name() == name) {
                return pr.second.get();
            }
        }

        return nullptr;
    }

    bool socket_server::add_protocol(std::unique_ptr<epoc::socket::protocol> &pr) {
        // Once added, the address family and protocol ID can't be changed. So I think it's fine to store in a map...
        std::uint64_t map_key = make_protocol_key_map(pr->family_id(), pr->id());
        if (protocols_.find(map_key) != protocols_.end()) {
            LOG_ERROR(SERVICE_ESOCK, "Trying to add an protocol with same family address and same protocol number!");
            return false;
        }

        protocols_.emplace(map_key, std::move(pr));
        return true;
    }

    epoc::socket::connect_agent *socket_server::get_connect_agent(const std::u16string &name) {
        auto agt_ite = std::find_if(agents_.begin(), agents_.end(), [name](std::unique_ptr<epoc::socket::connect_agent> &agt) {
            return agt->agent_name() == name;
        });

        if (agt_ite != agents_.end()) {
            return agt_ite->get();
        }

        return nullptr;
    }

    bool socket_server::add_agent(std::unique_ptr<epoc::socket::connect_agent> &ag) {
        for (const auto &agt: agents_) {
            if (agt->agent_name() == ag->agent_name()) {
                return false;
            }
        }

        agents_.push_back(std::move(ag));
        return true;
    }

    socket_client_session::socket_client_session(service::typical_server *serv, const kernel::uid ss_id,
        epoc::version client_version)
        : service::typical_session(serv, ss_id, client_version) {
    }

    bool socket_client_session::is_oldarch() {
        return server<socket_server>()->get_kernel_object_owner()->get_epoc_version() < epocver::epoc81a;
    }

    void socket_client_session::fetch(service::ipc_context *ctx) {
        if (is_oldarch()) {
            switch (ctx->msg->function) {
            case socket_old_pr_find:
                pr_find(ctx);
                return;

            case socket_old_so_create:
                so_create(ctx);
                return;

            case socket_old_hr_open:
                hr_create(ctx, false);
                return;

            default:
                break;
            }
        } else {
            switch (ctx->msg->function) {
            case socket_pr_find:
                pr_find(ctx);
                return;

            case socket_so_create:
                so_create(ctx);
                return;

            case socket_hr_open:
                hr_create(ctx, false);
                return;

            case socket_hr_open_with_connection:
                hr_create(ctx, true);
                return;

            case socket_sr_get_by_number:
                sr_get_by_number(ctx);
                return;

            case socket_cn_get_long_des_setting:
                cn_get_long_des_setting(ctx);
                return;

            default:
                break;
            }
        }

        std::optional<std::uint32_t> subsess_id = ctx->get_argument_value<std::uint32_t>(3);

        if (subsess_id && (subsess_id.value() > 0)) {
            socket_subsession_instance *inst = subsessions_.get(subsess_id.value());

            if (inst) {
                inst->get()->dispatch(ctx);
                return;
            }
        }
    
        LOG_ERROR(SERVICE_ESOCK, "Unimplemented opcode for Socket server 0x{:X}", ctx->msg->function);
    }

    void socket_client_session::sr_get_by_number(eka2l1::service::ipc_context *ctx) {
        std::int32_t port = *(ctx->get_argument_value<std::int32_t>(1));

        std::string name = "DummyService";
        ctx->write_data_to_descriptor_argument(0, reinterpret_cast<std::uint8_t *>(&name[0]),
            static_cast<std::uint32_t>(name.length()));
        ctx->complete(epoc::error_none);
    }
    
    void socket_client_session::cn_get_long_des_setting(eka2l1::service::ipc_context *ctx) {
        LOG_TRACE(SERVICE_ESOCK, "CnGetLongDesSetting stubbed");
        ctx->complete(epoc::error_none);
    }

    static void fill_protocol_description(epoc::socket::protocol *pr, protocol_description &des) {
        des.addr_fam_ = pr->family_id();
        des.protocol_ = pr->id();
        des.ver_ = pr->ver();
        des.bord_ = pr->get_byte_order();
        des.sock_type_ = pr->sock_type();
        des.message_size_ = pr->message_size();

        des.name_.assign(nullptr, pr->name());

        // TODO: From this
        des.service_info_ = 0;
        des.naming_services_ = 0;
        des.service_sec_ = 0;
    }

    void socket_client_session::pr_find(service::ipc_context *ctx) {
        std::optional<std::u16string> protocol_name = ctx->get_argument_value<std::u16string>(1);
        if (!protocol_name.has_value()) {
            ctx->complete(epoc::error_argument);
            return;
        }

        epoc::socket::protocol *result_pr = server<socket_server>()->find_protocol_by_name(protocol_name.value());
        if (!result_pr) {
            LOG_WARN(SERVICE_ESOCK, "Can't find protocol named {}", common::ucs2_to_utf8(protocol_name.value()));
            ctx->complete(epoc::error_not_found);

            return;
        }

        protocol_description description_to_return;
        fill_protocol_description(result_pr, description_to_return);

        ctx->write_data_to_descriptor_argument<protocol_description>(0, description_to_return);
        ctx->complete(epoc::error_none);
    }

    void socket_client_session::hr_create(service::ipc_context *ctx, const bool with_conn) {
        std::optional<std::uint32_t> addr_family = ctx->get_argument_value<std::uint32_t>(0);
        std::optional<std::uint32_t> protocol = ctx->get_argument_value<std::uint32_t>(1);
        std::optional<std::uint32_t> conn_subhandle = ctx->get_argument_value<std::uint32_t>(2);

        if (!addr_family || !protocol || (with_conn && !conn_subhandle)) {
            ctx->complete(epoc::error_argument);
            return;
        }

        // Find the protocol that satifies our condition first
        epoc::socket::protocol *target_pr = server<socket_server>()->find_protocol(addr_family.value(), protocol.value());
        if (!target_pr) {
            LOG_ERROR(SERVICE_ESOCK, "Unable to find host resolver with address={}, protocol={}", addr_family.value(), protocol.value());
            ctx->complete(epoc::error_not_found);

            return;
        }

        epoc::socket::socket_connection_proxy *conn = nullptr;

        if (with_conn) {
            socket_subsession_instance *inst = subsessions_.get(conn_subhandle.value());

            if (!inst) {
                ctx->complete(epoc::error_argument);
                return;
            }

            conn = reinterpret_cast<epoc::socket::socket_connection_proxy*>(inst->get());
        }

        // Try to instantiate the host resolver
        std::unique_ptr<epoc::socket::host_resolver> resolver_impl = target_pr->make_host_resolver();
        if (!resolver_impl) {
            LOG_ERROR(SERVICE_ESOCK, "The protocol {} does not support resolving host!", common::ucs2_to_utf8(target_pr->name()));
            ctx->complete(epoc::error_not_supported);

            return;
        }

        // Create new session
        socket_subsession_instance hr_inst = std::make_unique<epoc::socket::socket_host_resolver>(
            this, resolver_impl, conn ? conn->get_connection() : nullptr);

        const std::uint32_t id = static_cast<std::uint32_t>(subsessions_.add(hr_inst));
        subsessions_.get(id)->get()->set_id(id);

        // Write the subsession handle
        ctx->write_data_to_descriptor_argument<std::uint32_t>(3, id);
        ctx->complete(epoc::error_none);
    }

    void socket_client_session::so_create(service::ipc_context *ctx) {
        std::optional<std::uint32_t> addr_family = ctx->get_argument_value<std::uint32_t>(0);
        std::optional<std::uint32_t> sock_type = ctx->get_argument_value<std::uint32_t>(1);
        std::optional<std::uint32_t> protocol = ctx->get_argument_value<std::uint32_t>(2);

        if (!addr_family || !protocol || !sock_type) {
            ctx->complete(epoc::error_argument);
            return;
        }

        // Find the protocol that satifies our condition first
        epoc::socket::protocol *target_pr = server<socket_server>()->find_protocol(addr_family.value(), protocol.value());
        if (!target_pr) {
            LOG_ERROR(SERVICE_ESOCK, "Unable to find host resolver with address={}, protocol={}", addr_family.value(), protocol.value());
            ctx->complete(epoc::error_not_found);

            return;
        }

        if (sock_type.value() != target_pr->sock_type()) {
            LOG_WARN(SERVICE_ESOCK, "Incompatible socket type {}, proceed to create socket anyway", sock_type.value());
        }

        std::unique_ptr<epoc::socket::socket> sock_impl = target_pr->make_socket();
        if (!sock_impl) {
            LOG_ERROR(SERVICE_ESOCK, "The protocol {} does not support opening socket!", common::ucs2_to_utf8(target_pr->name()));
            ctx->complete(epoc::error_not_supported);

            return;
        }
        
        // Create new session
        socket_subsession_instance so_inst = std::make_unique<epoc::socket::socket_socket>(this, sock_impl);

        const std::uint32_t id = static_cast<std::uint32_t>(subsessions_.add(so_inst));
        subsessions_.get(id)->get()->set_id(id);

        // Write the subsession handle
        ctx->write_data_to_descriptor_argument<std::uint32_t>(3, id);
        ctx->complete(epoc::error_none);
    }
}
