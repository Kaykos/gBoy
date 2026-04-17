//
// Created by bobad on 4/16/2026.
//

#ifndef GBOY_BUS_H
#define GBOY_BUS_H

#pragma once

#include <cstdint>
#include <array>

namespace Core {
    class Bus {
    public:
        Bus();
        ~Bus() = default;

        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t value);
    private:
        std::array<uint8_t,0x10000> memory{};
    };
}

#endif //GBOY_BUS_H
