#include <thread>
#include <chrono>
#include <bitset>

#include "../include/chip8.hxx"

const std::unordered_map<uint8_t, uint8_t> Chip8::keymap {
        {30, 0x1}, {31, 0x2}, {32, 0x3}, {33, 0xC}, // 1 2 3 4
        {20, 0x4}, {26, 0x5}, {8,  0x6}, {21, 0xD}, // Q W E R
        {4,  0x7}, {22, 0x8}, {7,  0x9}, {9,  0xE}, // A S D F
        {29, 0xA}, {27, 0x0}, {6,  0xB}, {25, 0xF}  // Z X C V
};

Chip8::Chip8() : index_register{}, reg{}, memory{}, stack{}, delay_timer{}, sound_timer{}, timer_count{}, framebuffer{}, fb_modified{true}, pc{}, keyboard{}, is_beeping{} {
    for(size_t i = 0; i < font.size(); i++) {
        memory[CH8_FONT_ADDR+i] = font[i];
    }
}

void Chip8::load(const std::vector<uint8_t>& bytecode) {
    pc = CH8_PROGRAM_ADDR;

    for(size_t i = 0; i < bytecode.size() && i < CH8_PROGRAM_SIZE; i++) {
        memory[pc+i] = bytecode[i];
    }
}

void Chip8::clear_fb() {
    for(size_t addr = 0; addr < framebuffer.size(); addr++) {
        framebuffer[addr] = CH8_SCREEN_COLOR_OFF;
    }
}

void* Chip8::get_fb() {
    if(!fb_modified) return nullptr;

    fb_modified = false;
    return &framebuffer.__elems_;
}

void Chip8::set_key(const uint8_t scancode, const bool status) {
    auto key = keymap.find(scancode);
    if(key != keymap.end()) {
        keyboard[key->second] = status;
    }
}

inline uint16_t Chip8::fetch_instruction() {
    return (memory[pc] << 8) | memory[pc+1];
}

inline void Chip8::increment_pc() {
    pc += CH8_PC_STEP;
}

void Chip8::handle_timers() {
    if(timer_count++ == CH8_CPU_FREQUENCY/CH8_TIMER_FREQUENCY) {    
        if(sound_timer > 0) is_beeping = --sound_timer > 0;
        if(delay_timer > 0) delay_timer--;
        timer_count = 0; 
    }
}

bool Chip8::step() {
    handle_timers();

    uint16_t ins = fetch_instruction();
    increment_pc();
    decode_instruction(ins);
    
    return pc < CH8_MEMORY_SIZE;
}

void Chip8::draw_sprite(const uint8_t x, const uint8_t y, const uint8_t height, const uint16_t sprite_addr) {
    for(auto i = 0; i < height; i++) {
        uint8_t row = (y+i) % CH8_SCREEN_HEIGHT;
        // Iterate over individual bits in the sprite, starting from LE
        std::bitset<8> sprite = memory[sprite_addr+i];
        for(auto j = 0; j < 8; j++) {
            if(sprite[7-j]) {
                uint8_t column = (x+j) % CH8_SCREEN_WIDTH;
                uint16_t position = column + row * CH8_SCREEN_WIDTH;

                // Flip the pixel's color, set the flag register if the pixel has been deactivated.
                if(framebuffer[position] == CH8_SCREEN_COLOR_ON) {
                    framebuffer[position] = CH8_SCREEN_COLOR_OFF;
                    reg[CH8_FLAG] = true;
                } else {
                    framebuffer[position] = CH8_SCREEN_COLOR_ON;
                }
            }
        }
     }
    fb_modified = true;
}

