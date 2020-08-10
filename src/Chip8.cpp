#include "../include/Chip8.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <ratio>

using ms = std::chrono::milliseconds;
using timestamp = std::chrono::high_resolution_clock;
using float_duration_ms = std::chrono::duration<float, std::milli>;

inline uint8_t Chip8::RandomByteGenerator::randbyte() noexcept {
    return byte_distribution(rand_gen);
}

Chip8::RandomByteGenerator::RandomByteGenerator() 
    : byte_distribution(std::uniform_int_distribution<uint8_t>(0, 255)) {}

// ===================== JUMP TABLES, EACH OF THEM CONTAINS THE APPROPRIATE FUNCTION POINTER TO INSTRUCTION DECODING PROCEDURE =========================
void (Chip8::*const Chip8::global_jt[global_jumptable_size])() = {
    &Chip8::dispatch_0,
    &Chip8::inst_1nnn,
    &Chip8::inst_2nnn,
    &Chip8::inst_3xkk,
    &Chip8::inst_4xkk,
    &Chip8::inst_5xy0,
    &Chip8::inst_6xkk,
    &Chip8::inst_7xkk,
    &Chip8::dispatch_8,
    &Chip8::inst_9xy0,
    &Chip8::inst_annn,
    &Chip8::inst_bnnn,
    &Chip8::inst_cxkk,
    &Chip8::inst_dxyn,
    &Chip8::dispatch_e,
    &Chip8::dispatch_f
};

void (Chip8::*const Chip8::subtable_op_0_jt[subtable_op_0_size])() = {
    &Chip8::inst_00e0,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_00ee
};

void (Chip8::*const Chip8::subtable_op_8_jt[subtable_op_8_size])() = {
    &Chip8::inst_8xy0,
    &Chip8::inst_8xy1,
    &Chip8::inst_8xy2,
    &Chip8::inst_8xy3,
    &Chip8::inst_8xy4,
    &Chip8::inst_8xy5,
    &Chip8::inst_8xy6,
    &Chip8::inst_8xy7,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_8xye
};

void (Chip8::*const Chip8::subtable_op_e_jt[subtable_op_e_size])() = {
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_exa1,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_ex9e
};

void (Chip8::*const Chip8::subtable_op_f_jt[subtable_op_f_size])() = {
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx07,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx0a,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx15,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx18,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx1e,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx29,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx33,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx55,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::invalid_instruction_handler,
    &Chip8::inst_fx65
};

Chip8::Chip8(const std::string &path_to_rom, const std::string &path_to_sound, const uint8_t scale_factor, const std::string &title) 
    : font_sprites({
            0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
            0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
            0x90, 0x90, 0xf0, 0x10, 0x10, // 4
            0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
            0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
            0xf0, 0x10, 0x20, 0x40, 0x40, // 7
            0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
            0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
            0xf0, 0x90, 0xf0, 0x90, 0x90, // A
            0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
            0xf0, 0x80, 0x80, 0x80, 0xf0, // C
            0xe0, 0x90, 0x90, 0x90, 0xe0, // D
            0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
            0xf0, 0x80, 0xf0, 0x80, 0x80  // F
            }),
      rand_byte_gen(), // RandomByteGenerator object construction
      rom_load_addr(0x200), // ROMs always loaded at address 0x200
      gfx_obj(display_width, display_height, scale_factor, title) /* Graphics object creation */ {
    
    load_rom(path_to_rom); 
    load_sound(path_to_sound);
    initialize_vm();
}

void Chip8::load_sound(const std::string &path_to_sound) {
    // load the beep (or whatever) sound
    if (!sound_buffer.loadFromFile(path_to_sound)) {
        fprintf(stderr, "Audio file '%s' is not found\n", path_to_sound.data());
        exit(EXIT_FAILURE);
    }
    beep.setBuffer(sound_buffer);
}

