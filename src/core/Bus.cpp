//
// Created by bobad on 4/16/2026.
//

#include "Bus.h"

#include <fstream>
#include <iostream>

namespace Core {
    Bus::Bus() {
        // Initialize memory
        memory.fill(0);
    }

    uint8_t Bus::read(uint16_t address) {
        // While the bot ROM is active, any reads from 0x0000 to 0x00FF are
        // intercepted and read from the boot ROM, instead of the cartridge
        if (boot_rom_active && address <= 0x00FF)
        {
            return boot_rom[address];
        }

        return memory[address];
    }

    void Bus::write(uint16_t address, uint8_t value) {
        if (address == 0xFF50)
        {
            boot_rom_active = false;
        }

        memory[address] = value;
    }

    bool Bus::load_boot_rom(const std::string& file_path) {
        std::ifstream file;

        // Open file at the end, to determine file size
        file.open(file_path, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            std::cerr << "Failed to open Boot ROM file: " << file_path << std::endl;
            return false;
        }

        // File size
        const std::streamsize size = file.tellg();
        if (size != 256)
        {
            std::cerr << "Warning: Boot ROM is not exactly 256 bytes! It is " << size << " bytes" << std::endl;
        }

        // Rewind to beginning
        file.seekg(0, std::ios::beg);

        // Read file content into boot ROM
        if (file.read(reinterpret_cast<char*>(boot_rom.data()), size))
        {
            std::cout << "Successfully loaded Boot ROM! (" << size << " bytes)" << std::endl;
            return true;
        }

        return false;
    }
}