void Chip8::decode_instruction(const uint16_t ins) {
    // 0x1000 - Primary instruction type
    uint8_t type = ins >> 12;

    // 0x0100 - First register[x]
    uint8_t x = (ins >> 8) & 0xF;
    // 0x0010 - Second register[y]
    uint8_t y = (ins >> 4) & 0xF;

    // 0x0001 - 1 byte operand OR secondary instruction type
    uint8_t  nibble = ins & 0xF;
    // 0x0110 - 2 byte operand OR secondary instruction type
    uint8_t  byte   = ins & 0xFF;
    // 0x0111 - 3 byte operand
    uint16_t addr   = ins & 0xFFF;
   
    switch(type) {
        case 0x0:
            switch(byte) {
                case 0xE0:
                    clear_fb(); break;
                case 0xEE:
                    pc = stack.top();
                    stack.pop();
                    break;
            }
            break;
        case 0x2:
            stack.push(pc); [[fallthrough]];
        case 0x1:
            pc = addr; break;
        case 0x3:
            if(reg[x] == byte) increment_pc(); break;
        case 0x4:
            if(reg[x] != byte) increment_pc(); break;
        case 0x5:
            if(reg[x] == reg[y]) increment_pc();
            break;
        case 0x6:
            reg[x] = byte; break;
        case 0x7:
            reg[x] += byte; break;
        case 0x8:
            switch(nibble) {
                case 0x0:
                    reg[x] = reg[y]; break;
                case 0x1:
                    reg[x] |= reg[y]; break;
                case 0x2: 
                    reg[x] &= reg[y]; break;
                case 0x3:
                    reg[x] ^= reg[y]; break;
                case 0x4: {
                    uint16_t result = reg[x] + reg[y];
                    reg[CH8_FLAG] = result > UINT8_MAX;
                    reg[x] = result;
                    break;
                }
                case 0x5: {
                    bool flag = reg[x] >= reg[y];
                    reg[x] -= reg[y];
                    reg[CH8_FLAG] = flag;
                    break;
                }
                case 0x7: {
                    bool flag =  reg[y] >= reg[x];
                    reg[x] = reg[y] - reg[x];
                    reg[CH8_FLAG] = flag;
                    break;
                }
                case 0x6: {
                    bool flag = reg[y] & 1;
                    reg[x] = reg[y] >> 1;
                    reg[CH8_FLAG] = flag;
                    break;
                }
                case 0xE: {
                    bool flag = reg[x] >> 7;
                    reg[x] = reg[x] << 1;
                    reg[CH8_FLAG] = flag;
                    break;
                }
            }
            break;
        case 0x9:
            if(reg[x] != reg[y]) increment_pc(); break;
        case 0xA:
            index_register = addr; break;
        case 0xB:
            pc = addr + reg[0]; break;
        case 0xC:
            reg[x] = rand() & byte; break;
        case 0xD:
            draw_sprite(reg[x], reg[y], nibble, index_register); break;
        case 0xE:
            switch(byte) {
                case 0x9E:
                    if(keyboard[reg[x]]) increment_pc(); break;
                case 0xA1:
                    if(!keyboard[reg[x]]) increment_pc(); break;
            }
            break;
        case 0xF:
            switch(byte) {
                case 0x07:
                    reg[x] = delay_timer; break;
                case 0x0A: {
                    bool keep_waiting = true;
                    for(size_t i = 0; i < keyboard.size(); i++) {
                        if(keyboard[i]) {
                            keep_waiting = false;
                            reg[x] = i;
                            break;
                        }
                    }
                    if(keep_waiting) pc -= CH8_PC_STEP;
                    break;
                }
                case 0x15: 
                    delay_timer = reg[x]; break;
                case 0x18:
                    sound_timer = reg[x]; break;
                case 0x1E:
                    index_register += reg[x]; break;
                case 0x29: {
                    auto i = reg[x] & 0xF;
                    index_register = memory[CH8_FONT_ADDR+i];
                    break;
                }
                case 0x33: {
                    uint8_t num = reg[x];
                    for(auto i = 2; i >= 0; i--) {
                        memory[index_register+i] = num % 10;
                        num = num / 10;
                    }
                    break;
                }
                case 0x55:
                    for(auto i = 0; i <= x; i++) {
                        memory[index_register+i] = reg[i];
                    }
                    break;
                case 0x65:
                    for(auto i = 0; i <= x; i++) {
                        reg[i] = memory[index_register+i];
                    }
                    break;
            }
        break;
    }
}