void Chip8::load_rom(const std::string &path_to_rom) {
    std::ifstream rom_ifstream { path_to_rom, std::ios::binary };
    if (!rom_ifstream) {
        fprintf(stderr, "ROM '%s' is not found\n", path_to_rom.data());
        exit(EXIT_FAILURE);
    }
    rom_ifstream.seekg(0, std::ios::end);
    if (rom_ifstream.tellg() > (memory_size - rom_load_addr)) {
        fprintf(stderr, "'%s' is too large (%llu bytes)\nMaximum allowed ROM size is %d bytes\n", path_to_rom.data(), 
                                                                                                  static_cast<unsigned long long>(rom_ifstream.tellg()), 
                                                                                                  memory_size - rom_load_addr);
        exit(EXIT_FAILURE);
    }
    rom_size = rom_ifstream.tellg();
    rom_ifstream.seekg(std::ios::beg);
    rom_ifstream.read(reinterpret_cast<char*>(memory.data() + rom_load_addr), rom_size);
    rom_ifstream.close();
}

// Turn off all the pixels on the display
inline void Chip8::clear_display() noexcept {
    std::for_each(display.begin(), 
                  display.end(), 
                  [this](auto &row) { 
                      std::fill(row.begin(), row.end(), 0);
                  });
}

void Chip8::initialize_vm() {
    std::memset(&reg, 0, sizeof(reg)); // reset all the registers
    std::memset(&timer, 0, sizeof(timer)); // reset all the timers
    reg.pc = rom_load_addr; // set the program counter to the beginning of the ROM code
    std::fill(memory.begin(), memory.begin() + rom_load_addr, 0); // pad the memory with zeroes up to the ROM start address
    std::fill(memory.begin() + rom_load_addr + rom_size, memory.end(), 0); // pad the memory after the ROM mapping with zeroes
    std::copy(font_sprites.begin(), font_sprites.end(), memory.begin()); // load the font sprites into the memory
    std::fill(stack.begin(), stack.end(), 0); // clear the stack
    std::fill(keypad.begin(), keypad.end(), 0); // clear the keypad, no key is pressed
    clear_display();
}

// timers are updated at 60 Hz frequency (roughly every 8th cycle with 500 Hz CPU frequency)
inline void Chip8::update_timers() noexcept {
    if (timer.delay > 0) {
        timer.delay--;
    }
    if (timer.sound > 0) {
        if (timer.sound == 1) {
            beep.play();
        }
        timer.sound--;
    }
}

// this method fetches the current instruction and decodes it 
inline void Chip8::emulate_cpu_cycle() noexcept {
    // fetch the current instruction to be emulated
    instruction = (memory[reg.pc] << 8) | memory[reg.pc + 1];
    // filter all relevant bytes and nibbles
    opcode = (instruction & 0xf000) >> 12;
    nnn = instruction & 0x0fff;
    x = (instruction & 0x0f00) >> 8;
    y = (instruction & 0x00f0) >> 4;
    n = instruction & 0x000f;
    kk = instruction & 0x00ff;
    printf("Emulated instruction : 0x%.4x at address 0x%.4x\n", instruction, reg.pc);
    // jump to the master jump table, the appropriate instruction decoding function will be called
    (this->*Chip8::global_jt[opcode])();
}

inline void Chip8::handle_key_down(sf::Event &e) noexcept {
    switch (e.key.code) {
        case sf::Keyboard::Num1:   keypad[0x1] = 1; break;
        case sf::Keyboard::Num2:   keypad[0x2] = 1; break;
        case sf::Keyboard::Num3:   keypad[0x3] = 1; break;
        case sf::Keyboard::Num4:   keypad[0xc] = 1; break;
        case sf::Keyboard::Q:      keypad[0x4] = 1; break;
        case sf::Keyboard::W:      keypad[0x5] = 1; break;
        case sf::Keyboard::E:      keypad[0x6] = 1; break;
        case sf::Keyboard::R:      keypad[0xd] = 1; break;
        case sf::Keyboard::A:      keypad[0x7] = 1; break;
        case sf::Keyboard::S:      keypad[0x8] = 1; break;
        case sf::Keyboard::D:      keypad[0x9] = 1; break;
        case sf::Keyboard::F:      keypad[0xe] = 1; break;
        case sf::Keyboard::Z:      keypad[0xa] = 1; break;
        case sf::Keyboard::X:      keypad[0x0] = 1; break;
        case sf::Keyboard::C:      keypad[0xb] = 1; break;
        case sf::Keyboard::V:      keypad[0xf] = 1; break;
        case sf::Keyboard::Escape: gfx_obj.window.close(); break;
        default: break;
    }
}

