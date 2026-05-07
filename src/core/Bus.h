//
// Created by bobad on 4/16/2026.
//

#ifndef GBOY_BUS_H
#define GBOY_BUS_H

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace Core {
    class Bus {
    public:
        Bus();
        ~Bus() = default;

        // Basic operations
        [[nodiscard]] uint8_t read(uint16_t address) const;
        void write(uint16_t address, uint8_t value);

        // Boot sequence
        bool load_boot_rom(const std::string& file_path);
        bool load_cartridge(const std::string& file_path);
    private:
        std::array<uint8_t,0x10000> memory{};

        std::array<uint8_t, 256> boot_rom{};

        // 2 16KB ROM bank (0x0000 - 0x7FFF)
        std::vector<uint8_t> cartridge_memory;

        // 8KB Video RAM (0x8000 - 0x9FFF)
        std::array<uint8_t, 0x2000> vram{};

        // 8KB Work RAM (0xC000 - 0xDFFF)
        std::array<uint8_t, 0x2000> wram{};

        // High RAM / Zero Page (0xFF80 - 0xFFFE)
        std::array<uint8_t, 127> hram{};

        bool boot_rom_active = true;
    };
}

#endif //GBOY_BUS_H
