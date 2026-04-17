#include <iostream>
#include <SDL3/SDL.h>

#include "core/Bus.h"
#include "core/CPU.h"

constexpr int GAMEBOY_DISPLAY_WIDTH = 160;
constexpr int GAMEBOY_DISPLAY_HEIGHT = 144;

constexpr int SCALE = 4;
constexpr int WINDOW_WIDTH = GAMEBOY_DISPLAY_WIDTH * SCALE;
constexpr int WINDOW_HEIGHT = GAMEBOY_DISPLAY_HEIGHT * SCALE;

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

    // Keep the window open for 3 seconds
    std::cout << "SDL3 successfully initialized. Window created!" << std::endl;


    // --- INITIALIZE GAME BOY HARDWARE ---
    Core::Bus bus;
    Core::CPU cpu(&bus);

    // Write a dummy NOP instruction (0x00) followed by a fake opcode (0xFF) to memory for testing
    bus.write(0x0100, 0x00);
    bus.write(0x0101, 0xFF);

    bool isRunning = true;
    SDL_Event event;

    // Will keep running until the window is closed
    while (isRunning) {
        // Poll for events from the OS
        while (SDL_PollEvent(&event)) {
            // If the user clicked the close button, break the loop
            if (event.type == SDL_EVENT_QUIT) {
                isRunning = false;
            }

            cpu.tick();
        }
    }

    // Clean up and exit
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}