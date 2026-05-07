#include <iostream>
#include <SDL3/SDL.h>

#include "core/Bus.h"
#include "core/CPU.h"

constexpr int GAMEBOY_DISPLAY_WIDTH = 160;
constexpr int GAMEBOY_DISPLAY_HEIGHT = 144;

constexpr int SCALE = 4;
constexpr int WINDOW_WIDTH = GAMEBOY_DISPLAY_WIDTH * SCALE;
constexpr int WINDOW_HEIGHT = GAMEBOY_DISPLAY_HEIGHT * SCALE;

// 0 = White, 1 = Light Gray, 2 = Dark Gray, 3 = Black
constexpr uint32_t PALETTE[4] = {
    0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000
};

// VRAM Viewer Helper Function
void render_vram_viewer(Core::Bus* bus, uint32_t* pixel_buffer) {
    for (int tile = 0; tile < 256; tile++) {
        for (int row = 0; row < 8; row++) {
            uint16_t address = 0x8000 + (tile * 16) + (row * 2);
            uint8_t byte1 = bus->read(address);
            uint8_t byte2 = bus->read(address + 1);

            for (int col = 0; col < 8; col++) {
                int bit_index = 7 - col;
                uint8_t lower_bit = (byte1 >> bit_index) & 1;
                uint8_t upper_bit = (byte2 >> bit_index) & 1;
                uint8_t color_id = (upper_bit << 1) | lower_bit;

                int pixel_x = (tile % 16) * 8 + col;
                int pixel_y = (tile / 16) * 8 + row;

                pixel_buffer[pixel_y * 128 + pixel_x] = PALETTE[color_id];
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL's Video Subsystem
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create the gBoy window
    SDL_Window* window = SDL_CreateWindow(
            "gBoy",
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            0
    );

    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    std::cout << "SDL3 successfully initialized. Window created!" << std::endl;

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "Renderer failed! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 128, 128);
    if (texture == nullptr) {
        std::cerr << "Texture failed! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    uint32_t pixel_buffer[128 * 128];

    // --- INITIALIZE GAME BOY HARDWARE ---
    Core::Bus bus;
    Core::CPU cpu(&bus);

    if (!bus.load_boot_rom("/Users/sebas/Development/gBoy/files/boot/dmg_boot.bin"))
    // if (!bus.load_boot_rom(R"(D:\Development\gBoy\files\boot\dmg_boot.bin)"))
    {
        SDL_Quit();
        return 1;
    }

    if (!bus.load_cartridge(R"(/Users/sebas/Development/gBoy/files/roms/tetris.gb)")) {
        SDL_Quit();
        return 1;
    }

    bool isRunning = true;
    bool bootRomFinished = false; // Flag to track when we switch to rendering
    SDL_Event event;

    // Will keep running until the window is closed
    while (isRunning) {
        // Poll for events from the OS
        while (SDL_PollEvent(&event)) {
            // If the user clicked the close button, break the loop
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }

            if (!bootRomFinished) {
                for (int i = 0; i < 50000; i++) {
                    cpu.tick();

                    uint16_t current_pc = cpu.get_pc();
                    // Stop the CPU before it crashes into empty cartridge memory
                    if (current_pc == 0x0100) {
                        std::cout << std::format("Boot ROM finished! current_pc: 0x{:04X}.  Rendering VRAM...", current_pc) << std::endl;
                        bootRomFinished = true;
                        break;
                    }
                }
            }
            // 3. Rendering Phase
            else {
                // Decode the VRAM into pixel buffer
                render_vram_viewer(&bus, pixel_buffer);

                // Send the pixels to the GPU
                SDL_UpdateTexture(texture, nullptr, pixel_buffer, 128 * sizeof(uint32_t));

                // Clear screen, draw texture, and present it
                SDL_RenderClear(renderer);
                SDL_RenderTexture(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);

                // Add a small 16ms delay (roughly 60 FPS)
                SDL_Delay(16);
            }
        }
    }

    // Clean up and exit
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}