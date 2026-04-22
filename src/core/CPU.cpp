#include "CPU.h"
#include "Bus.h"

#include <iostream>
#include <format>


namespace Core {
    CPU::CPU(Bus *bus) : bus(bus) {
        pc = 0x0000; // Start at the beginning of the Boot ROM
        sp = 0x0000;
    }

    void CPU::inc_8bit(uint8_t &reg) {
        // INC r8: Increment the value in register r8 by 1
        bool half_carry = (reg & 0x0F) == 0x0F; // Check if the lower nibble is maxed out (15) before adding 1

        reg++;

        set_flag_z(reg == 0);
        set_flag_n(false);
        set_flag_h(half_carry);
    }

    void CPU::dec_8bit(uint8_t &reg) {
        // DEC r8: Decrement the value in register r8 by 1
        bool half_borrow = (reg & 0x0F) == 0x00; // Check if the lower nibble is 0. If it is, subtracting 1 will borrow from bit 4

        reg--;

        set_flag_z(reg == 0);
        set_flag_n(true);
        set_flag_h(half_borrow);
    }

    void CPU::compare_a(uint8_t value) {
        // Compare the value in A with the given value
        uint8_t result = a - value;
        // Half-borrow happens if the lower nibble of A is smaller than the lower nibble of the value
        bool half_borrow = (a & 0x0F) < (value & 0x0F);

        set_flag_z(result == 0);
        set_flag_n(true);
        set_flag_h(half_borrow);
        set_flag_c(value > a);
    }

    // Combine registries into one 16-bit value
    uint16_t CPU::get_af() const { return a << 8 | f; }
    uint16_t CPU::get_bc() const { return b << 8 | c; }
    uint16_t CPU::get_de() const { return d << 8 | e; }
    uint16_t CPU::get_hl() const { return h << 8 | l; }

