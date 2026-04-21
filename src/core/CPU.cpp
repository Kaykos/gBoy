#include "CPU.h"
#include "Bus.h"

#include <iostream>
#include <format>


namespace Core {
    CPU::CPU(Bus* bus) : bus(bus) {
        uint16_t pc = 0x0000; // Start at the beginning of the Boot ROM
        uint16_t sp = 0x0000;
    };

    // Combine registries into one 16-bit value
    uint16_t CPU::get_af() const { return a << 8 | f; }
    uint16_t CPU::get_bc() const { return b << 8 | c; }
    uint16_t CPU::get_de() const { return d << 8 | e; }
    uint16_t CPU::get_hl() const { return h << 8 | l; }

    // Split a 16-bit value into two registers
    void CPU::set_af(uint16_t value) {
        a = (value & 0xFF00) >> 8;
        f = value & 0x00F0; // The bottom 4 bits of the F register are always zerp
    }

    void CPU::set_bc(const uint16_t value) {
        b = (value & 0xFF00) >> 8;
        c = value & 0x00FF;
    }

    void CPU::set_de(const uint16_t value) {
        d = (value & 0xFF00) >> 8;
        e = value & 0x00FF;
    }

    void CPU::set_hl(const uint16_t value) {
        h = (value & 0xFF00) >> 8;
        l = value & 0x00FF;
    }

    // Flag helpers
    // If setting to 1, OR against the mask
    // If setting to 0, AND against the inverse of the mask
    void CPU::set_flag_z(const bool value) {
        if (value) f |= FLAG_Z;
        else f &= ~FLAG_Z;
    }

    void CPU::set_flag_n(const bool value) {
        if (value) f |= FLAG_N;
        else f &= ~FLAG_N;
    }

    void CPU::set_flag_h(const bool value) {
        if (value) f |= FLAG_H;
        else f &= ~FLAG_H;
    }

    void CPU::set_flag_c(const bool value) {
        if (value) f |= FLAG_C;
        else f &= ~FLAG_C;
    }

    uint16_t CPU::fetch() {
        // Read byte at the current Program Counter, then increment it
        uint16_t opcode = bus->read(pc);
        pc++;

        return opcode;
    }

    uint16_t CPU::fetch_16() {
        // Read 2 consecutive bytes and combine them into one single 16-bit value
        // lower_byte: 0xFE, upper_byte: 0xFF. Combine into 0xFFFE
        const uint16_t lower_byte = fetch();
        const uint16_t upper_byte = fetch();

        // Shift the upper byte to the left by 8 bits, and combine with the lower byte
        return (upper_byte << 8) | lower_byte;
    }

    void CPU::execute(uint8_t opcode) {
        switch (opcode) {
        case 0x00:
            // NOP: Do nothing
            break;
        case 0x3E: {
            // LD A, n: Fetch the next byte from memory and load it into register A
            a = fetch();
            break;
        }
        case 0x3C: {
            // INC A: Increment register A
            bool half_carry = (a & 0x0F) == 0x0F;

            a++;

            set_flag_z(a == 0);
            set_flag_n(false);
            set_flag_h(half_carry);
            break;
        }
        case 0x31: {
            // LD SP,n16: Copy the value n16 into register SP
            sp = fetch_16();
            break;
        }
        default:
            std::cerr << std::format("Unimplemented Opcode: 0x{:02X} at PC: 0x{:04X}\n", opcode, pc - 1);
            break;
        }
    }

    void CPU::tick() {
        uint8_t opcode = fetch();
        execute(opcode);
    }

    void CPU::print_state() {
        std::cout << std::format(
            "PC:0x{:04X} | A:{:02X} F:{:02X} | B:{:02X} C:{:02X} | D:{:02X} E:{:02X} | H:{:02X} L:{:02X} | SP:0x{:04X}\n",
            pc, a, f, b, c, d, e, h, l, sp
        );
    }
}
