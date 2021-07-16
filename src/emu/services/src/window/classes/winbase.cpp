/*
 * Copyright (c) 2019 EKA2L1 Team
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

#include <services/window/classes/winbase.h>
#include <services/window/op.h>
#include <services/window/opheader.h>
#include <services/window/screen.h>
#include <services/window/window.h>

#include <kernel/timing.h>
#include <kernel/kernel.h>
#include <utils/err.h>

namespace eka2l1::epoc {
    void window::queue_event(const epoc::event &evt) {
        client->queue_event(evt);
    }

    void window::set_parent(window *new_parent) {
        // New one will be the oldest. Quirky, but how WSERV functions.
        parent = new_parent;

        if (parent) {
            sibling = parent->child;
            parent->child = this;
        }
    }

    window *window::root_window() {
        return scr->root.get();
    }

    void window::walk_tree(window_tree_walker *walker, const window_tree_walk_style style) {
        window *end = root_window();
        window *cur = this;
        window *sibling = cur->sibling;
        window *parent = cur->parent;

        if (style != window_tree_walk_style::bonjour_previous_siblings) {
            if (style == window_tree_walk_style::bonjour_children) {
                end = this;
            }

            sibling = cur;

            while (sibling->child != nullptr) {
                sibling = sibling->child;
            }
        }

        do {
            if (sibling != nullptr) {
                // Traverse to our next sibling
                cur = sibling;

                // If this has a child, traverse to the oldest possible child.
                // We can start walking again from bottom. Don't question much, this code is like why does it work idk.
                // TODO (pent0): figure out the magic behind this. This is ripped from OSS code.
                while (cur->child != nullptr) {
                    cur = cur->child;
                }
            } else {
                // No more sibling present. Get to the parent.
                cur = parent;
            }

            parent = cur->parent;
            sibling = cur->sibling;

            if (walker->do_it(cur)) {
                return;
            }
        } while (cur != end);
    }

    window::~window() {
    }

    eka2l1::vec2 window::get_origin() {
        return { 0, 0 };
    }

    void window::set_position(const int new_pos) {
        if (check_order_change(new_pos)) {
            move_window(parent, new_pos);

            if (type == epoc::window_kind::group) {
                window_server &serv = client->get_ws();
                scr->update_focus(&serv, nullptr);
            }
        }
    }

    void window::move_window(epoc::window *new_parent, const int new_pos) {
        if (type == window_kind::group || type == window_kind::client || new_parent != parent) {
            // TODO: Check if any childs need a redraw before hassle.
            client->get_ws().get_anim_scheduler()->schedule(client->get_ws().get_graphics_driver(),
                scr, client->get_ws().get_ntimer()->microseconds());
        }

        remove_from_sibling_list();

        // The window that will be previous sibling of our future window.
        window **prev = &new_parent->child;

        // Iterates until finding a younger window
        while (*prev != nullptr && priority < (*prev)->priority) {
            prev = &((*prev)->sibling);
        }

        int pos = new_pos;

        // Iterates to find right window position.
        while (pos-- != 0 && *prev != nullptr && (*prev)->priority == priority) {
            prev = &((*prev)->sibling);
        }

        // Link to the list
        sibling = *prev;
        parent = new_parent;
        *prev = this;
    }

    bool window::check_order_change(const int new_pos) {
        // The more soon we reach the window in the linked list, the higher priority it's.
        window *cur = parent->child;
        window *prev = nullptr;

        // If the current window is the head: iterates to next child right away.
        // Iterates until we meet a window that has smaller or equal priority then us.
        while (cur == this || (cur != nullptr && priority < cur->priority)) {
            prev = cur;
            cur = cur->sibling;
        }

        if (prev == this) {
            cur = this;
        } else if (cur == nullptr || (cur->sibling == this && priority > cur->sibling->priority)) {
            // If there is no window that has smaller priority then us, we need to order this at the end
            // of the list. That means there is some order change
            //
            // Case 2: this window is the next sibling of the current. if the priority of the current window is larger
            // then it's in the wrong order and really can't do a position check (since priority not equal).
            return true;
        }

        // Traverse and find our current window, and see if we need to change order
        int pos = new_pos;

        while (pos-- != 0 && cur->sibling != nullptr && priority == cur->sibling->priority) {
            cur = cur->sibling;
        }

        return (cur != this);
    }

    void window::remove_from_sibling_list() {
        if (!parent) {
            return;
        }

        window *ite = parent->child;

        if (parent->child == this) {
            // Make the next sibling oldest
            parent->child = sibling;
            return;
        }

        while (true) {
            if (ite->sibling == this) {
                break;
            }

            ite = ite->sibling;
        }

        ite->sibling = sibling;
    }

    // This is the biggest sin i have commit, using DFS
    // 9:10 PM 5/9/2019 pent0
    void walk_tree_back_to_front(window *start, window_tree_walker *walker) {
        if (start == nullptr) {
            return;
        }

        // Gone through all siblings, walk on their childs
        // TODO: Is this correct? Older guy, children of older guy, then newer guy, children of newer guy.
        walk_tree_back_to_front(start->sibling, walker);
        if (walker->do_it(start)) {
            return;
        }

        walk_tree_back_to_front(start->child, walker);
    }

    void window::walk_tree_back_to_front(window_tree_walker *walker) {
        eka2l1::epoc::walk_tree_back_to_front(this, walker);
    }

    int window::ordinal_position(const bool full) {
        window *win = parent->child;

        if (full) {
            while (win->priority > priority) {
                win = win->sibling;
            }
        }

        int count = 0;

        for (count = 0; win != this; count++) {
            win = win->sibling;
        }

        return count;
    }

    void window::inquire_offset(service::ipc_context &ctx, ws_cmd &cmd) {
        // The data given is a 32 bit handle.
        // We are suppose to write back the offset distance between the given window and this.
        const std::uint32_t handle = *reinterpret_cast<std::uint32_t *>(cmd.data_ptr);
        epoc::window *win = reinterpret_cast<epoc::window *>(client->get_object(handle));

        if (!win) {
            ctx.complete(epoc::error_not_found);
            return;
        }

        eka2l1::vec2 offset_dist = get_origin() - win->get_origin();
        ctx.write_data_to_descriptor_argument(0, offset_dist);
        ctx.complete(epoc::error_none);
    }

    void window::set_fade(service::ipc_context &ctx, eka2l1::ws_cmd &cmd) {
        ws_cmd_set_fade *fade_param = reinterpret_cast<ws_cmd_set_fade *>(cmd.data_ptr);

        flags &= ~flags_faded;
        flags &= ~flags_faded_also_children;
        flags &= ~flags_faded_default_param;

        if (fade_param->flags & 1) {
            flags |= flags_faded;
        }

        if (fade_param->flags & 2) {
            flags |= flags_faded_also_children;
        }

        if (fade_param->flags & 4) {
            flags |= flags_faded_default_param;

            black_map = fade_param->black_map;
            white_map = fade_param->white_map;
        }

        ctx.complete(epoc::error_none);
    }

    void window::window_group_id(service::ipc_context &ctx, eka2l1::ws_cmd &cmd) {
        if (type == window_kind::group) {
            ctx.complete(id);
        } else {
            epoc::window *pp = parent;
            while (pp->type != window_kind::group) {
                pp = pp->parent;
            }

            ctx.complete(pp->id);
        }
    }

    void window::enable_visiblity_change_events(service::ipc_context &ctx, eka2l1::ws_cmd &cmd) {
        flags |= flag_visiblity_event_report;
        ctx.complete(epoc::error_none);
    }

    bool window::execute_command_for_general_node(eka2l1::service::ipc_context &ctx, eka2l1::ws_cmd &cmd) {
        epoc::version cli_ver = client->client_version();
        kernel_system *kern = client->get_ws().get_kernel_system();

        // Patching out user opcode.
        if ((cli_ver.major == WS_MAJOR_VER) && (cli_ver.minor == WS_MINOR_VER)) {
            if (cli_ver.build <= WS_OLDARCH_VER) {
                // Skip absolute position opcode
                if (cmd.header.op >= EWsWinOpAbsPosition) {
                    cmd.header.op += 1;
                }
            }

            if (cli_ver.build <= WS_NEWARCH_VER) {
                if ((cmd.header.op >= EWsWinOpSendAdvancedPointerEvent) && (kern->get_epoc_version() <= epocver::epoc94)) {
                    // Send advanced pointer event opcode does not exist in the version.
                    cmd.header.op += 1;
                }
            }
        }

        TWsWindowOpcodes op = static_cast<decltype(op)>(cmd.header.op);

        switch (op) {
        case EWsWinOpEnableModifierChangedEvents: {
            epoc::event_mod_notifier_user nof;
            nof.notifier = *reinterpret_cast<event_mod_notifier *>(cmd.data_ptr);
            nof.user = this;

            client->add_event_notifier<epoc::event_mod_notifier_user>(nof);
            ctx.complete(epoc::error_none);

            return true;
        }

        case EWsWinOpOrdinalPosition: {
            ctx.complete(ordinal_position(false));
            return true;
        }

        case EWsWinOpFullOrdinalPosition: {
            ctx.complete(ordinal_position(true));
            return true;
        }

        case EWsWinOpSetOrdinalPosition: {
            const int position = *reinterpret_cast<int *>(cmd.data_ptr);
            set_position(position);

            ctx.complete(epoc::error_none);

            return true;
        }

        case EWsWinOpSetOrdinalPositionPri: {
            ws_cmd_ordinal_pos_pri *info = reinterpret_cast<decltype(info)>(cmd.data_ptr);
            priority = info->pri1;
            const int position = info->pri2;

            set_position(position);
            ctx.complete(epoc::error_none);

            return true;
        }

        case EWsWinOpIdentifier: {
            ctx.complete(static_cast<int>(id));
            return true;
        }

        case EWsWinOpEnableErrorMessages: {
            epoc::event_control ctrl = *reinterpret_cast<epoc::event_control *>(cmd.data_ptr);
            epoc::event_error_msg_user nof;
            nof.when = ctrl;
            nof.user = this;

            client->add_event_notifier<epoc::event_error_msg_user>(nof);
            ctx.complete(epoc::error_none);

            return true;
        }

        case EWsWinOpInquireOffset: {
            inquire_offset(ctx, cmd);
            return true;
        }

        case EWsWinOpSetFade: {
            set_fade(ctx, cmd);
            return true;
        }
 
        case EWsWinOpWindowGroupId:
            window_group_id(ctx, cmd);
            return true;

        case EWsWinOpEnableVisibilityChangeEvents:
            enable_visiblity_change_events(ctx, cmd);
            return true;    

        default: {
            break;
        }
        }

        return false;
    }
}
