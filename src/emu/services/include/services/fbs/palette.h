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

#include <array>

#include <common/rgb.h>
#include <common/types.h>

namespace eka2l1::epoc {
    using palette_256 = std::array<common::rgb, 256>;

    static palette_256 color_256_palette_old = {
        0, 0xCCFFFF, 0x99FFFF, 0x66FFFF, 0x33FFFF, 0xFFFF, 0xFFCCFF,
        0xCCCCFF, 0x99CCFF, 0x66CCFF, 0x33CCFF, 0xCCFF, 0xFF99FF,
        0xCC99FF, 0x9999FF, 0x6699FF, 0x3399FF, 0x99FF, 0xFF66FF,
        0xCC66FF, 0x9966FF, 0x6666FF, 0x3366FF, 0x66FF, 0xFF33FF,
        0xCC33FF, 0x9933FF, 0x6633FF, 0x3333FF, 0x33FF, 0xFF00FF,
        0xCC00FF, 0x9900FF, 0x6600FF, 0x3300FF, 0xFF, 0xFFFFCC,
        0xCCFFCC, 0x99FFCC, 0x66FFCC, 0x33FFCC, 0xFFCC, 0xFFCCCC,
        0xCCCCCC, 0x99CCCC, 0x66CCCC, 0x33CCCC, 0xCCCC, 0xFF99CC,
        0xCC99CC, 0x9999CC, 0x6699CC, 0x3399CC, 0x99CC, 0xFF66CC,
        0xCC66CC, 0x9966CC, 0x6666CC, 0x3366CC, 0x66CC, 0xFF33CC,
        0xCC33CC, 0x9933CC, 0x6633CC, 0x3333CC, 0x33CC, 0xFF00CC,
        0xCC00CC, 0x9900CC, 0x6600CC, 0x3300CC, 0xCC, 0xFFFF99,
        0xCCFF99, 0x99FF99, 0x66FF99, 0x33FF99, 0xFF99, 0xFFCC99,
        0xCCCC99, 0x99CC99, 0x66CC99, 0x33CC99, 0xCC99, 0xFF9999,
        0xCC9999, 0x999999, 0x669999, 0x339999, 0x9999, 0xFF6699,
        0xCC6699, 0x996699, 0x666699, 0x336699, 0x6699, 0xFF3399,
        0xCC3399, 0x993399, 0x663399, 0x333399, 0x3399, 0xFF0099,
        0xCC0099, 0x990099, 0x660099, 0x330099, 0x99, 0xFFFF66,
        0xCCFF66, 0x99FF66, 0x66FF66, 0x33FF66, 0xFF66, 0xFFCC66,
        0xCCCC66, 0x99CC66, 0x66CC66, 0x33CC66, 0xCC66, 0xFF9966,
        0xCC9966, 0x999966, 0x669966, 0x339966, 0x9966, 0xFF6666,
        0xCC6666, 0x996666, 0x666666, 0x336666, 0x6666, 0xFF3366,
        0xCC3366, 0x993366, 0x663366, 0x333366, 0x3366, 0xFF0066,
        0xCC0066, 0x990066, 0x660066, 0x330066, 0x66, 0xFFFF33,
        0xCCFF33, 0x99FF33, 0x66FF33, 0x33FF33, 0xFF33, 0xFFCC33,
        0xCCCC33, 0x99CC33, 0x66CC33, 0x33CC33, 0xCC33, 0xFF9933,
        0xCC9933, 0x999933, 0x669933, 0x339933, 0x9933, 0xFF6633,
        0xCC6633, 0x996633, 0x666633, 0x336633, 0x6633, 0xFF3333,
        0xCC3333, 0x993333, 0x663333, 0x333333, 0x3333, 0xFF0033,
        0xCC0033, 0x990033, 0x660033, 0x330033, 0x33, 0xFFFF00,
        0xCCFF00, 0x99FF00, 0x66FF00, 0x33FF00, 0xFF00, 0xFFCC00,
        0xCCCC00, 0x99CC00, 0x66CC00, 0x33CC00, 0xCC00, 0xFF9900,
        0xCC9900, 0x999900, 0x669900, 0x339900, 0x9900, 0xFF6600,
        0xCC6600, 0x996600, 0x666600, 0x336600, 0x6600, 0xFF3300,
        0xCC3300, 0x993300, 0x663300, 0x333300, 0x3300, 0xFF0000,
        0xCC0000, 0x990000, 0x660000, 0x330000, 0xDD00DD, 0xEEEEEE,
        0xDDDDDD, 0xBBBBBB, 0xAAAAAA, 0x888888, 0x777777, 0x555555,
        0x444444, 0x222222, 0x111111, 0xEECCFF, 0x99BBFF, 0x88AAFF,
        0x77AAFF, 0x55AAFF, 0x44AAFF, 0x33AAFF, 0x11AAFF, 0xAAFF,
        0x2299FF, 0x1166FF, 0x66EE, 0x1166EE, 0x1166DD, 0x1166CC,
        0x1166BB, 0xFEFEFE, 0x110000, 0x11BBFF, 0xFEFEFE, 0xFEFEFE,
        0xFEFEFE, 0xFEFEFE, 0xEEFF, 0xDD, 0xFF7700, 0xEE00,
        0x339900, 0xCC3300, 0xFFFFFF
    };