    // Split a 16-bit value into two registers
    void CPU::set_af(uint16_t value) {
        a = (value & 0xFF00) >> 8;
        f = value & 0x00F0; // The bottom 4 bits of the F register are always zero
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

    void CPU::stack_push16(uint16_t value) {
        uint8_t upper_byte = value >> 8;
        uint8_t lower_byte = value & 0xFF;

        // When pushing data, the stack pointer moves backward in memory
        sp--;
        bus->write(sp, upper_byte);

        sp--;
        bus->write(sp, lower_byte);
    }

    void CPU::stack_pop16(uint8_t &upper_byte, uint8_t &lower_byte) {
        // When popping data, the stack pointer moves forward in memory
        lower_byte = bus->read(sp);
        sp++;

        upper_byte = bus->read(sp);
        sp++;
    }

    uint16_t CPU::stack_pop16() {
        // Pop 2 consecutive 8-bit values merged into one value
        uint8_t lower_byte = bus->read(sp);
        sp++;

        uint8_t upper_byte = bus->read(sp);
        sp++;

        // Combine into 16-bit value
        return (upper_byte << 8) | lower_byte;
    }

    void CPU::check_bit(uint8_t value, uint8_t bit_index) {
        // Check the bit at position bit_index of value
        const uint8_t mask = 1 << bit_index;

        // Sets the zero flag depending on the bit value
        set_flag_z((value & mask) == 0);

        set_flag_n(false);
        set_flag_h(true);
    }

    void CPU::rl(uint8_t& reg) {
        // Rotate bits to the left, through the Carry flag
        bool bit7 = (reg & 0x80) != 0;
        bool old_carry = get_flag_c();

        // Shift to the left
        reg <<= 1;

        // Inject old_carry in first bit
        if (old_carry) {
            reg |= 1;
        }

        set_flag_z(reg == 0);
        set_flag_n(false);
        set_flag_h(false);
        set_flag_c(bit7);
    }

    void CPU::execute(uint8_t opcode) {
        switch (opcode) {
            case 0x00:
                // NOP: Do nothing
                break;
            // ---- Load instructions ----
            case 0x06: {
                // LD B, n8: Fetch the next byte from memory and load it into register B
                b = fetch();
                break;
            }
            case 0x16: {
                // LD D, n8: Fetch the next byte from memory and load it into register D
                d = fetch();
                break;
            }
            case 0x26: {
                // LD H, n8: Fetch the next byte from memory and load it into register H
                h = fetch();
                break;
            }
            case 0x0E: {
                // LD C, n8: Fetch the next byte from memory and load it into register C
                c = fetch();
                break;
            }
            case 0x0A: {
                // LD A, [BC]: Copy the value pointed to by BC into register A
                a = bus->read(get_bc());

                break;
            }
            case 0x1A: {
                // LD A, [DE]: Copy the value pointed to by DE into register A
                a = bus->read(get_de());

                break;
            }
            case 0x1E: {
                // LD E, n8: Fetch the next byte from memory and load it into register E
                e = fetch();
                break;
            }
            case 0x2E: {
                // LD L, n8: Fetch the next byte from memory and load it into register L
                l = fetch();
                break;
            }
            case 0x3E: {
                // LD A, n8: Fetch the next byte from memory and load it into register A
                a = fetch();
                break;
            }
            case 0x22: {
                // LD [HLI], A: Copy the value in register A into the byte pointed by HL and increment HL afterward
                // Written as LD [HL+], A or LDI [HL], A
                bus->write(get_hl(), a);
                set_hl(get_hl() + 1);

                break;
            }
            case 0x2A: {
                // LD A, [HLI]: Copy the value pointed to by HL into A, then increment HL
                a = bus->read(get_hl());
                set_hl(get_hl() + 1);

                break;
            }
            case 0x32: {
                // LD [HLD], A: Copy the value in register A into the byte pointed by HL and decrement HL afterward
                // Written as LD [HL-], A or LDD [HL], A
                bus->write(get_hl(), a);
                set_hl(get_hl() - 1);

                break;
            }
            // @formatter:off
            // 0x40 - 0x47: Destination B
            case 0x40: { b = b; break; } // LD B, B
            case 0x41: { b = c; break; } // LD B, C
            case 0x42: { b = d; break; } // LD B, D
            case 0x43: { b = e; break; } // LD B, E
            case 0x44: { b = h; break; } // LD B, H
            case 0x45: { b = l; break; } // LD B, L
            case 0x46: { b = bus->read(get_hl()); break; } // LD B, [HL]
            case 0x47: { b = a; break; } // LD B, A

            // 0x48 - 0x4F: Destination C
            case 0x48: { c = b; break; } // LD C, B
            case 0x49: { c = c; break; } // LD C, C
            case 0x4A: { c = d; break; } // LD C, D
            case 0x4B: { c = e; break; } // LD C, E
            case 0x4C: { c = h; break; } // LD C, H
            case 0x4D: { c = l; break; } // LD C, L
            case 0x4E: { c = bus->read(get_hl()); break; } // LD C, [HL]
            case 0x4F: { c = a; break; } // LD C, A

            // 0x50 - 0x57: Destination D
            case 0x50: { d = b; break; } // LD D, B
            case 0x51: { d = c; break; } // LD D, C
            case 0x52: { d = d; break; } // LD D, D
            case 0x53: { d = e; break; } // LD D, E
            case 0x54: { d = h; break; } // LD D, H
            case 0x55: { d = l; break; } // LD D, L
            case 0x56: { d = bus->read(get_hl()); break; } // LD D, [HL]
            case 0x57: { d = a; break; } // LD D, A

            // 0x58 - 0x5F: Destination E
            case 0x58: { e = b; break; } // LD E, B
            case 0x59: { e = c; break; } // LD E, C
            case 0x5A: { e = d; break; } // LD E, D
            case 0x5B: { e = e; break; } // LD E, E
            case 0x5C: { e = h; break; } // LD E, H
            case 0x5D: { e = l; break; } // LD E, L
            case 0x5E: { e = bus->read(get_hl()); break; } // LD E, [HL]
            case 0x5F: { e = a; break; } // LD E, A

            // 0x60 - 0x67: Destination H
            case 0x60: { h = b; break; } // LD H, B
            case 0x61: { h = c; break; } // LD H, C
            case 0x62: { h = d; break; } // LD H, D
            case 0x63: { h = e; break; } // LD H, E
            case 0x64: { h = h; break; } // LD H, H
            case 0x65: { h = l; break; } // LD H, L
            case 0x66: { h = bus->read(get_hl()); break; } // LD H, [HL]
            case 0x67: { h = a; break; } // LD H, A

            // 0x68 - 0x6F: Destination L
            case 0x68: { l = b; break; } // LD L, B
            case 0x69: { l = c; break; } // LD L, C
            case 0x6A: { l = d; break; } // LD L, D
            case 0x6B: { l = e; break; } // LD L, E
            case 0x6C: { l = h; break; } // LD L, H
            case 0x6D: { l = l; break; } // LD L, L
            case 0x6E: { l = bus->read(get_hl()); break; } // LD L, [HL]
            case 0x6F: { l = a; break; } // LD L, A

            // 0x70 - 0x77: Destination [HL] (Memory Writes)
            case 0x70: { bus->write(get_hl(), b); break; } // LD [HL], B
            case 0x71: { bus->write(get_hl(), c); break; } // LD [HL], C
            case 0x72: { bus->write(get_hl(), d); break; } // LD [HL], D
            case 0x73: { bus->write(get_hl(), e); break; } // LD [HL], E
            case 0x74: { bus->write(get_hl(), h); break; } // LD [HL], H
            case 0x75: { bus->write(get_hl(), l); break; } // LD [HL], L
            case 0x77: { bus->write(get_hl(), a); break; } // LD [HL], A

            // 0x78 - 0x7F: Destination A
            case 0x78: { a = b; break; } // LD A, B
            case 0x79: { a = c; break; } // LD A, C
            case 0x7A: { a = d; break; } // LD A, D
            case 0x7B: { a = e; break; } // LD A, E
            case 0x7C: { a = h; break; } // LD A, H
            case 0x7D: { a = l; break; } // LD A, L
            case 0x7E: { a = bus->read(get_hl()); break; } // LD A, [HL]
            case 0x7F: { a = a; break; } // LD A, A
            // @formatter:on

            case 0xE0: {
                // LDH [n16], A: Copy the value in register A into the byte at address n16
                const uint8_t offset = fetch();
                const uint16_t address = 0xFF00 + offset;
                bus->write(address, a);

                break;
            }
            case 0xF0: {
                // LDH A, [a8]: Copy the byte at High RAM address (0xFF00 + a8) into register A
                uint8_t offset = fetch();
                uint16_t address = 0xFF00 + offset;
                a = bus->read(address);

                break;
            }
            case 0xE2: {
                // LDH [C], A: Copy the value in register A into the byte at address 0xFF00+C
                const uint16_t address = 0xFF00 + c;
                bus->write(address, a);

                break;
            }
            // -- 16-bit registries --
            case 0x01: {
                // LD BC, n16: Copy the value n16 into register BC
                uint16_t value = fetch_16();
                set_bc(value);
                break;
            }
            case 0x11: {
                // LD DE, n16: Copy the value n16 into register DE
                uint16_t value = fetch_16();
                set_de(value);
                break;
            }
            case 0x21: {
                // LD HL, n16: Copy the value n16 into registers HL
                uint16_t value = fetch_16();
                set_hl(value);
                break;
            }
            case 0x31: {
                // LD SP, n16: Copy the value n16 into register SP
                uint16_t value = fetch_16();
                sp = value;
                break;
            }
            case 0xEA: {
                // LD [a16], A: Copy the value in register A into the byte at address n16
                uint16_t address = fetch_16();
                bus->write(address, a);
                break;
            }
            case 0xFA: {
                // LD A, [a16]: Copy the value at memory address a16 into register A
                uint16_t address = fetch_16();
                a = bus->read(address);

                // Flags: - - - -
                break;
            }
            // ---- 8-bit arithmetic instructions ----
            // @formatter:off
            case 0xB8: { compare_a(b); break; } // CP A, B
            case 0xB9: { compare_a(c); break; } // CP A, C
            case 0xBA: { compare_a(d); break; } // CP A, D
            case 0xBB: { compare_a(e); break; } // CP A, E
            case 0xBC: { compare_a(h); break; } // CP A, H
            case 0xBD: { compare_a(l); break; } // CP A, L
            case 0xBE: { compare_a(bus->read(get_hl())); break; } // CP A, [HL]
            case 0xBF: { compare_a(a); break; } // CP A, A
            case 0xFE: { compare_a(fetch()); break; } // CP A, n8
            case 0x04: { inc_8bit(b); break; } // INC B
            case 0x14: { inc_8bit(d); break; } // INC D
            case 0x24: { inc_8bit(h); break; } // INC H
            case 0x0C: { inc_8bit(c); break; } // INC C
            case 0x1C: { inc_8bit(e); break; } // INC E
            case 0x2C: { inc_8bit(l); break; } // INC L
            case 0x3C: { inc_8bit(a); break; } // INC A
            case 0x05: { dec_8bit(b); break; } // DEC B
            case 0x0D: { dec_8bit(c); break; } // DEC C
            case 0x15: { dec_8bit(d); break; } // DEC D
            case 0x1D: { dec_8bit(e); break; } // DEC E
            case 0x25: { dec_8bit(h); break; } // DEC H
            case 0x2D: { dec_8bit(l); break; } // DEC L
            case 0x3D: { dec_8bit(a); break; } // DEC A
            // ---- 16-bit arithmetic instructions ----
            case 0x03: { set_bc(get_bc() + 1); break; } // INC BC
            case 0x13: { set_de(get_de() + 1); break; } // INC DE
            case 0x23: { set_hl(get_hl() + 1); break; } // INC HL
            case 0x33: { sp++; break; } // INC SP
            // @formatter:on
            // ---- Bitwise logic instructions ----
            case 0xAF: {
                // XOR A, A: Set A to the bitwise XOR between the value of A and itself
                a ^= a;

                set_flag_z(a == 0); // Set to true if the result is exactly zero
                set_flag_n(false);
                set_flag_h(false);
                set_flag_c(false);
                break;
            }
            case 0xCB: {
                // 0xCB is a prefix for additional opcodes.
                const uint8_t cb_opcode = fetch();
                execute_cb(cb_opcode);
                break;
            }
            // ---- Bit shift instructions ----
            case 0x17: {
                // RLA: Rotate A left through Carry. Forcefully clears the Zero flag!
                rl(a);
                set_flag_z(false);
                break;

            }
            // ---- Jumps and subroutine instructions ----
            case 0x18: {
                // JR e8: Unconditional relative jump
                const auto offset = static_cast<int8_t>(fetch());
                pc += offset;

                break;
            }
            case 0x20: {
                // JR NZ, e8: Relative jump to address n16 if condition (Not Zero) is met
                const uint8_t value = fetch();
                const auto offset = static_cast<int8_t>(value); // Relative jumps treat n16 as an 8-bit signed integer

                // Not Zero means the last operation result was not zero, so the zero flag is false
                if (!get_flag_z()) {
                    pc += offset;
                }

                break;
            }
            case 0x28: {
                // JR Z, e8: Relative jump to address n16 if condition (Zero) is met
                const uint8_t value = fetch();
                const auto offset = static_cast<int8_t>(value); // Relative jumps treat n16 as an 8-bit signed integer

                // Zero means the last operation result was zero, so the zero flag is true
                if (get_flag_z()) {
                    pc += offset;
                }

                break;
            }
            case 0x30: {
                // JR NC, e8: Relative jump if Carry flag is false (Not Carry)
                const auto offset = static_cast<int8_t>(fetch());
                if (!get_flag_c()) {
                    pc += offset;
                }

                break;
            }
            case 0x38: {
                // JR C, e8: Relative jump if Carry flag is true (Carry)
                const auto offset = static_cast<int8_t>(fetch());
                if (get_flag_c()) {
                    pc += offset;
                }

                break;
            }
            case 0xCD: {
                // CALL n16: Pushes the address of the instruction after the CALL on the stack, such that RET can pop it later;
                // then, it executes an implicit JP n16.
                uint16_t address = fetch_16(); // Get jump address
                stack_push16(pc); // Store previous address
                pc = address; // Jump to address

                break;
            }
            case 0xC9: {
                // RET: Return from subroutine
                pc = stack_pop16();
                break;
            }
            // ---- Stack manipulation instructions ----
            case 0xC1: {
                // POP BC: Pop register BC from the stack
                stack_pop16(b, c);
                break;
            }
            case 0xD1: {
                // POP DE: Pop register DE from the stack
                stack_pop16(d, e);
                break;
            }
            case 0xE1: {
                // POP HL: Pop register HL from the stack
                stack_pop16(h, l);
                break;
            }
            case 0xC5: {
                // PUSH BC: Push BC into the stack
                stack_push16(get_bc());
                break;
            }
            case 0xD5: {
                // PUSH DE: Push DE into the stack
                stack_push16(get_de());
                break;
            }
            case 0xE5: {
                // PUSH HL: Push HL into the stack
                stack_push16(get_hl());
                break;
            }
            case 0xF5: {
                // PUSH AF: Push AF into the stack
                stack_push16(get_af());
                break;
            }

            default:
                std::cerr << std::format("Unimplemented Opcode: 0x{:02X} at PC: 0x{:04X}\n", opcode, pc - 1);
                std::exit(EXIT_FAILURE);
        }
    }

    void CPU::execute_cb(uint8_t cb_opcode) {
        switch (cb_opcode) {
            case 0x10: {
                // RL B: Rotate bits in B, through the carry flag
                rl(b);
                break;
            }
            case 0x11: {
                // RL C: Rotate bits in C, through the carry flag
                rl(c);
                break;
            }
            case 0x12: {
                // RL D: Rotate bits in D, through the carry flag
                rl(d);
                break;
            }
            case 0x13: {
                // RL E: Rotate bits in E, through the carry flag
                rl(e);
                break;
            }
            case 0x14: {
                // RL H: Rotate bits in H, through the carry flag
                rl(h);
                break;
            }
            case 0x15: {
                // RL L: Rotate bits in L, through the carry flag
                rl(l);
                break;
            }
            case 0x7C: {
                // BIT 7, H: Test bit 7 in register H, set the zero flag if bit not set
                check_bit(h, 7);

                break;
            }
            default:
                std::cerr << std::format("Unimplemented CB Opcode: 0x{:02X} at PC: 0x{:04X}\n", cb_opcode, pc - 1);
                std::exit(EXIT_FAILURE);
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
