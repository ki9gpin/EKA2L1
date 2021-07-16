/*
 * Copyright (c) 2020 EKA2L1 Team.
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

#include <common/cvt.h>
#include <system/epoc.h>
#include <kernel/kernel.h>
#include <kernel/property.h>
#include <services/etel/common.h>
#include <services/etel/etel.h>
#include <services/etel/phone.h>
#include <services/sysagt/sysagt.h>
#include <utils/err.h>

namespace eka2l1 {
    std::string get_etel_server_name_by_epocver(const epocver ver) {
        if (ver <= epocver::eka2) {
            return "EtelServer";
        }

        return "!EtelServer";
    }

    etel_server::etel_server(eka2l1::system *sys)
        : service::typical_server(sys, get_etel_server_name_by_epocver(sys->get_symbian_version_use()))
        , call_status_prop_(nullptr)
        , network_bars_prop_(nullptr)
        , battery_bars_prop_(nullptr)
        , charger_status_prop_(nullptr)
        , init2ed_(false) {
        init(sys->get_kernel_system());
    }

    void etel_server::init(kernel_system *kern) {
        if (call_status_prop_ || network_bars_prop_ || battery_bars_prop_ || charger_status_prop_) {
            return;
        }

        // Make call status property.
        call_status_prop_ = kern->create<service::property>();
        call_status_prop_->define(service::property_type::int_data, 4);

        call_status_prop_->first = eka2l1::SYSTEM_AGENT_PROPERTY_CATEGORY;
        call_status_prop_->second = epoc::ETEL_PHONE_CURRENT_CALL_UID;

        call_status_prop_->set_int(epoc::etel_phone_current_call_none);

        // Make network bars property
        network_bars_prop_ = kern->create<service::property>();
        network_bars_prop_->define(service::property_type::int_data, 4);

        network_bars_prop_->first = eka2l1::SYSTEM_AGENT_PROPERTY_CATEGORY;
        network_bars_prop_->second = epoc::ETEL_PHONE_NETWORK_BARS_UID;

        network_bars_prop_->set_int(epoc::ETEL_MAX_BAR_LEVEL * epoc::ETEL_BAR_MULTIPLIER);

        // Make battery bars property.
        battery_bars_prop_ = kern->create<service::property>();
        battery_bars_prop_->define(service::property_type::int_data, 4);

        battery_bars_prop_->first = eka2l1::SYSTEM_AGENT_PROPERTY_CATEGORY;
        battery_bars_prop_->second = epoc::ETEL_PHONE_BATTERY_BARS_UID;

        battery_bars_prop_->set_int(epoc::ETEL_MAX_BAR_LEVEL * epoc::ETEL_BAR_MULTIPLIER);

        // Make charger status property
        charger_status_prop_ = kern->create<service::property>();
        charger_status_prop_->define(service::property_type::int_data, 4);

        charger_status_prop_->first = eka2l1::SYSTEM_AGENT_PROPERTY_CATEGORY;
        charger_status_prop_->second = epoc::ETEL_PHONE_CHARGER_STATUS_UID;

        charger_status_prop_->set_int(epoc::etel_charger_status_connected);
    }

    void etel_server::init2(kernel_system *kern, io_system *io) {
        // Load neccessary modules! This is usually handle by another party...
        static constexpr const char *REGULAR_TSY_NAME = "phonetsy";

        mngr_.load_tsy(kern, io, 0, REGULAR_TSY_NAME);
        init2ed_ = true;
    }

    void etel_server::connect(service::ipc_context &ctx) {
        if (!init2ed_) {
            init2(ctx.sys->get_kernel_system(), ctx.sys->get_io_system());
        }

        create_session<etel_session>(&ctx);
        ctx.complete(epoc::error_none);
    }

    etel_legacy_level etel_server::legacy_level() {
        if (kern->get_epoc_version() <= epocver::epoc6) {
            return ETEL_LEGACY_LEVEL_LEGACY;
        }

        if (kern->get_epoc_version() <= epocver::epoc81a) {
            return ETEL_LEGACY_LEVEL_TRANSITION;
        }

        return ETEL_LEGACY_LEVEL_MORDEN;
    }

    etel_session::etel_session(service::typical_server *serv, kernel::uid client_ss_uid, epoc::version client_ver)
        : service::typical_session(serv, client_ss_uid, client_ver) {
    }

    etel_session::~etel_session() {
        etel_server *serv = server<etel_server>();
        io_system *io = serv->get_system()->get_io_system();

        serv->mngr_.unload_from_sessions(io, client_ss_uid_);
    }

    void etel_session::close_phone_module(service::ipc_context *ctx) {
        std::optional<std::u16string> name = ctx->get_argument_value<std::u16string>(0);

        if (!name) {
            ctx->complete(epoc::error_argument);
            return;
        }

        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;

        if (!mngr.close_tsy(ctx->sys->get_io_system(), client_ss_uid_, common::ucs2_to_utf8(name.value()))) {
            ctx->complete(epoc::error_not_found);
            return;
        }

        ctx->complete(epoc::error_none);
    }

    void etel_session::load_phone_module(service::ipc_context *ctx) {
        std::optional<std::u16string> name = ctx->get_argument_value<std::u16string>(0);

        if (!name) {
            ctx->complete(epoc::error_argument);
            return;
        }

        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;

        if (!mngr.load_tsy(ctx->sys->get_kernel_system(), ctx->sys->get_io_system(), client_ss_uid_, common::ucs2_to_utf8(name.value()))) {
            ctx->complete(epoc::error_already_exists);
            return;
        }

        ctx->complete(epoc::error_none);
    }

    void etel_session::enumerate_phones(service::ipc_context *ctx) {
        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;
        std::uint32_t total_phone = static_cast<std::uint32_t>(mngr.total_entries(epoc::etel_entry_phone));

        ctx->write_data_to_descriptor_argument(0, total_phone);
        ctx->complete(epoc::error_none);
    }

    void etel_session::get_phone_info_by_index(service::ipc_context *ctx) {
        epoc::etel_phone_info info;
        const std::int32_t index = *ctx->get_argument_value<std::int32_t>(1);
        
        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;
        std::optional<std::uint32_t> real_index = mngr.get_entry_real_index(index, epoc::etel_entry_phone);

        if (!real_index.has_value()) {
            ctx->complete(epoc::error_argument);
            return;
        }

        epoc::etel_module_entry *entry = nullptr;
        mngr.get_entry(real_index.value(), &entry);

        etel_phone &phone = static_cast<etel_phone &>(*entry->entity_);

        ctx->write_data_to_descriptor_argument<epoc::etel_phone_info>(0, phone.info_);
        ctx->complete(epoc::error_none);
    }

    void etel_session::get_tsy_name(service::ipc_context *ctx) {
        const std::int32_t index = *ctx->get_argument_value<std::int32_t>(0);

        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;
        std::optional<std::uint32_t> real_index = mngr.get_entry_real_index(index,
            epoc::etel_entry_phone);

        if (!real_index.has_value()) {
            ctx->complete(epoc::error_argument);
            return;
        }

        epoc::etel_module_entry *entry = nullptr;
        mngr.get_entry(real_index.value(), &entry);

        ctx->write_arg(1, common::utf8_to_ucs2(entry->tsy_name_));
        ctx->complete(epoc::error_none);
    }

    void etel_session::add_new_subsession(service::ipc_context *ctx, etel_subsession_instance &instance) {
        auto empty_slot = std::find(subsessions_.begin(), subsessions_.end(), nullptr);

        if (empty_slot == subsessions_.end()) {
            subsessions_.push_back(std::move(instance));
            ctx->write_data_to_descriptor_argument<std::uint32_t>(3, static_cast<std::uint32_t>(subsessions_.size()));
        } else {
            *empty_slot = std::move(instance);
            ctx->write_data_to_descriptor_argument<std::uint32_t>(3, static_cast<std::uint32_t>(std::distance(subsessions_.begin(), empty_slot) + 1));
        }
    }

    void etel_session::open_from_session(service::ipc_context *ctx) {
        std::optional<std::u16string> name_of_object = ctx->get_argument_value<std::u16string>(0);

        if (!name_of_object) {
            ctx->complete(epoc::error_argument);
            return;
        }

        LOG_TRACE(SERVICE_ETEL, "Opening {} from session", common::ucs2_to_utf8(name_of_object.value()));

        epoc::etel_module_entry *entry = nullptr;

        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;
        if (!mngr.get_entry_by_name(common::ucs2_to_utf8(name_of_object.value()), &entry)) {
            ctx->complete(epoc::error_not_found);
            return;
        }

        // Check for empty slot in the subessions array
        std::unique_ptr<etel_subsession> subsession;

        switch (entry->entity_->type()) {
        case epoc::etel_entry_phone:
            subsession = std::make_unique<etel_phone_subsession>(this, reinterpret_cast<etel_phone *>(entry->entity_.get()),
                server<etel_server>()->legacy_level());

            break;

        default:
            LOG_ERROR(SERVICE_ETEL, "Unsupported entry type of etel module {}", static_cast<int>(entry->entity_->type()));
            ctx->complete(epoc::error_general);
            return;
        }

        add_new_subsession(ctx, subsession);
        ctx->complete(epoc::error_none);
    }

    void etel_session::open_from_subsession(service::ipc_context *ctx) {
        std::optional<std::u16string> name_of_object = ctx->get_argument_value<std::u16string>(0);

        if (!name_of_object) {
            ctx->complete(epoc::error_argument);
            return;
        }

        LOG_TRACE(SERVICE_ETEL, "Opening {} from session", common::ucs2_to_utf8(name_of_object.value()));

        // Try to get the subsession
        std::optional<std::uint32_t> subsession_handle = ctx->get_argument_value<std::uint32_t>(2);

        if (!subsession_handle || subsession_handle.value() > subsessions_.size()) {
            LOG_ERROR(SERVICE_ETEL, "Subsession handle not available");
            ctx->complete(epoc::error_argument);
            return;
        }

        etel_subsession_instance &sub = subsessions_[subsession_handle.value() - 1];
        etel_subsession_instance new_sub = nullptr;

        if (!sub) {
            ctx->complete(epoc::error_not_found);
            return;
        }

        if (sub->type() == etel_subsession_type_phone) {
            etel_phone *phone = reinterpret_cast<etel_phone_subsession *>(sub.get())->phone_;

            // First, try to find the line
            auto line_ite = std::find_if(phone->lines_.begin(), phone->lines_.end(), [=](const etel_line *line) {
                return common::compare_ignore_case(common::utf8_to_ucs2(line->name_), name_of_object.value()) == 0;
            });

            if (line_ite != phone->lines_.end()) {
                // Create the subsession
                new_sub = std::make_unique<etel_line_subsession>(this, *line_ite, server<etel_server>()->legacy_level());
            } else {
                LOG_ERROR(SERVICE_ETEL, "Unable to open subsession with object name {}", common::ucs2_to_utf8(name_of_object.value()));
            }
        } else {
            LOG_ERROR(SERVICE_ETEL, "Unhandled subsession type to open from {}", static_cast<int>(sub->type()));
        }

        if (!new_sub) {
            ctx->complete(epoc::error_not_found);
            return;
        }

        add_new_subsession(ctx, new_sub);
        ctx->complete(epoc::error_none);
    }

    void etel_session::close_sub(service::ipc_context *ctx) {
        std::optional<std::uint32_t> subhandle = ctx->get_argument_value<std::uint32_t>(3);

        if (subhandle && subhandle.value() <= subsessions_.size()) {
            subsessions_[subhandle.value() - 1].reset();
        }

        ctx->complete(epoc::error_none);
    }

    void etel_session::query_tsy_functionality(service::ipc_context *ctx) {
        LOG_TRACE(SERVICE_ETEL, "Query TSY functionality stubbed");
        ctx->complete(epoc::error_none);
    }

    void etel_session::line_enumerate_call(service::ipc_context *ctx) {
        epoc::etel::module_manager &mngr = server<etel_server>()->mngr_;
        std::uint32_t total_call = static_cast<std::uint32_t>(mngr.total_entries(epoc::etel_entry_call));

        ctx->write_data_to_descriptor_argument(0, total_call);
        ctx->complete(epoc::error_none);
    }

    void etel_session::fetch(service::ipc_context *ctx) {
        if (server<etel_server>()->legacy_level() <= ETEL_LEGACY_LEVEL_TRANSITION) {
            switch (ctx->msg->function) {
            case epoc::etel_old_open_from_session:
                open_from_session(ctx);
                break;

            case epoc::etel_old_open_from_subsession:
                open_from_subsession(ctx);
                break;

            case epoc::etel_old_close:
                close_sub(ctx);
                break;

            case epoc::etel_old_load_phone_module:
                load_phone_module(ctx);
                break;

            case epoc::etel_old_unload_phone_module:
                close_phone_module(ctx);
                break;

            case epoc::etel_old_enumerate_phones:
                enumerate_phones(ctx);
                break;

            case epoc::etel_old_get_phone_info:
                get_phone_info_by_index(ctx);
                break;

            case epoc::etel_old_get_tsy_name:
                get_tsy_name(ctx);
                break;

            case epoc::etel_old_line_enumerate_call:
                line_enumerate_call(ctx);
                break;

            default:
                std::optional<std::uint32_t> subsess_id = ctx->get_argument_value<std::uint32_t>(3);

                if (subsess_id && (subsess_id.value() > 0)) {
                    if (subsess_id.value() <= subsessions_.size()) {
                        subsessions_[subsess_id.value() - 1]->dispatch(ctx);
                        return;
                    }
                }

                LOG_ERROR(SERVICE_ETEL, "Unimplemented ETel server opcode {}", ctx->msg->function);
                break;
            }
        } else {
            switch (ctx->msg->function) {
            case epoc::etel_open_from_session:
                open_from_session(ctx);
                break;

            case epoc::etel_open_from_subsession:
                open_from_subsession(ctx);
                break;

            case epoc::etel_close:
                close_sub(ctx);
                break;

            case epoc::etel_close_phone_module:
                close_phone_module(ctx);
                break;

            case epoc::etel_enumerate_phones:
                enumerate_phones(ctx);
                break;

            case epoc::etel_get_tsy_name:
                get_tsy_name(ctx);
                break;

            case epoc::etel_phone_info_by_index:
                get_phone_info_by_index(ctx);
                break;

            case epoc::etel_load_phone_module:
                load_phone_module(ctx);
                break;

            case epoc::etel_query_tsy_functionality:
                query_tsy_functionality(ctx);
                break;

            case epoc::etel_line_enumerate_call:
                line_enumerate_call(ctx);
                break;

            default:
                std::optional<std::uint32_t> subsess_id = ctx->get_argument_value<std::uint32_t>(3);

                if (subsess_id && (subsess_id.value() > 0)) {
                    if (subsess_id.value() <= subsessions_.size()) {
                        subsessions_[subsess_id.value() - 1]->dispatch(ctx);
                        return;
                    }
                }

                LOG_ERROR(SERVICE_ETEL, "Unimplemented ETel server opcode {}", ctx->msg->function);
                break;
            }
        }
    }
}