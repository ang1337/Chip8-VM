#pragma once

#include <stdint.h>
#include <string>
#include <random>
#include <SFML/Audio.hpp>
#include "Graphics.hpp"

inline constexpr uint16_t memory_size          { 4096 };
inline constexpr uint8_t stack_size            { 16 },
                         display_width         { 64 },
                         display_height        { 32 },
                         fontset_size          { 80 },
                         keypad_size           { 16 },
                         global_jumptable_size { 16 },
                         subtable_op_0_size    { 15 },
                         subtable_op_8_size    { 15 }, 
                         subtable_op_e_size    { 15 },
                         subtable_op_f_size    { 102 },
                         general_reg_arr_size  { 16 };
// frequencies are in Hz
inline constexpr unsigned timers_frequency     { 60 }, // original timers frequency
                          cpu_frequency        { 500 }, // original Chip-8 frequency 
                          // this amount of cpu cycles will be emulated each time before timers get updated
                          timers_clock_cycles  { cpu_frequency / timers_frequency };
// instruction execution time = 2 ms for 500 Hz CPU
inline constexpr float instruction_time { (1.f / static_cast<float>(cpu_frequency)) * 1000.f }; 

class Chip8 {
    public:
        explicit Chip8(const std::string&, const std::string&, const uint8_t, const std::string&);
        ~Chip8() = default;
        void run() noexcept; 
    private:
        std::array<uint8_t, memory_size> memory; // Chip-8 memory space
        std::array<std::array<uint8_t, display_width>, display_height> display; // 64-wide 32-height display (will be scaled by the scale factor in actual window)
        Graphics gfx_obj;
        sf::SoundBuffer sound_buffer;
        const std::array<uint8_t, fontset_size> font_sprites;
        // this anonymous struct represents all Chip-8 registers
        struct {
            std::array<uint8_t, general_reg_arr_size> V; // general purpose registers
            uint16_t I; // index register
            uint16_t pc; // program counter
            uint8_t sp; // stack pointer
        } reg;
        sf::Sound beep;
        std::array<uint16_t, stack_size> stack; 
        std::array<uint8_t, keypad_size> keypad;
        uint16_t instruction, nnn, rom_size;
        const uint16_t rom_load_addr;
        // Chip-8 timers, decremented at the rate of 60 Hz
        struct {
            uint8_t delay, sound;
        } timer;
        // extremely thin random byte generator wrapper class 
        class RandomByteGenerator {
            public:
                explicit RandomByteGenerator();
                ~RandomByteGenerator() = default;
                uint8_t randbyte() noexcept;
            private:
                std::uniform_int_distribution<uint8_t> byte_distribution;
                inline static std::default_random_engine rand_gen { std::random_device()() };
        };
        RandomByteGenerator rand_byte_gen;
        uint8_t x, y, n, kk, opcode;
        // jump tables for instruction decoding routine
        // global_jt is a master jump table
        static void(Chip8::*const global_jt[global_jumptable_size])();
        static void(Chip8::*const subtable_op_0_jt[subtable_op_0_size])();
        static void(Chip8::*const subtable_op_8_jt[subtable_op_8_size])();
        static void(Chip8::*const subtable_op_e_jt[subtable_op_e_size])();
        static void(Chip8::*const subtable_op_f_jt[subtable_op_f_size])();

        void dispatch_0() noexcept;
        void inst_00e0() noexcept;
        void inst_00ee() noexcept;
        void inst_1nnn() noexcept;
        void inst_2nnn() noexcept;
        void inst_3xkk() noexcept;
        void inst_4xkk() noexcept;
        void inst_5xy0() noexcept;
        void inst_6xkk() noexcept;
        void inst_7xkk() noexcept;
        void dispatch_8() noexcept;
        void inst_8xy0() noexcept;
        void inst_8xy1() noexcept;
        void inst_8xy2() noexcept;
        void inst_8xy3() noexcept;
        void inst_8xy4() noexcept;
        void inst_8xy5() noexcept;
        void inst_8xy6() noexcept;
        void inst_8xy7() noexcept;
        void inst_8xye() noexcept;
        void inst_9xy0() noexcept;
        void inst_annn() noexcept;
        void inst_bnnn() noexcept;
        void inst_cxkk() noexcept;
        void inst_dxyn() noexcept;
        void dispatch_e() noexcept;
        void inst_ex9e() noexcept;
        void inst_exa1() noexcept;
        void dispatch_f() noexcept;
        void inst_fx07() noexcept;
        void inst_fx0a() noexcept;
        void inst_fx15() noexcept;
        void inst_fx18() noexcept;
        void inst_fx1e() noexcept;
        void inst_fx29() noexcept;
        void inst_fx33() noexcept;
        void inst_fx55() noexcept;
        void inst_fx65() noexcept;
        // in case of illegal instruction - call this function
        void invalid_instruction_handler() noexcept;

        void clear_display() noexcept;
        void initialize_vm();
        void load_rom(const std::string&);
        void load_sound(const std::string&);
        void emulate_cpu_cycle() noexcept;
        void update_timers() noexcept;
        void handle_key_up(sf::Event&) noexcept;
        void handle_key_down(sf::Event&) noexcept;
};
