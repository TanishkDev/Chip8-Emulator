#include "chip8.hpp"

chip8::chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    randByte =  std::uniform_int_distribution<uint8_t>(0, 255U);

    table[0x2] = &chip8::OP_2nnn;
    table[0x3] = &chip8::OP_3xkk;
    table[0x4] = &chip8::OP_4xkk;
    table[0x5] = &chip8::OP_5xy0;
    table[0x6] = &chip8::OP_6xkk;
    table[0x7] = &chip8::OP_7xkk;
    table[0x8] = &chip8::Table8;
    table[0x9] = &chip8::OP_9xy0;
    table[0xA] = &chip8::OP_Annn;
    table[0xB] = &chip8::OP_Bnnn;
    table[0xC] = &chip8::OP_Cxkk;
    table[0xD] = &chip8::OP_Dxyn;
    table[0xE] = &chip8::TableE;
    table[0xF] = &chip8::TableF;

    table0[0x0] = &chip8::OP_00E0;
    table0[0xE] = &chip8::OP_00EE;

    table8[0x0] = &chip8::OP_8xy0;
    table8[0x1] = &chip8::OP_8xy1;
    table8[0x2] = &chip8::OP_8xy2;
    table8[0x3] = &chip8::OP_8xy3;
    table8[0x4] = &chip8::OP_8xy4;
    table8[0x5] = &chip8::OP_8xy5;
    table8[0x6] = &chip8::OP_8xy6;
    table8[0x7] = &chip8::OP_8xy7;
    table8[0xE] = &chip8::OP_8xyE;

    tableE[0x1] = &chip8::OP_ExA1;
    tableE[0xE] = &chip8::OP_Ex9E;

    tableF[0x07] = &chip8::OP_Fx07;    
    tableF[0x0A] = &chip8::OP_Fx0A;    
    tableF[0x15] = &chip8::OP_Fx15;    
    tableF[0x18] = &chip8::OP_Fx18;    
    tableF[0x1E] = &chip8::OP_Fx1E;    
    tableF[0x29] = &chip8::OP_Fx29;    
    tableF[0x33] = &chip8::OP_Fx33;    
    tableF[0x55] = &chip8::OP_Fx55;    
    tableF[0x65] = &chip8::OP_Fx65;    
}


void chip8::loadROM(char const* filename) 
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open())
{
        std::streampos size = file.tellg(); // The position at the end of the file is it's size

        char* buffer = new char[size]; // Create a buffer to store the contents of rom

        file.seekg(0, std::ios::beg); // Go to the beginning of the file
        file.read(buffer, size); // Read the file from the beginning in the buffer

        file.close();

        //Load the contents of the ROM to memory
        for (long i = 0; i < size; ++i)
            memory[START_ADDRESS + i] = buffer[i];

        delete[] buffer; // Free up buffer space
    }
    
}


void chip8::OP_NULL(){}

void chip8::OP_00E0()
{
    memset(video, 0, sizeof(video));
}

void chip8::OP_00EE()
{
    pc = stack[--sp];
}

void chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void chip8::OP_2nnn()
{
    stack[sp++] = pc;

    uint16_t address = opcode & 0x0FFFu;
    pc = address;
    
}

void chip8::OP_3xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] == kk) // if Register Vx == Byte, skip the next instruction
    {
        pc += 2;
    }
}

void chip8::OP_4xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] != kk) // if Register Vx != Byte, skip the next instruction
    {
        pc += 2;
    }
}

void chip8::OP_5xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] == registers[y]) // if Register Vx == Register Vy, skip the next instruction
    {
        pc += 2;
    }
}

void chip8::OP_6xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    registers[x] = kk;
}

void chip8::OP_7xkk()
{
    uint8_t kk = opcode & 0x00FFu; // Byte
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    registers[x] += kk ;
}

void chip8::OP_8xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[x] = registers[y];
}

void chip8::OP_8xy1()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[x] |= registers[y];
}

void chip8::OP_8xy2()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[x] &= registers[y];
}

void chip8::OP_8xy3()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[x] ^= registers[y];
}

void chip8::OP_8xy4()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    uint16_t sum = registers[x] + registers[y]; // Vx + Vy

    if (sum > 255U)
        registers[0xF] = 1; // if overflow, then Vf = 1 (Carry)
    else
        registers[0xF] = 0;        

    registers[x] = sum & 0xFFu;
}

void chip8::OP_8xy5()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] > registers[y])
        registers[0xF] = 1; // if Vx > Vy, set Vf to 1 (not borrow)
    else
        registers[0xF] = 0;        

    registers[x] -= registers[y];
}

