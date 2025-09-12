#ifndef ARGS_H
#define ARGS_H

#include <argparse/argparse.hpp>

#include <expected>
#include <optional>
#include <string>

struct ArgConfig {
    int mn_threshold;
    int frame_save_step;
    float scale;
    std::optional<int> frame_count;
    std::string input_dir;
    std::string output_dir;
    std::string input_ext;
    std::string output_ext;
    std::string video_format;
};

std::expected<ArgConfig, std::string> parse_args(int argc, char *argv[]);

#endif // ARGS_H
