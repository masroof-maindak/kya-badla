#include <chng/args.h>

#include <argparse/argparse.hpp>

ArgConfig parse_args(int argc, char *argv[]) {
	argparse::ArgumentParser prog("chng");

	prog.add_argument("-o", "--output-dir")
		.required()
		.help("specify the output dir.");

	prog.add_argument("-i", "--input-dir")
		.required()
		.help("specify the input dir.");

	prog.add_argument("-ie", "--input-ext")
		.help("specify the input frames' extension (png, jpg, jpeg).")
		.default_value("png");

	prog.add_argument("-oe", "--output-ext")
		.help("specify the output frames' extension (png, jpg).")
		.default_value("png");

	prog.add_argument("-vf", "--video-format")
		.help("specify the output video's extension (mp4, avi).")
		.default_value("mp4");

	try {
		prog.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << prog;
		throw err;
	}

	ArgConfig params{
		.input_dir	  = prog.get<std::string>("--output-dir"),
		.output_dir	  = prog.get<std::string>("--input-dir"),
		.input_ext	  = prog.get<std::string>("--input-ext"),
		.output_ext	  = prog.get<std::string>("--input-ext"),
		.video_format = prog.get<std::string>("--video-format"),
	};

	return params;
}
