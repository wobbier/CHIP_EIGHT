#include "ChipEight.h"
#include <fstream>
#include <vector>
#include <assert.h>
#include <iostream>

void ChipEight::Initialize()
{
    ProgramCounter = 0x200;
    OpCode = 0;
    RegisterIndex = 0;
    StackPointer = 0;

    for (int i = 0; i < kMemorySize; ++i)
    {
        Data[i] = 0;
    }

    for (int i = 0; i < 80; ++i)
    {
        Data[i] = Font[i];
    }

    for (int i = 0; i < (64 *32); ++i)
    {
        GfxBuffer[i] = 1;
    }

    LoadProgram("Assets/Breakout.ch8");
}

bool ChipEight::LoadProgram(const char* inPath)
{
    std::ifstream source(inPath, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(source), {});

    assert(buffer.size() != 0 && "Failed to load program.");

    for (int i = 0; i < buffer.size(); ++i)
    {
        Data[512 + i] = buffer[i];
    }

    return true;
}

void ChipEight::EmulateCycle()
{
    OpCode = Data[ProgramCounter] << 8 | Data[ProgramCounter + 1];

    switch (OpCode & 0xF000)
    {
        case 0x0000:
            // Clear Screen
            std::cout << "Clear Screen";
            break;
        case 0xA000:
            RegisterIndex = OpCode & 0x0FFF;
            ProgramCounter += 2;
            break;
        default:
            break;
    }
}