void chip8::OP_8xy6()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[0xF] = (registers[x] & 0x1u); // Store Least Significant bit of Vx in Vf

    registers[x] >>= 1;
}

void chip8::OP_8xy7()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[y] > registers[x])
        registers[0xF] = 1; // if Vy > Vx, set Vf to 1 (not borrow)
    else
        registers[0xF] = 0;        

    registers[x] = registers[y] - registers[x];
}

void chip8::OP_8xyE()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    registers[0xF] = (registers[x] & 0x80u); // Store Most Significant bit of Vx in Vf

    registers[x] <<= 1;
}

void chip8::OP_9xy0()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy    
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    if (registers[x] != registers[y]) // if Register Vx != Vy, skip the next instruction
    {
        pc += 2;
    }
}

void chip8::OP_Annn()
{
  uint16_t address = opcode & 0x0FFFu;
  index = address; 
}

void chip8::OP_Bnnn()
{
  uint16_t address = opcode & 0x0FFFu;
  pc = registers[0] + address;  
}

void chip8::OP_Cxkk()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Vx
    uint8_t byte = opcode & 0x00FFu;
    registers[x] = byte & randByte(randGen);

}

void chip8::OP_Dxyn()
{
    uint8_t y = (opcode & 0x00F0u) >> 4u; // Register Vy
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[x] % VIDEO_WIDTH;
    uint8_t yPos = registers[y] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) 
        {
            uint8_t spritePixel = spriteByte & (0x80u >> col);

            uint32_t* screenPixel = &video[((yPos + row) * VIDEO_WIDTH + (xPos + col))%(VIDEO_HEIGHT * VIDEO_WIDTH)];

            if (spritePixel)
            {
                if(*screenPixel == 0xFFFFFFFF) 
                {
                    registers[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }  

}

void chip8::OP_Ex9E()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t key = registers[x];

    if(keypad[key])
        pc += 2; // Skip the next instruction if key in Vx is pressed
}

void chip8::OP_ExA1()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t key = registers[x];

    if(!keypad[key])
        pc += 2; // Skip the next instruction if key in Vx is not pressed    
}

void chip8::OP_Fx07()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    registers[x] = delayTimer;
}

void chip8::OP_Fx0A()
{
   uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx


   if (keypad[0])
   {
    registers[x] = 0;
   }
   else if (keypad[1])
   {
       registers[x] = 1;
   } 
      else if (keypad[2])
   {
       registers[x] = 2;
   } 
   else if (keypad[3])
   {
       registers[x] = 3;
   } 
   else if (keypad[4])
   {
       registers[x] = 4;
   } 
   else if (keypad[5])
   {
       registers[x] = 5;
   } 
   else if (keypad[6])
   {
       registers[x] = 6;
   } 
   else if (keypad[7])
   {
       registers[x] = 7;
   } 
   else if (keypad[8])
   {
       registers[x] = 8;
   } 
   else if (keypad[9])
   {
       registers[x] = 9;
   } 
   else if (keypad[10])
   {
       registers[x] = 10;
   } 
   else if (keypad[11])
   {
       registers[x] = 11;
   } 
   else if (keypad[12])
   {
       registers[x] = 12;
   } 
   else if (keypad[13])
   {
       registers[x] = 13;
   } 
   else if (keypad[14])
   {
       registers[x] = 14;
   } 
   else if (keypad[15])
   {
       registers[x] = 15;
   } 
    else
    {
        pc -= 2;
    }
}

void chip8::OP_Fx15()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    delayTimer = registers[x];
}

void chip8::OP_Fx18()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    soundTimer = registers[x];
}

void chip8::OP_Fx1E()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    index += registers[x];
}

void chip8::OP_Fx29()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t digit = registers[x];
    index = FONTSET_START_ADDRESS + (5 * digit); // Because each digit occupies 5 Bytes
}

void chip8::OP_Fx33()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx
    uint8_t value = registers[x];

    memory[index + 2] = value % 10; // Ones place
    value /= 10;

    memory[index + 1] = value % 10; // Tens place
    value /= 10;

    memory[index] = value % 10; // Hundreds place
}

void chip8::OP_Fx55()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    for (uint8_t i = 0; i <= x; ++i)
        memory[index + i] = registers[i];
}

void chip8::OP_Fx65()
{
    uint8_t x = (opcode & 0x0F00u) >> 8u; // Register Vx

    for (uint8_t i = 0; i <= x; ++i)
        registers[i] = memory[index + i];
} 

void chip8::Cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0)
        --soundTimer;
}

