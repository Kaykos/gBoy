#include <iostream>
#include <SDL3/SDL.h>

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
    SDL_Delay(3000);

    // Clean up and exit
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}