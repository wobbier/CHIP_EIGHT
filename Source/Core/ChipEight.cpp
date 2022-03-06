#include "ChipEight.h"
#include <fstream>
#include <vector>
#include <assert.h>
#include <iostream>
#include <string>

void ChipEight::Initialize()
{
    ProgramCounter = 0x200;
    OpCode = 0;
    RegisterIndex = 0;
    StackPointer = 0;

    memset(&Data[0], 0, kMemorySize);
    memset(&GfxBuffer[0], 0, (64 * 32));
    memset(&Registers[0], 0, 16);

    for (int i = 0; i < 80; ++i)
    {
        Data[i] = Font[i];
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
        {
            switch (OpCode & 0x000F)
            {
                case 0x0000:
                    // Clear Screen
                    ProgramCounter += 2;
                    break;
                case 0x000E:
                    // Subroutine return
                    break;
                default:
                    break;
            }
            break;
        }

        case 0x1000: // Jump
        {
            ProgramCounter = (OpCode & 0x0FFF);
            break;
        }

        case 0x3000: // Skip next instruction if Vx = kk.
        {
            unsigned char index = (OpCode & 0x0F00) >> 8;

            if (Registers[index] == (OpCode & 0x00FF))
            {
                ProgramCounter += 2;
            }

            ProgramCounter += 2;

            break;
        }

        case 0x4000: // Skip next instruction if Vx != kk.
        {
            unsigned char index = (OpCode & 0x0F00) >> 8;

            if (Registers[index] != (OpCode & 0x00FF))
            {
                ProgramCounter += 2;
            }

            ProgramCounter += 2;

            break;
        }
            
        case 0xA000: //Set I = nnn. - The value of register I is set to nnn.
        {
            RegisterIndex = OpCode & 0x0FFF;
            ProgramCounter += 2;
            break;
        }

        case 0x6000: // Set Register - 6xkk - Vx = kk.
        {
            unsigned char index = (OpCode & 0x0F00)>>8;
            Registers[index] = OpCode & 0x00FF;

            ProgramCounter += 2;
            break;
        }

        case 0x7000: // 7xkk - Set Vx = Vx + kk.
        {
            unsigned char index = (OpCode & 0x0F00) >> 8;
            Registers[index] += OpCode & 0x00FF;

            ProgramCounter += 2;
            break;
        }

        case 0x8000:
        {
            unsigned char x = Registers[(OpCode & 0x0F00) >> 8];
            unsigned char y = Registers[(OpCode & 0x00F0) >> 4];

            switch (OpCode & 0x000F)
            {
                case 0x0002:
                {
                    Registers[(OpCode & 0x0F00) >> 8] = (x & y);
                    ProgramCounter += 2;
                    break;
                }

                default:
                {
                    std::cout << "Unknown OpCode: " << std::to_string(OpCode);
                    break;
                }
            }
            break;
        }

        case 0xD000: // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        {
            unsigned char x = Registers[(OpCode & 0x0F00) >> 8];
            unsigned char y = Registers[(OpCode & 0x00F0) >> 4];
            unsigned char n = (OpCode & 0x000F);
            unsigned short pixel = 0;

            Registers[0xF] = 0;
            for (int yPos = 0; yPos < n; yPos++)
            {
                pixel = Data[RegisterIndex + yPos];
                for (int xPos = 0; xPos < 8; xPos++)
                {
                    if ((pixel & (0x80 >> xPos)) != 0)
                    {
                        const unsigned short index = (x + xPos + ((y + yPos) * 64));
                        if (GfxBuffer[index] == 1)
                        {
                            Registers[0xF] = 1;
                        }

                        GfxBuffer[index] ^= 1;
                    }
                }
            }
            ProgramCounter += 2;
            break;
        }
        default:
            std::cout << "Unknown OpCode: " << std::to_string(OpCode);
            break;
    }
}
