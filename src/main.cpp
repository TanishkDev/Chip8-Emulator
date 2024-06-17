#include <chrono>
#include <iostream>
#include "platform.hpp"
#include "chip8.hpp"

int main (int argc, char *argv[]) {

    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }
    
    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    platform emulator("Chip8", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
                      VIDEO_WIDTH, VIDEO_HEIGHT);

    chip8 Chip8;

    Chip8.loadROM(romFilename);

    int videoPitch = sizeof(Chip8.video[0])*VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    
    while(!quit){
        quit = emulator.processInput(Chip8.keypad);
        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime-lastCycleTime).count();
        if(dt>cycleDelay){
            lastCycleTime = currentTime;
            Chip8.Cycle();
            emulator.update(Chip8.video, videoPitch);
        }
    }
    return 0;
}
