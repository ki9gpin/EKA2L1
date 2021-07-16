/*
 * Copyright (c) 2019 EKA2L1 Team.
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

#include <cassert>

#include <services/ecom/plugin.h>

#include <common/buffer.h>
#include <common/chunkyseri.h>

namespace eka2l1 {
    static bool read_ecom_str_16(common::ro_buf_stream &stream, std::u16string &buf) {
        std::uint8_t length = 0;
        stream.read(&length, 1);

        if (length != 0 && (stream.tell() & 0x1)) {
            // Misalign, there should be a padding byte
            std::uint8_t padding = 0;
            stream.read(&padding, 1);

            assert(padding == 0xAB);
        }

        buf.resize(length);
        stream.read(&buf[0], buf.length() * 2);

        return true;
    }

    static bool read_ecom_str(common::ro_buf_stream &stream, std::string &buf,
        const bool is_len_16_bit = false) {
        std::uint32_t length = 0;

        if (is_len_16_bit) {
            std::uint16_t len16 = 0;
            stream.read(&len16, 2);

            length = len16;
        } else {
            std::uint8_t len8 = 0;
            stream.read(&len8, 1);

            length = len8;
        }

        if (length == 0) {
            return true;
        }

        buf.resize(length);
        stream.read(&buf[0], buf.size());

        return true;
    }

    static bool read_impl_ver1(ecom_implementation_info &info, common::ro_buf_stream &stream) {
        stream.read(&info.uid, 4);
        stream.read(&info.version, 1);

        read_ecom_str_16(stream, info.display_name);
        read_ecom_str(stream, info.default_data);
        read_ecom_str(stream, info.opaque_data);

        return true;
    }

    static bool read_impl_ver2(ecom_implementation_info &info, common::ro_buf_stream &stream) {
        if (!read_impl_ver1(info, stream)) {
            return false;
        }

        std::uint8_t is_in_rom = 0;
        stream.read(&is_in_rom, 1);

        if (is_in_rom) {
            info.flags |= ecom_implementation_info::FLAG_ROM_ONLY;
        }

        return true;
    }

    static bool read_ecom_arr_string(common::ro_buf_stream &stream, std::string &dest_buf) {
        std::uint16_t total_strings = 0;
        stream.read(&total_strings, 2);

        // Only allows maximum of 2 strings, or none
        if (total_strings > 2) {
            return false;
        }

        for (std::uint16_t i = 0; i < total_strings; i++) {
            std::string temp_string;
            read_ecom_str(stream, temp_string, false);

            // Prevent if deletes all of our \0 data
            dest_buf.insert(dest_buf.begin(), temp_string.begin(), temp_string.end());
        }

        return true;
    }

    static bool read_impl_ver3(ecom_implementation_info &info, common::ro_buf_stream &stream) {
        stream.read(&info.format, 1);
        stream.read(&info.uid, 4);
        stream.read(&info.version, 1);

        read_ecom_str_16(stream, info.display_name);

        // Ver 3 was intended to support more large name and opaque data
        // For string storage option, now:
        // You can choose: Either an array of string, or a string with 16-bit length
        switch (info.format) {
        // Format 1: Array of strings
        case 1: {
            read_ecom_arr_string(stream, info.default_data);
            read_ecom_arr_string(stream, info.opaque_data);

            break;
        }

        // Format 2: 16 bit string
        case 2: {
            read_ecom_str(stream, info.default_data, true);
            read_ecom_str(stream, info.opaque_data, true);

            break;
        }

        default: {
            // Unsupported
            return false;
        }
        }

        // Read list of extended interfaces
        std::uint16_t extended_interfaces_count = 0;
        stream.read(&extended_interfaces_count, 2);

        info.extended_interfaces.resize(extended_interfaces_count);

        for (std::uint16_t i = 0; i < extended_interfaces_count; i++) {
            stream.read(&info.extended_interfaces[i], 4);
        }

        std::sort(info.extended_interfaces.begin(), info.extended_interfaces.end());

        // Read flags
        std::uint8_t given_flags = 0;
        stream.read(&given_flags, 1);

        if (given_flags & 1) {
            info.flags |= ecom_implementation_info::FLAG_ROM_ONLY;
        }

        return true;
    }

    bool load_plugin(loader::rsc_file &rsc, ecom_plugin &plugin) {
        // Ecom resource index is 1
        std::vector<std::uint8_t> ecom_rsc_data = rsc.read(1);
        common::ro_buf_stream stream(ecom_rsc_data.data(), ecom_rsc_data.size());

        // Read the UID
        // In format 2 and 3, there is a magic, but in format 1, we goes straight to plugin UID
        stream.read(&plugin.type, 4);

        if (plugin.type == ecom_plugin_type_2 || plugin.type == ecom_plugin_type_3) {
            stream.read(&plugin.uid, 4);
        } else {
            plugin.uid = plugin.type;
            plugin.type = 0;
        }

        // Next 2 bytes is total interface
        std::uint16_t total_interfaces = 0;
        stream.read(&total_interfaces, 2);

        // This is certainly a hack to prevent file which is not valid popping in our database
        // Many resources file don't even exceed 5 interfaces
        if (total_interfaces > 10) {
            return false;
        }

        if (plugin.type == ecom_plugin_type_3 && total_interfaces > 3) {
            return false;
        }

        if (total_interfaces == 0) {
            return true;
        }

        plugin.interfaces.resize(total_interfaces);

        // Depend on the type of ECOM resource, we parse the interface description in different
        // way. There are three versions, and 3 way to read it

        // Interface is a specification, what bare bones. It lists a bunch of functions that is useful
        // but we can't use it because nothing is implemented
        // Implementation is interfaces but functional can usable

        // ECom resource files contains a list of interfaces info, each interface will come with some
        // implementation infos.
        for (auto &interface : plugin.interfaces) {
            stream.read(&interface.uid, 4);

            std::uint16_t total_impls = 0;
            stream.read(&total_impls, 2);

            if (plugin.type == ecom_plugin_type_3 && total_impls > 8) {
                return false;
            }

            if (total_impls == 0) {
                continue;
            }

            interface.implementations.resize(total_impls);

            for (auto &implementation : interface.implementations) {
                implementation = std::make_shared<ecom_implementation_info>();

                switch (plugin.type) {
                case ecom_plugin_type_3: {
                    if (!read_impl_ver3(*implementation, stream)) {
                        return false;
                    }

                    break;
                }

                case ecom_plugin_type_2: {
                    implementation->flags |= ecom_implementation_info::FLAG_HINT_NO_EXTENDED_INTERFACE;

                    if (!read_impl_ver2(*implementation, stream)) {
                        return false;
                    }

                    break;
                }

                default: {
                    if (!read_impl_ver1(*implementation, stream)) {
                        return false;
                    }

                    implementation->flags |= ecom_implementation_info::FLAG_HINT_NO_EXTENDED_INTERFACE;

                    break;
                }
                }
            }
        }

        return true;
    }

    void ecom_implementation_info::do_state(common::chunkyseri &seri, const bool support_extended_interface,
        const bool old_abi) {
        seri.absorb(uid);

        std::uint32_t ver32 = version;
        seri.absorb(ver32);

        if (seri.get_seri_mode() == common::SERI_MODE_READ) {
            version = static_cast<std::uint8_t>(ver32);
        }

        if (old_abi) {
            // TODO(pent0): Disable flag check
            std::uint32_t disabled = 0;
            seri.absorb(disabled);

            if (seri.get_seri_mode() == common::SERI_MODE_READ) {
                flags |= FLAG_DISABLED;
            }
        }

        seri.absorb(display_name);
        seri.absorb(default_data);
        seri.absorb(opaque_data);

        // Absorb drive and VID (?)
        std::uint32_t drv32 = static_cast<decltype(drv32)>(drv);
        seri.absorb(drv32);

        if (seri.get_seri_mode() == common::SERI_MODE_READ) {
            drv = static_cast<drive_number>(drv32);
        }

        if (old_abi) {
            std::uint32_t is_rom_only = (flags & FLAG_ROM_ONLY);
            seri.absorb(is_rom_only);

            // TODO(pent0): Dangerous hardcode
            std::uint32_t is_rom_based = (drv == drive_z);
            seri.absorb(is_rom_based);

            if (seri.get_seri_mode() == common::SERI_MODE_READ) {
                if (is_rom_only) {
                    flags |= FLAG_ROM_ONLY;
                }

                if (is_rom_based) {
                    flags |= FLAG_ROM_BASED;
                }
            }
        }

        // TODO: What is this ?? Version ID?
        std::uint32_t vid = 0;
        seri.absorb(vid);

        // Bit 0: Rom only, bit 1: Rom based, bit 2: Disabled ?
        if (!old_abi) {
            std::uint8_t absorb_flags = 0;
            if (flags & FLAG_ROM_ONLY) {
                absorb_flags |= 0b1;
            }

            // TODO: Hardcode is dangerous
            if (drv == drive_z) {
                absorb_flags |= 0b10;
            }

            if (flags & FLAG_DISABLED) {
                absorb_flags |= 0b100;
            }

            seri.absorb(absorb_flags);
            if (seri.get_seri_mode() == common::SERI_MODE_READ) {
                if ((absorb_flags & 0b10) || (absorb_flags & 0b01))
                    flags |= FLAG_ROM_ONLY | FLAG_ROM_BASED;

                if (absorb_flags & 0b100)
                    flags |= FLAG_DISABLED;
            }
        }

        // Absorb extended interfaces
        // There is a difference in S^3 and S9.4 and down: GetExtendedInterfaceListL does not exist in
        // Symbian 9.4 document or any lower ECOM documents.

        // Causious about this.
        if (support_extended_interface) {
            if (extended_interfaces.size() == 0) {
                std::int32_t none_interface_count = -1;
                seri.absorb(none_interface_count);
            } else {
                seri.absorb_container(extended_interfaces);
            }
        }
    }
}
