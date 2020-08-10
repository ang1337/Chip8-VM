#include "../include/Chip8.hpp"
#include <memory>
#include <unistd.h>
#include <tuple>
#include <regex>

void usage_info(char** argv, FILE* stream) {
    fprintf(stream, "Usage : %s -r <path to ROM> -a <path to beep WAV file (or whatever sound effect)> " 
                    "[-s <scale factor of the window, the default is 10 which emits 640x320 window>]\n", argv[0]);
    if (stream == stderr) {
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}

// this function determines if a string represents unsigned integer or not
bool is_uint(const std::string &str_arg) {
    return std::regex_match(str_arg, std::regex("[1-9]+[0-9]*"));
}

// tuple of parsed arguments is returned
// 0 - path to ROM
// 1 - path to sound effect WAV file (beep)
// 2 - window scale factor (default is x10 -> 640x320 window)
std::tuple<std::string, std::string, unsigned> parse_args(int argc, char** argv) {
    std::tuple<std::string, std::string, unsigned> args { std::make_tuple("", "", 10) };
    int opt {};
    while ((opt = getopt(argc, argv, "hr:a:s:")) != -1) {
        switch (opt) {
            case 'r': // -r option is for path to ROM
                std::get<0>(args) = optarg; 
                break;
            case 'a': // -a option is for path to sound file (typically beep)
                std::get<1>(args) = optarg;
                break;
            case 's': // -s option is for scale (default is x10)
                if (is_uint(optarg)) {
                    std::get<2>(args) = std::stoul(optarg) % 30; // scale factor is bounded x30
                    // if it wraps around 30 - reset to the default (x10)
                    if (std::get<2>(args) < 10) {
                        std::get<2>(args) = 10; 
                    }
                } else {
                    usage_info(argv, stderr);
                }
                break;
            case 'h': // -h option is for help
                if (argc == 2) {
                    usage_info(argv, stdout);
                } 
                usage_info(argv, stderr);
            default:
                usage_info(argv, stderr);
        }
    }
    if (std::get<0>(args).empty() || std::get<1>(args).empty()) {
        usage_info(argv, stderr);
    }
    return args;
}

int main(int argc, char** argv) {
    if (argc > 7) {
        usage_info(argv, stderr);
    }
    auto args_tup { parse_args(argc, argv) };
    const uint8_t scale_factor { static_cast<uint8_t>(std::get<2>(args_tup)) };
    const std::string path_to_rom   { std::get<0>(args_tup) },
                      path_to_sound { std::get<1>(args_tup) },
                      title         { "Chip-8 Emulator - " + 
                                      std::to_string(display_width * scale_factor) + 
                                      " x " + 
                                      std::to_string(display_height * scale_factor) };
    
    std::unique_ptr<Chip8> chip8_vm { std::make_unique<Chip8>(path_to_rom, path_to_sound, scale_factor, title) } ;
    chip8_vm->run(); 
    return 0;
}
