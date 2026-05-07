//
// Created by bobad on 4/16/2026.
//

#include "Bus.h"

#include <fstream>
#include <iostream>
#include <cstdint>

namespace Core {
    Bus::Bus() {
        // Initialize memory
        memory.fill(0);
    }

    uint8_t Bus::read(const uint16_t address) const {
        // TODO Temporary
        // PPU Spoof (Temporary hack to pass the VBlank wait loop)
        if (address == 0xFF44) {
            return 0x90;
        }

        // While the bot ROM is active, any reads from 0x0000 to 0x00FF are
        // intercepted and read from the boot ROM, instead of the cartridge
        if (boot_rom_active && address <= 0x00FF)
        {
            return boot_rom[address];
        }

        // Cartridge ROM (0x0000 - 0x7FFF)
        if (address < 0x8000) {
            if (address < cartridge_memory.size()) {
                return cartridge_memory[address];
            }
            return 0xFF; // Unmapped cartridge space
        }

        // Video RAM - VRAM (0x8000 - 0x9FFF)
        if (address >= 0x8000 && address <= 0x9FFF) {
            return vram[address - 0x8000];
        }

        // Cartridge External RAM (0xA000 - 0xBFFF)
        // Used for save files
        if (address >= 0xA000 && address <= 0xBFFF) {
            return 0xFF;
        }

        // Work RAM - WRAM (0xC000 - 0xDFFF)
        if (address >= 0xC000 && address <= 0xDFFF) {
            return wram[address - 0xC000];
        }

        // Echo RAM (0xE000 - 0xFDFF)
        // Hardware quirk: Mirrors WRAM exactly.
        if (address >= 0xE000 && address <= 0xFDFF) {
            return wram[address - 0xE000];
        }

        // OAM / Sprite Attribute Table (0xFE00 - 0xFE9F)
        // TODO. Pending when doing the PPU
        if (address >= 0xFE00 && address <= 0xFE9F) {
            return 0xFF; // Placeholder
        }

        // Unusable Memory (0xFEA0 - 0xFEFF)
        if (address >= 0xFEA0 && address <= 0xFEFF) {
            return 0xFF;
        }

        // Hardware I/O Registers (0xFF00 - 0xFF7F)
        // Joypad, Timers, Audio, PPU registers live here.
        if (address >= 0xFF00 && address <= 0xFF7F) {
            return 0xFF; // Return 0xFF for unhandled I/O right now
        }

        // High RAM - HRAM (0xFF80 - 0xFFFE)
        if (address >= 0xFF80 && address <= 0xFFFE) {
            return hram[address - 0xFF80];
        }

        // Interrupt Enable Register (0xFFFF)
        if (address == 0xFFFF) {
            return 0xFF; // Placeholder
        }

        return 0xFF; // Default fallback
    }

    void Bus::write(uint16_t address, uint8_t value) {
        if (address == 0xFF50) {
            std::cout << "Finishing boot ROM" << std::endl;
            boot_rom_active = false;
            return;
        }

        // Video RAM - VRAM (0x8000 - 0x9FFF)
        if (address >= 0x8000 && address <= 0x9FFF) {
            vram[address - 0x8000] = value;
            return;
        }

        // Cartridge External RAM (0xA000 - 0xBFFF)
        // Used for save files
        if (address >= 0xA000 && address <= 0xBFFF) {
            //
        }

        // Work RAM - WRAM (0xC000 - 0xDFFF)
        if (address >= 0xC000 && address <= 0xDFFF) {
            wram[address - 0xC000] = value;
            return;
        }

        // Echo RAM (0xE000 - 0xFDFF)
        // Hardware quirk: Mirrors WRAM exactly.
        if (address >= 0xE000 && address <= 0xFDFF) {
            wram[address - 0xE000] = value;
            return;
        }

        // OAM / Sprite Attribute Table (0xFE00 - 0xFE9F)
        // TODO. Pending when doing the PPU
        if (address >= 0xFE00 && address <= 0xFE9F) {
            //
        }

        // Hardware I/O Registers (0xFF00 - 0xFF7F)
        // Joypad, Timers, Audio, PPU registers live here.
        if (address >= 0xFF00 && address <= 0xFF7F) {
            //
        }

        // High RAM - HRAM (0xFF80 - 0xFFFE)
        if (address >= 0xFF80 && address <= 0xFFFE) {
            hram[address - 0xFF80] = value;
            return;
        }

        // Interrupt Enable Register (0xFFFF)
        if (address == 0xFFFF) {
            //
        }
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

    bool Bus::load_cartridge(const std::string& file_path) {
        std::ifstream file(file_path, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Failed to load cartridge: " << file_path << std::endl;
            return false;
        }

        // Copy entire ROM into cartridge memory vector
        cartridge_memory.assign(std::istreambuf_iterator(file), std::istreambuf_iterator<char>());
        std::cout << "Cartridge loaded successfully! Size: " << cartridge_memory.size() << " bytes" << std::endl;
        return true;
    }
}
