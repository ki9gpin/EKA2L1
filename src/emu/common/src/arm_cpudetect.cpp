// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/

#include <common/platform.h>

#if EKA2L1_ARCH(ARM) || EKA2L1_ARCH(ARM64)

#include <common/cpudetect.h>

#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Only Linux platforms have /proc/cpuinfo
#if EKA2L1_PLATFORM(UNIX)
#include <fstream>
#include <sstream>

const char procfile[] = "/proc/cpuinfo";
// https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-devices-system-cpu
const char syscpupresentfile[] = "/sys/devices/system/cpu/present";

std::string get_cpu_string() {
    std::string cpu_string;
    std::fstream file(procfile, std::ios::in);

    if (!file.fail()) {
        std::string line, marker = "Hardware\t: ";
        while (std::getline(file, line)) {
            if (line.find(marker) != std::string::npos) {
                cpu_string = line.substr(marker.length());
            }
        }
    }

    if (cpu_string.empty()) {
        cpu_string = "Unknown";
    } else if (cpu_string.back() == '\n') {
        cpu_string.pop_back(); // Drop the new-line character
    }

    return cpu_string;
}

std::string get_cpu_brand_string() {
    std::string brand_string;
    std::fstream file(procfile, std::ios::in);

    if (!file.fail()) {
        std::string line, marker = "Processor\t: ";
        while (std::getline(file, line)) {
            if (line.find(marker) != std::string::npos) {
                brand_string = line.substr(marker.length());
                if (brand_string.length() != 0 && !isdigit(brand_string[0])) {
                    break;
                }
            }
        }
    }

    if (brand_string.empty()) {
        brand_string = "Unknown";
    } else if (brand_string.back() == '\n') {
        brand_string.pop_back(); // Drop the new-line character
    }

    return brand_string;
}

unsigned char get_cpu_implementer() {
    std::string line, marker = "CPU implementer\t: ";
    unsigned char implementer = 0;
    std::fstream file(procfile, std::ios::in);

    if (file.fail()) {
        return 0;
    }

    while (std::getline(file, line)) {
        if (line.find(marker) != std::string::npos) {
            line = line.substr(marker.length());
            sscanf(line.c_str(), "0x%02hhx", &implementer);
            break;
        }
    }

    return implementer;
}

unsigned short get_cpu_part() {
    std::string line, marker = "CPU part\t: ";
    unsigned short part = 0;
    std::fstream file(procfile, std::ios::in);

    if (file.fail()) {
        return 0;
    }

    while (std::getline(file, line)) {
        if (line.find(marker) != std::string::npos) {
            line = line.substr(marker.length());
            sscanf(line.c_str(), "0x%03hx", &part);
            break;
        }
    }

    return part;
}

bool check_cpu_feature(const std::string &feature) {
    std::string line, marker = "Features\t: ";
    std::fstream file(procfile, std::ios::in);

    if (file.fail()) {
        return 0;
    }

    while (std::getline(file, line)) {
        if (line.find(marker) != std::string::npos) {
            std::stringstream line_stream(line);
            std::string token;
            while (std::getline(line_stream, token, ' ')) {
                if (token == feature)
                    return true;
            }
        }
    }

    return false;
}

int get_core_count() {
    std::string line, marker = "processor\t: ";
    int cores = 1;
    std::fstream file(syscpupresentfile, std::ios::in);

    if (!file.fail()) {
        int low, high, found;
        std::getline(file, line);
        found = sscanf(line.c_str(), "%d-%d", &low, &high);
        if (found == 1)
            return 1;
        if (found == 2)
            return high - low + 1;
    }

    file.open(procfile, std::ios::in);

    if (file.fail()) {
        return 1;
    }

    while (std::getline(file, line)) {
        if (line.find(marker) != std::string::npos)
            ++cores;
    }

    return cores;
}
#endif

namespace eka2l1::common {

    // Detects the various cpu features
    void cpu_info::detect() {
        // Set some defaults here
        HTT = false;
#if EKA2L1_ARCH(ARM64)
        OS64bit = true;
        CPU64bit = true;
        Mode64bit = true;
#else
        OS64bit = false;
        CPU64bit = false;
        Mode64bit = false;
#endif

#if EKA2L1_ARCH(ARMV7)
        bARMv7 = true;
#endif
        
        vendor = VENDOR_ARM;

        // Get the information about the CPU
#if !EKA2L1_PLATFORM(UNIX)
        bool isVFP3 = false;
        bool isVFP4 = false;
#if EKA2L1_PLATFORM(IOS)
        isVFP3 = true;
        // Check for swift arch (VFP4)
#ifdef __ARM_ARCH_7S__
        isVFP4 = true;
#endif
        strcpy(brand_string, "Apple A", sizeof(brand_string));
        num_cores = 2;
#elif EKA2L1_PLATFORM(UWP)
        strcpy(brand_string, "Unknown", sizeof(brand_string));
        isVFP3 = true;
        isVFP4 = false;
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        num_cores = sysInfo.dwNumberOfProcessors;
#else // !EKA2L1_PLATFORM(IOS)
        strcpy(brand_string, "Unknown");
        num_cores = 1;
#endif
        strncpy(cpu_string, brand_string, sizeof(cpu_string));
        // Hardcode this for now
        bSwp = true;
        bHalf = true;
        bThumb = false;
        bFastMult = true;
        bVFP = true;
        bEDSP = true;
        bThumbEE = isVFP3;
        bNEON = isVFP3;
        bVFPv3 = isVFP3;
        bTLS = true;
        bVFPv4 = isVFP4;
        bIDIVa = isVFP4;
        bIDIVt = isVFP4;
        bFP = false;
        bASIMD = false;
#else // EKA2L1_PLATFORM(UNIX)
        strncpy(cpu_string, get_cpu_string().c_str(), sizeof(cpu_string));
        strncpy(brand_string, get_cpu_brand_string().c_str(), sizeof(brand_string));

        bSwp = check_cpu_feature("swp");
        bHalf = check_cpu_feature("half");
        bThumb = check_cpu_feature("thumb");
        bFastMult = check_cpu_feature("fastmult");
        bVFP = check_cpu_feature("vfp");
        bEDSP = check_cpu_feature("edsp");
        bThumbEE = check_cpu_feature("thumbee");
        bNEON = check_cpu_feature("neon");
        bVFPv3 = check_cpu_feature("vfpv3");
        bTLS = check_cpu_feature("tls");
        bVFPv4 = check_cpu_feature("vfpv4");
        bIDIVa = check_cpu_feature("idiva");
        bIDIVt = check_cpu_feature("idivt");
        // Qualcomm Krait supports IDIVA but it doesn't report it. Check for krait (0x4D = Plus, 0x6F = Pro).
        unsigned short CPUPart = get_cpu_part();
        if (get_cpu_implementer() == 0x51 && (CPUPart == 0x4D || CPUPart == 0x6F))
            bIDIVa = bIDIVt = true;
        // These two require ARMv8 or higher
        bFP = check_cpu_feature("fp");
        bASIMD = check_cpu_feature("asimd");
        num_cores = get_core_count();
#endif
#if EKA2L1_ARCH(ARM64)
        // Whether the above detection failed or not, on ARM64 we do have ASIMD/NEON.
        bNEON = true;
        bASIMD = true;
#endif
    }

}

#endif