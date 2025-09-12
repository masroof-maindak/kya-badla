#include <chng/args.h>

#include <optional>
#include <print>

ArgConfig parse_args(int argc, char *argv[]) {
	argparse::ArgumentParser prog("chng");

	ArgConfig params{.scale = std::nullopt};

	prog.add_argument("-o", "--output-dir")
		.required()
		.help("specify the output dir")
		.store_into(params.output_dir);

	prog.add_argument("-i", "--input-dir")
		.required()
		.help("specify the input dir")
		.store_into(params.input_dir);

	prog.add_argument("-ie", "--input-ext")
		.help("specify the input frames' extension")
		.default_value(".png")
		.choices(".png", ".jpg")
		.store_into(params.input_ext);

	prog.add_argument("-oe", "--output-ext")
		.help("specify the output frames' extension")
		.default_value(".png")
		.choices(".png", ".jpg")
		.store_into(params.output_ext);

	prog.add_argument("-vf", "--video-format")
		.help("specify the output video's extension")
		.default_value(".mp4")
		.choices(".mp4", ".avi")
		.store_into(params.video_format);

	prog.add_argument("-rs", "--resize-scale")
		.help("specify the input frames' scale post-resizing -- (0 - 1)")
		.scan<'g', float>();

	try {
		prog.parse_args(argc, argv);

		if (prog.is_used("--resize-scale")) {
			float f = prog.get<float>("--resize-scale");

			if (f <= 0 || f >= 1) {
				std::println(stderr, "Erroneous float: {}. Ignoring.", f);
			} else {
				params.scale = f;
			}
		}

	} catch (const std::exception &err) {
		std::println(stderr, "{}\n\n{}", err.what(), prog.usage());
		throw err;

		// TODO: return std::unexpected;
	}

	return params;
}
