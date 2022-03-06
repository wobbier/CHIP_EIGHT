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
    // uncomment when input goes in
    //memset(&Key[0], 0, 16);
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

    ProgramCounter += 2;

    unsigned short x = (OpCode & 0x0F00) >> 8;
    unsigned short y = (OpCode & 0x00F0) >> 4;

    switch (OpCode & 0xF000)
    {
        case 0x0000:
        {
            switch (OpCode & 0x000F)
            {
                case 0x0000:
                    // Clear Screen
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
            if (Registers[x] == (OpCode & 0x00FF))
            {
                ProgramCounter += 2;
            }

            break;
        }

        case 0x4000: // Skip next instruction if Vx != kk.
        {
            if (Registers[x] != (OpCode & 0x00FF))
            {
                ProgramCounter += 2;
            }

            break;
        }

        case 0x6000: // Set Register - 6xkk - Vx = kk.
        {
            Registers[x] = OpCode & 0x00FF;
            break;
        }

        case 0x7000: // 7xkk - Set Vx = Vx + kk.
        {
            Registers[x] += OpCode & 0x00FF;
            break;
        }

        case 0x8000:
        {
            switch (OpCode & 0x000F)
            {
                case 0x0002:
                {
                    Registers[x] &= Registers[y];
                    break;
                }

                case 0x0004: // Set Vx = Vx + Vy, set VF = carry.
                {
                    unsigned short val = Registers[x] + Registers[y];
                    Registers[0xF] = 0;
                    if (val > 255)
                    {
                        Registers[0xF] = 1;
                        val -= 256;
                    }

                    Registers[x] = val;

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

        case 0xA000: //Set I = nnn. - The value of register I is set to nnn.
        {
            RegisterIndex = OpCode & 0x0FFF;
            break;
        }

        case 0xC000:
        {
            unsigned char n = (OpCode & 0x000F);

            Registers[x] = floor((rand() % 0xFF) & n);

            break;
        }

        case 0xD000: // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
        {
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
                        const unsigned short index = (Registers[x] + xPos + ((Registers[y] + yPos) * 64));
                        if (GfxBuffer[index] == 1)
                        {
                            Registers[0xF] = 1;
                        }

                        GfxBuffer[index] ^= 1;
                    }
                }
            }
            break;
        }

        case 0xE000:
        {
            switch (OpCode & 0x00FF)
            {
                case 0x009E:
                {
                    if (Key[Registers[x]] != 0)
                    {
                        ProgramCounter += 2;
                    }
                    break;
                }
                case 0x00A1:
                {
                    unsigned char key = Registers[x];

                    if (Key[key] == 0)
                    {
                        ProgramCounter += 2;
                    }
                    break;
                }
                default:
                    std::cout << "Unknown OpCode: " << std::to_string(OpCode);
                    break;
            }
            break;
        }

        case 0xF000:
        {
            switch (OpCode & 0x00FF)
            {
                case 0x000A: // Wait for a key press, store the value of the key in Vx.
                {
                    bool keyPress = false;
                    for (int i = 0; i < 16; ++i)
                    {
                        if (Key[i] != 0)
                        {
                            Registers[x] = i;
                            keyPress = true;
                        }
                    }
                    if (!keyPress)
                    {
                        ProgramCounter -= 2;
                    }
                    break;
                }

                case 0x0018:
                {
                    SoundTimer = Registers[x];
                    break;
                }

                case 0x0029: // Set I = location of sprite for digit Vx.
                {
                    RegisterIndex = Registers[x] * 5;
                    break;
                }

                case 0x0033: // Store BCD representation of Vx in memory locations I, I+1, and I+2.
                {
                    unsigned char xVal = Registers[x];

                    for (int i = 3; i > 0; --i)
                    {
                        Data[RegisterIndex + i - 1] = xVal % 10;
                        xVal /= 10;
                    }

                    break;
                }

                case 0x0065: // Read registers V0 through Vx from memory starting at location I.
                {
                    for (int i = 0; i <= x; ++i)
                    {
                        Registers[i] = Data[RegisterIndex + i];
                    }
                    break;
                }

                default:
                    std::cout << "Unknown OpCode: " << std::to_string(OpCode);
                    break;
            }
            break;
        }

        default:
            std::cout << "Unknown OpCode: " << std::to_string(OpCode);
            break;
    }

    if (SoundTimer > 0)
    {
        SoundTimer--;
    }
}