    // From Symbian Source code. BMCONV. Found in S^3 and above
    static palette_256 color_256_palette_new = {
        0x00000000, 0x00000033, 0x00000066, 0x00000099, 0x000000cc, 0x000000ff,
        0x00003300, 0x00003333, 0x00003366, 0x00003399, 0x000033cc, 0x000033ff,
        0x00006600, 0x00006633, 0x00006666, 0x00006699, 0x000066cc, 0x000066ff,
        0x00009900, 0x00009933, 0x00009966, 0x00009999, 0x000099cc, 0x000099ff,
        0x0000cc00, 0x0000cc33, 0x0000cc66, 0x0000cc99, 0x0000cccc, 0x0000ccff,
        0x0000ff00, 0x0000ff33, 0x0000ff66, 0x0000ff99, 0x0000ffcc, 0x0000ffff,

        0x00330000, 0x00330033, 0x00330066, 0x00330099, 0x003300cc, 0x003300ff,
        0x00333300, 0x00333333, 0x00333366, 0x00333399, 0x003333cc, 0x003333ff,
        0x00336600, 0x00336633, 0x00336666, 0x00336699, 0x003366cc, 0x003366ff,
        0x00339900, 0x00339933, 0x00339966, 0x00339999, 0x003399cc, 0x003399ff,
        0x0033cc00, 0x0033cc33, 0x0033cc66, 0x0033cc99, 0x0033cccc, 0x0033ccff,
        0x0033ff00, 0x0033ff33, 0x0033ff66, 0x0033ff99, 0x0033ffcc, 0x0033ffff,

        0x00660000, 0x00660033, 0x00660066, 0x00660099, 0x006600cc, 0x006600ff,
        0x00663300, 0x00663333, 0x00663366, 0x00663399, 0x006633cc, 0x006633ff,
        0x00666600, 0x00666633, 0x00666666, 0x00666699, 0x006666cc, 0x006666ff,
        0x00669900, 0x00669933, 0x00669966, 0x00669999, 0x006699cc, 0x006699ff,
        0x0066cc00, 0x0066cc33, 0x0066cc66, 0x0066cc99, 0x0066cccc, 0x0066ccff,
        0x0066ff00, 0x0066ff33, 0x0066ff66, 0x0066ff99, 0x0066ffcc, 0x0066ffff,

        0x00111111, 0x00222222, 0x00444444, 0x00555555, 0x00777777,
        0x00000011, 0x00000022, 0x00000044, 0x00000055, 0x00000077,
        0x00001100, 0x00002200, 0x00004400, 0x00005500, 0x00007700,
        0x00110000, 0x00220000, 0x00440000, 0x00550000, 0x00770000,

        0x00880000, 0x00aa0000, 0x00bb0000, 0x00dd0000, 0x00ee0000,
        0x00008800, 0x0000aa00, 0x0000bb00, 0x0000dd00, 0x0000ee00,
        0x00000088, 0x000000aa, 0x000000bb, 0x000000dd, 0x000000ee,
        0x00888888, 0x00aaaaaa, 0x00bbbbbb, 0x00dddddd, 0x00eeeeee,

        0x00990000, 0x00990033, 0x00990066, 0x00990099, 0x009900cc, 0x009900ff,
        0x00993300, 0x00993333, 0x00993366, 0x00993399, 0x009933cc, 0x009933ff,
        0x00996600, 0x00996633, 0x00996666, 0x00996699, 0x009966cc, 0x009966ff,
        0x00999900, 0x00999933, 0x00999966, 0x00999999, 0x009999cc, 0x009999ff,
        0x0099cc00, 0x0099cc33, 0x0099cc66, 0x0099cc99, 0x0099cccc, 0x0099ccff,
        0x0099ff00, 0x0099ff33, 0x0099ff66, 0x0099ff99, 0x0099ffcc, 0x0099ffff,

        0x00cc0000, 0x00cc0033, 0x00cc0066, 0x00cc0099, 0x00cc00cc, 0x00cc00ff,
        0x00cc3300, 0x00cc3333, 0x00cc3366, 0x00cc3399, 0x00cc33cc, 0x00cc33ff,
        0x00cc6600, 0x00cc6633, 0x00cc6666, 0x00cc6699, 0x00cc66cc, 0x00cc66ff,
        0x00cc9900, 0x00cc9933, 0x00cc9966, 0x00cc9999, 0x00cc99cc, 0x00cc99ff,
        0x00cccc00, 0x00cccc33, 0x00cccc66, 0x00cccc99, 0x00cccccc, 0x00ccccff,
        0x00ccff00, 0x00ccff33, 0x00ccff66, 0x00ccff99, 0x00ccffcc, 0x00ccffff,

        0x00ff0000, 0x00ff0033, 0x00ff0066, 0x00ff0099, 0x00ff00cc, 0x00ff00ff,
        0x00ff3300, 0x00ff3333, 0x00ff3366, 0x00ff3399, 0x00ff33cc, 0x00ff33ff,
        0x00ff6600, 0x00ff6633, 0x00ff6666, 0x00ff6699, 0x00ff66cc, 0x00ff66ff,
        0x00ff9900, 0x00ff9933, 0x00ff9966, 0x00ff9999, 0x00ff99cc, 0x00ff99ff,
        0x00ffcc00, 0x00ffcc33, 0x00ffcc66, 0x00ffcc99, 0x00ffcccc, 0x00ffccff,
        0x00ffff00, 0x00ffff33, 0x00ffff66, 0x00ffff99, 0x00ffffcc, 0x00ffffff
    };

    inline palette_256 &get_suitable_palette_256(const epocver ver) {
        if (ver >= epocver::epoc95) {
            return color_256_palette_new;
        }

        return color_256_palette_old;
    }
}
