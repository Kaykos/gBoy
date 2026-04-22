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

        void print_state();
    private:
        Bus* bus;

        // -- 8-bit registers --
        // These can be accessed as 16-bit pairs: AF, BC, DE, HL
        uint8_t a = 0, f = 0;
        uint8_t b = 0, c = 0;
        uint8_t d = 0, e = 0;
        uint8_t h = 0, l = 0;

        // -- 16-bit registers --
        uint16_t sp; // Stack pointer
        uint16_t pc; // Program counter

        // -- Helpers for registers
        void inc_8bit(uint8_t& reg);
        void dec_8bit(uint8_t& reg);
        void compare_a(uint8_t value);

        // -- Helpers for paired registers --
        // [[nodiscard]] is used to make sure whenever the function is called, the return value is taken/assigned
        [[nodiscard]] uint16_t get_af() const;
        [[nodiscard]] uint16_t get_bc() const;
        [[nodiscard]] uint16_t get_de() const;
        [[nodiscard]] uint16_t get_hl() const;

        void set_af(uint16_t value);
        void set_bc(uint16_t value);
        void set_de(uint16_t value);
        void set_hl(uint16_t value);

        // -- Flag helpers --
        // Bit masks to extract the value of specific bits
        static constexpr uint8_t FLAG_Z = 1 << 7; // Zero flag. Bit #7
        static constexpr uint8_t FLAG_N = 1 << 6; // Substraction flag. Bit #6
        static constexpr uint8_t FLAG_H = 1 << 5; // Half carry flag. Bit #5
        static constexpr uint8_t FLAG_C = 1 << 4; // Carry flag. Bit #4

        [[nodiscard]] bool get_flag_z() const {return f & FLAG_Z;}
        [[nodiscard]] bool get_flag_n() const {return f & FLAG_N;}
        [[nodiscard]] bool get_flag_h() const {return f & FLAG_H;}
        [[nodiscard]] bool get_flag_c() const {return f & FLAG_C;}

        void set_flag_z(bool value);
        void set_flag_n(bool value);
        void set_flag_h(bool value);
        void set_flag_c(bool value);

        // Stack functions
        void stack_push16(uint16_t value);
        void stack_pop16(uint8_t &upper_byte, uint8_t &lower_byte);
        uint16_t stack_pop16();

        // For 0xCB opcodes
        void check_bit(uint8_t value, uint8_t bit_index);
        void rl(uint8_t& reg);

        // Execution functions
        uint16_t fetch();
        uint16_t fetch_16();
        void execute(uint8_t opcode);
        void execute_cb(uint8_t cb_opcode);
    };
}

#endif //GBOY_CPU_H
