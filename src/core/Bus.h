//
// Created by bobad on 4/16/2026.
//

#ifndef GBOY_BUS_H
#define GBOY_BUS_H

#pragma once

#include <array>
#include <string>

namespace Core {
    class Bus {
    public:
        Bus();
        ~Bus() = default;

        // Basic operations
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t value);

        // Boot sequence
        bool load_boot_rom(const std::string& file_path);
    private:
        std::array<uint8_t,0x10000> memory{};
    };
}

#endif //GBOY_BUS_H
