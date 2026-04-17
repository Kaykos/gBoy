//
// Created by bobad on 4/16/2026.
//

#ifndef GBOY_CPU_H
#define GBOY_CPU_H

#pragma once

#include <cstdint>

namespace Core {
    class Bus;

    class CPU {
    public:
        explicit CPU(Bus* bus);
        ~CPU() = default;

        void tick();

    private:
        Bus* bus;

        // 8-bit registers
        // These can be accessed as 16-bit pairs: AF, BC, DE, HL
        uint8_t a = 0, f = 0;
        uint8_t b = 0, c = 0;
        uint8_t d = 0, e = 0;
        uint8_t h = 0, l = 0;

        // 16-bit registers
        uint16_t sp; // Stack pointer
        uint16_t pc; // Program counter

        // Helpers for paired registers
        // [[nodiscard]] is used to make sure whenever the function is called, the return value is taken/assigned
        [[nodiscard]] uint16_t get_af() const;
        [[nodiscard]] uint16_t get_bc() const;
        [[nodiscard]] uint16_t get_de() const;
        [[nodiscard]] uint16_t get_hl() const;

        void set_af(uint16_t value);
        void set_bc(uint16_t value);
        void set_de(uint16_t value);
        void set_hl(uint16_t value);

        // Execution functions
        uint16_t fetch();
        void execute(uint16_t opcode);
    };
}

#endif //GBOY_CPU_H
