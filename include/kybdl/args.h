#ifndef ARGS_H
#define ARGS_H

#include <argparse/argparse.hpp>

#include <optional>
#include <string>

struct ArgConfig {
	std::string input_dir;
	std::string output_dir;
	std::string input_ext;
	std::string output_ext;
	std::string video_format;
	std::optional<float> scale;
	std::optional<uint> frame_count;
};

ArgConfig parse_args(int argc, char *argv[]);

#endif // ARGS_H
