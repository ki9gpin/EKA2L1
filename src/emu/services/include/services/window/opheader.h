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

#pragma once

#include <mem/ptr.h>
#include <services/window/common.h>
#include <utils/des.h>

#include <common/uid.h>
#include <common/vecx.h>

namespace eka2l1 {
    struct ws_cmd_header {
        uint16_t op;
        uint16_t cmd_len;
    };

    struct ws_cmd {
        ws_cmd_header header;
        uint32_t obj_handle;

        void *data_ptr;
    };

    struct ws_cmd_screen_device_header {
        int num_screen;
        uint32_t screen_dvc_ptr;
    };

    struct ws_cmd_window_header {
        int parent;
        uint32_t client_handle;

        epoc::window_type win_type;
        epoc::display_mode dmode;
    };

    struct ws_cmd_window_group_header {
        uint32_t client_handle;
        bool focus;
        uint32_t parent_id;
        uint32_t screen_device_handle;
    };

    struct ws_cmd_create_sprite_header {
        int window_handle;
        eka2l1::vec2 base_pos;
        int flags;
    };

    struct ws_cmd_ordinal_pos_pri {
        int pri2;
        int pri1;
    };

    struct ws_cmd_find_window_group_identifier {
        std::uint32_t previous_id;
        int offset;
        int length;
    };

    struct ws_cmd_find_window_group_identifier_thread {
        std::uint32_t previous_id;
        std::uint64_t thread_id;
    };

    struct ws_cmd_find_window_group_identifier_thread_eka1 {
        std::uint32_t previous_id;
        std::uint32_t thread_id;
    };

    struct ws_cmd_set_extent {
        eka2l1::vec2 pos;
        eka2l1::vec2 size;
    };

    struct ws_cmd_pointer_filter {
        std::uint32_t mask;
        std::uint32_t flags;
    };

    struct ws_cmd_draw_text {
        eka2l1::vec2 pos;
        std::int32_t length;
    };

    struct ws_cmd_draw_text_ptr {
        eka2l1::vec2 pos;
        eka2l1::ptr<epoc::desc16> text;
    };

    struct ws_cmd_draw_box_text_ptr {
        vec2 left_top_pos;
        vec2 right_bottom_pos;
        std::int32_t baseline_offset;
        epoc::text_alignment horiz;
        std::int32_t left_mgr;
        std::int32_t width;
        eka2l1::ptr<epoc::desc16> text;
    };

    struct ws_cmd_draw_box_text_optimised1 {
        vec2 left_top_pos;
        vec2 right_bottom_pos;
        std::int32_t baseline_offset;
        std::int32_t length;
    };

    struct ws_cmd_draw_box_text_optimised2 {
        vec2 left_top_pos;
        vec2 right_bottom_pos;
        std::int32_t baseline_offset;
        epoc::text_alignment horiz;
        std::int32_t left_mgr;
        std::int32_t length;
    };

    struct ws_cmd_draw_line {
        vec2 left_top_pos;
        vec2 right_bottom_pos;
    };

    struct ws_cmd_set_text_cursor {
        uint32_t win;
        vec2 pos;

        // TODO: Add more
    };

    struct ws_cmd_send_event_to_window_group {
        int id;
        epoc::event evt;
    };

    struct ws_cmd_send_message_to_window_group {
        std::int32_t id_or_priority;
        epoc::uid uid;
        std::int32_t data_length;
        eka2l1::ptr<epoc::desc8> data;
    };

    struct ws_cmd_draw_text_vertical_v94 {
        vec2 pos;
        vec2 bottom_right;
        int unk1;
        int length;
    };

    struct ws_cmd_get_window_group_name_from_id {
        std::uint32_t id;
        int max_len;
    };

    struct ws_cmd_capture_key {
        std::uint32_t modifiers;
        std::uint32_t modifier_mask;
        std::uint32_t key;
        int priority;
    };

    struct ws_cmd_draw_bitmap {
        std::uint32_t handle;
        eka2l1::vec2 pos;
    };

    struct ws_cmd_draw_bitmap2 {
        std::uint32_t handle;
        eka2l1::rect dest_rect;
    };

    struct ws_cmd_draw_bitmap3: public ws_cmd_draw_bitmap2 {
        eka2l1::rect source_rect;
    };

    struct ws_cmd_window_group_list {
        std::int32_t priority;
        std::int32_t count;
        std::int32_t screen_num;
    };

    struct ws_cmd_gdi_blt2 {
        std::uint32_t handle;
        eka2l1::vec2 pos;
    };

    struct ws_cmd_gdi_blt3 : public ws_cmd_gdi_blt2 {
        eka2l1::rect source_rect;
    };

    struct ws_cmd_draw_ws_bitmap_masked {
        std::uint32_t source_handle;
        std::uint32_t mask_handle;
        eka2l1::rect dest_rect;
        eka2l1::rect source_rect;
        std::uint32_t invert_mask;
    };

    struct ws_cmd_gdi_blt_masked {
        eka2l1::vec2 pos;
        std::uint32_t source_handle;
        eka2l1::rect source_rect;
        std::uint32_t mask_handle;
        std::uint32_t invert_mask;
    };

    struct ws_cmd_set_window_group_ordinal_position {
        std::uint32_t identifier;
        std::int32_t ord_pos;
    };

    struct ws_cmd_set_fade {
        std::uint8_t black_map;
        std::uint8_t white_map;
        std::uint8_t flags;
        std::uint8_t imheretoalignthestruct; ///< Yes.
    };

    struct ws_cmd_set_pointer_cursor_area {
        std::int32_t mode;
        eka2l1::rect pointer_area;
    };

    struct ws_cmd_alloc_pointer_buffer {
        std::uint32_t max_points;
        std::uint32_t flags;
    };

    struct ws_cmd_keyboard_repeat_rate {
        std::uint32_t initial_time;
        std::uint32_t next_time;
    };
}