inline void Chip8::handle_key_up(sf::Event &e) noexcept {
    switch (e.key.code) {
        case sf::Keyboard::Num1:   keypad[0x1] = 0; break;
        case sf::Keyboard::Num2:   keypad[0x2] = 0; break;
        case sf::Keyboard::Num3:   keypad[0x3] = 0; break;
        case sf::Keyboard::Num4:   keypad[0xc] = 0; break;
        case sf::Keyboard::Q:      keypad[0x4] = 0; break;
        case sf::Keyboard::W:      keypad[0x5] = 0; break;
        case sf::Keyboard::E:      keypad[0x6] = 0; break;
        case sf::Keyboard::R:      keypad[0xd] = 0; break;
        case sf::Keyboard::A:      keypad[0x7] = 0; break;
        case sf::Keyboard::S:      keypad[0x8] = 0; break;
        case sf::Keyboard::D:      keypad[0x9] = 0; break;
        case sf::Keyboard::F:      keypad[0xe] = 0; break;
        case sf::Keyboard::Z:      keypad[0xa] = 0; break;
        case sf::Keyboard::X:      keypad[0x0] = 0; break;
        case sf::Keyboard::C:      keypad[0xb] = 0; break;
        case sf::Keyboard::V:      keypad[0xf] = 0; break;
        case sf::Keyboard::Escape: gfx_obj.window.close(); break;
        default: break;
    }
}

void Chip8::run() noexcept {
    sf::Event e;
    unsigned cycle_cnt {};
    while (gfx_obj.window.isOpen()) {
        while (gfx_obj.window.pollEvent(e)) {
            // handle the pressed key
            switch (e.type) {
                case sf::Event::Closed:
                    gfx_obj.window.close();
                    break;
                case sf::Event::EventType::KeyPressed:
                    handle_key_down(e);
                    break;
                case sf::Event::EventType::KeyReleased:
                    handle_key_up(e);
                    break;
                default:
                    break;
            }
        } 
        // measure the CPU cycle time
        auto start { timestamp::now() };
        emulate_cpu_cycle();
        auto end { timestamp::now() };
        cycle_cnt++;
        // timers updates happen every (CPU frequency / 60) CPU cycles, the update frequency is bounded to 60 Hz
        if (cycle_cnt == timers_clock_cycles) {
            update_timers();
            cycle_cnt = 0;
        }
        float_duration_ms inst_time_elapsed { end - start };
        // if the instruction execution time is less than 2 ms (for 500 Hz CPU frequency), sleep the (desired exec time - actual exec time)
        // It emulates the original Chip-8 frequency - 500 Hz
        if (inst_time_elapsed.count() < instruction_time) {
            std::this_thread::sleep_for(float_duration_ms(instruction_time - inst_time_elapsed.count()));
        }
    }
}

// =============================== SUBTABLE DISPATCH ROUTINES =========================================== 
inline void Chip8::dispatch_0() noexcept {
    if (n < subtable_op_0_size) {
        (this->*Chip8::subtable_op_0_jt[n])();
    } else {
        invalid_instruction_handler();
    }
}

inline void Chip8::dispatch_8() noexcept {
    if (n < subtable_op_8_size) {
        (this->*Chip8::subtable_op_8_jt[n])();
    } else {
        invalid_instruction_handler();
    }
}

inline void Chip8::dispatch_e() noexcept {
    if (n < subtable_op_e_size) {
        (this->*Chip8::subtable_op_e_jt[n])();
    } else {
        invalid_instruction_handler();
    }
}

inline void Chip8::dispatch_f() noexcept {
    if (kk < subtable_op_f_size) {
        (this->*Chip8::subtable_op_f_jt[kk])();
    } else {
        invalid_instruction_handler();
    }
}

// ==================================== OPCODE DECODING ROUTINES ======================================

