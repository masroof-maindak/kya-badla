#include <kybdl/args.h>

#include <optional>

std::expected<ArgConfig, std::string> parse_args(int argc, char *argv[]) {
    argparse::ArgumentParser prog("chng");

    ArgConfig params{.scale = std::nullopt, .frame_count = std::nullopt};

    prog.add_argument("-o", "--output-dir").required().help("specify the output dir").store_into(params.output_dir);

    prog.add_argument("-i", "--input-dir").required().help("specify the input dir").store_into(params.input_dir);

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

    prog.add_argument("-fc", "--frame-count")
        .help("specify the first #N frames to use for background generation")
        .scan<'u', uint>();

    prog.add_argument("-mt", "--mahalanobis-threshold")
        .help("specify the threshold to be used to compute the mask via Mahalanobis Distance")
        .default_value(5)
        .scan<'i', int>()
        .store_into(params.mn_threshold);

    try {
        prog.parse_args(argc, argv);

        if (prog.is_used("-fc"))
            params.frame_count = prog.get<uint>("-fc");

        if (prog.is_used("-rs")) {
            float f = prog.get<float>("-rs");

            if (f <= 0 || f >= 1)
                return std::unexpected(std::format("Erroneous float: {}. Scale must be between (0, 1).", f));

            params.scale = f;
        }
    } catch (const std::exception &err) {
        auto errmsg = std::format("{}\n\n{}", err.what(), prog.usage());
        return std::unexpected(errmsg);
    }

    return params;
}
