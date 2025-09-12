#include <expected>
#include <kybdl/args.h>

#include <optional>

std::expected<ArgConfig, std::string> parse_args(int argc, char *argv[]) {
    argparse::ArgumentParser prog("kybdl", "v2025-09-13a", argparse::default_arguments::help);

    ArgConfig params{.frame_count = std::nullopt};

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
        .default_value(1.0)
        .scan<'g', float>()
        .store_into(params.scale);

    prog.add_argument("-fss", "--frame-save-step")
        .help("specify the number of frames after which a new frame will be saved")
        .default_value(10)
        .scan<'i', int>()
        .store_into(params.frame_save_step);

    prog.add_argument("-fc", "--frame-count")
        .help("specify the first #N frames to use for background generation")
        .scan<'i', int>();

    prog.add_argument("-mt", "--mahalanobis-threshold")
        .help("specify the threshold to be used to compute the mask via Mahalanobis Distance")
        .default_value(5)
        .scan<'i', int>()
        .store_into(params.mn_threshold);

    try {
        prog.parse_args(argc, argv);

        if (prog.is_used("-fc")) {
            int i{prog.get<int>("-fc")};

            if (i < 0)
                return std::unexpected(std::format("Erroneous arg {}. Count can not be negative.", i));

            // Can't directly `.store_into()` std::optional<float>
            params.frame_count = prog.get<uint>("-fc");
        }

        if (prog.is_used("-fss")) {
            int i{prog.get<int>("-fss")};
            if (i < 0)
                return std::unexpected(std::format("Erroneous arg {}. Frame save step can not be negative.", i));
        }

        if (prog.is_used("-mt")) {
            int i{prog.get<int>("-mt")};
            if (i < 0)
                return std::unexpected(
                    std::format("Erroneous arg {}. Mahalanobis Distance threshold can not be negative.", i));
        }

        if (prog.is_used("-rs")) {
            float f{prog.get<float>("-rs")};
            if (f <= 0 || f > 1)
                return std::unexpected(std::format("Erroneous float: {}. Scale must be between (0, 1).", f));
        }

    } catch (const std::exception &err) {
        auto errmsg = std::format("{}\n\n{}", err.what(), prog.usage());
        return std::unexpected(errmsg);
    }

    return params;
}