// instruction : CLS
inline void Chip8::inst_00e0() noexcept {
    clear_display();
    reg.pc += 2;
}

// instruction : RET
inline void Chip8::inst_00ee() noexcept {
    if (reg.sp > 0) {
        reg.pc = stack[--reg.sp];
        reg.pc += 2; 
    } else {
        std::cerr << "Stack underflow!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// instruction : JP nnn
inline void Chip8::inst_1nnn() noexcept {
    reg.pc = nnn;
}

// instruction : CALL nnn
inline void Chip8::inst_2nnn() noexcept {
    if (reg.sp < (stack_size - 1)) {
        stack[reg.sp++] = reg.pc;
        reg.pc = nnn;
    } else {
        std::cerr << "Stack overflow!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// instruction : SE Vx, kk
inline void Chip8::inst_3xkk() noexcept {
    if (reg.V[x] == kk) {
        reg.pc += 2;
    }         
    reg.pc += 2;
}

// instruction : SNE Vx, kk
inline void Chip8::inst_4xkk() noexcept {
    if (reg.V[x] != kk) {
        reg.pc += 2;
    } 
    reg.pc += 2;
}

// instruction : SE Vx, Vy
inline void Chip8::inst_5xy0() noexcept {
    if (reg.V[x] == reg.V[y]) {
        reg.pc += 2;
    } 
    reg.pc += 2;
}

// instruction : LD Vx, kk
inline void Chip8::inst_6xkk() noexcept {
    reg.V[x] = kk;
    reg.pc += 2;
}

// instruction : ADD Vx, kk
inline void Chip8::inst_7xkk() noexcept {
    reg.V[x] += kk;
    reg.pc += 2;
}

// instruction : LD Vx, Vy
inline void Chip8::inst_8xy0() noexcept {
    reg.V[x] = reg.V[y];
    reg.pc += 2;
}

// instruction : OR Vx, Vy
inline void Chip8::inst_8xy1() noexcept {
    reg.V[x] |= reg.V[y];
    reg.pc += 2;
}

// instruction : AND Vx, Vy
inline void Chip8::inst_8xy2() noexcept {
    reg.V[x] &= reg.V[y];
    reg.pc += 2;
}

// instruction : XOR Vx, Vy
inline void Chip8::inst_8xy3() noexcept {
    reg.V[x] ^= reg.V[y];
    reg.pc += 2;
}

// instruction : ADD Vx, Vy
inline void Chip8::inst_8xy4() noexcept {
    reg.V[0xf] = (reg.V[x] + reg.V[y]) > 255 ? 1 : 0;
    reg.V[x] = (reg.V[x] + reg.V[y]) & 0x00ff; 
    reg.pc += 2;
}

// instruction : SUB Vx, Vy
inline void Chip8::inst_8xy5() noexcept {
    reg.V[0xf] = reg.V[x] < reg.V[y] ? 0 : 1;
    reg.V[x] -= reg.V[y];
    reg.pc += 2;
}

// instruction : SHR Vx {, Vy}
inline void Chip8::inst_8xy6() noexcept {
    reg.V[0xf] = reg.V[x] & 0x1u;
    reg.V[x] >>= 1;
    reg.pc += 2;
}

// instruction : SUBN Vx, Vy
inline void Chip8::inst_8xy7() noexcept {
    reg.V[0xf] = reg.V[x] > reg.V[y] ? 0 : 1;
    reg.V[x] = reg.V[y] - reg.V[x];
    reg.pc += 2;
}

// instruction : SHL Vx {, Vy}
inline void Chip8::inst_8xye() noexcept {
    reg.V[0xf] = reg.V[x] >> 7;
    reg.V[x] <<= 1;
    reg.pc += 2;
}

// instruction : SNE Vx, Vy
inline void Chip8::inst_9xy0() noexcept {
    if (reg.V[x] != reg.V[y]) {
        reg.pc += 2;
    } 
    reg.pc += 2;
}

// instruction : LD I, nnn
inline void Chip8::inst_annn() noexcept {
    reg.I = nnn;
    reg.pc += 2;
}

// instruction : JP V0, nnn
inline void Chip8::inst_bnnn() noexcept {
    reg.pc = nnn + reg.V[0];
}

// instruction : RND Vx, kk
inline void Chip8::inst_cxkk() noexcept {
    reg.V[x] = rand_byte_gen.randbyte() & kk;
    reg.pc += 2;
}

// instruction : DRW Vx, Vy, nibble 
inline void Chip8::inst_dxyn() noexcept {
    // in case of display overflow, the sprite wraps around the screen
    uint8_t coord_x = reg.V[x] % 64,
            coord_y = reg.V[y] % 32,
            sprite_height = n;
    // default state - no collision
    reg.V[0xf] = 0;
    for (uint8_t row {}; row < sprite_height; row++) {
        uint8_t px_to_draw { memory[reg.I + row] }; // pixel to draw on the screen
        for (uint8_t bit_pos {}; bit_pos < 8; bit_pos++) {
            uint8_t &curr_px { display[coord_y + row][coord_x + bit_pos] }; // current pixel on the screen (can be on or off)
            uint8_t sprite_px = (px_to_draw >> (7 - bit_pos)) & 0x1u;
            // if both pixels are on -> collision has been occured
            if (curr_px && sprite_px) {
                reg.V[0xf] = 1;
            }
            // either set or off the pixel on the actual display matrix
            curr_px ^= sprite_px;
        }
    }
    gfx_obj.redraw_screen<display_width, display_height>(display);
    reg.pc += 2;
}

// instruction : SKP Vx
inline void Chip8::inst_ex9e() noexcept {
    if (keypad[reg.V[x]]) {
        reg.pc += 2;
    } 
    reg.pc += 2;
}

// instruction : SKNP Vx
inline void Chip8::inst_exa1() noexcept {
    if (!keypad[reg.V[x]]) {
        reg.pc += 2;
    } 
    reg.pc += 2;
}

// instruction : LD Vx, DT
inline void Chip8::inst_fx07() noexcept {
    reg.V[x] = timer.delay;
    reg.pc += 2;
}

// instruction : LD Vx, K 
inline void Chip8::inst_fx0a() noexcept {
    for (uint8_t key {}; key < keypad_size; key++) {
        if (keypad[key]) {
            reg.V[x] = key;
            reg.pc += 2;
            return;
        }
    }
}

// instruction : LD DT, Vx
inline void Chip8::inst_fx15() noexcept {
    timer.delay = reg.V[x];
    reg.pc += 2;
}

// instruction : LD ST, Vx
inline void Chip8::inst_fx18() noexcept {
    timer.sound = reg.V[x];
    reg.pc += 2;
}

// instruction : ADD I, Vx
inline void Chip8::inst_fx1e() noexcept {
    reg.V[0xf] = (reg.I + reg.V[x]) > 0xfff ? 1 : 0;
    reg.I += reg.V[x];
    reg.pc += 2;
}

// instruction : LD F, Vx
inline void Chip8::inst_fx29() noexcept {
    reg.I = reg.V[x] * 5;
    reg.pc += 2;
}

// instruction : LD B, Vx
inline void Chip8::inst_fx33() noexcept {
    memory[reg.I] = reg.V[x] / 100;
    memory[reg.I + 1] = (reg.V[x] / 10) % 10;
    memory[reg.I + 2] = reg.V[x] % 10;
    reg.pc += 2;
}

// instruction : LD [I], Vx 
inline void Chip8::inst_fx55() noexcept {
    for (uint8_t idx {}; idx <= x; idx++) {
        memory[reg.I + idx] = reg.V[idx];
    }
    //reg.I += x + 1; // may be required by some ROMs
    reg.pc += 2;
}

// instruction : LD Vx, [I] 
inline void Chip8::inst_fx65() noexcept {
    for (uint8_t idx {}; idx <= x; idx++) {
        reg.V[idx] = memory[reg.I + idx];
    }
    //reg.I += x + 1; // may be required by some ROMs
    reg.pc += 2;
}

void Chip8::invalid_instruction_handler() noexcept {
    fprintf(stderr, "Illegal instruction : 0x%.4x at address 0x%x\n", instruction, reg.pc);
    exit(EXIT_FAILURE);
}
