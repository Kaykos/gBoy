//
// Created by bobad on 4/16/2026.
//

#include "Bus.h"

namespace Core {
    Bus::Bus() {
        // Initialize memory
        memory.fill(0);
    }

    uint8_t Bus::read(uint16_t address) {
        return memory[address];
    }

    void Bus::write(uint16_t address, uint8_t value) {
        memory[address] = value;
    }
}
