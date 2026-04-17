#include "CPU.h"
#include "Bus.h"

#include <iostream>
#include <format>


namespace Core {
    CPU::CPU(Bus* bus): bus(bus) {
        sp = 0xFFFE;
        pc = 0x0100;
    };

    // Combine registries into one 16-bit value
    uint16_t CPU::get_af() const {return a << 8 | f;}
    uint16_t CPU::get_bc() const {return b << 8 | c;}
    uint16_t CPU::get_de() const {return d << 8 | e;}
    uint16_t CPU::get_hl() const {return h << 8 | l;}

    // Split a 16-bit value into two registers
    void CPU::set_af(uint16_t value) {
        a = (value & 0xFF00) >> 8;
        f = value & 0x00F0; // The bottom 4 bits of the F register are always zerp
    }
    void CPU::set_bc(const uint16_t value) {b = (value & 0xFF00) >> 8; c = value & 0x00FF;}
    void CPU::set_de(const uint16_t value) {d = (value & 0xFF00) >> 8; e = value & 0x00FF;}
    void CPU::set_hl(const uint16_t value) {h = (value & 0xFF00) >> 8; l = value & 0x00FF;}

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

    void CPU::execute(uint16_t opcode) {
        switch (opcode) {
            case 0x00:
                // NOP: Do nothing
                break;
            default:
                std::cerr << std::format("Unimplemented Opcode: 0x{:02X} at PC: 0x{:04X}\n", opcode, pc - 1);
                break;
        }
    }

    void CPU::tick() {
        uint8_t opcode = fetch();
        execute(opcode);
    }
}
