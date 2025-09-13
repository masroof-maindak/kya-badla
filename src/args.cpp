#include <kybdl/args.h>

#include <expected>
#include <optional>

std::expected<ArgConfig, std::string> parse_args(int argc, char *argv[]) {
    argparse::ArgumentParser prog("kybdl", "v2025-09-13a", argparse::default_arguments::help);

    ArgConfig args{.frame_count = std::nullopt};

    prog.add_argument("-o", "--output-dir").required().help("specify the output dir").store_into(args.output_dir);

    prog.add_argument("-i", "--input-dir").required().help("specify the input dir").store_into(args.input_dir);

    prog.add_argument("-ie", "--input-ext")
        .help("specify the input frames' extension")
        .default_value(".png")
        .choices(".png", ".jpg")
        .store_into(args.input_ext);

    prog.add_argument("-oe", "--output-ext")
        .help("specify the output frames' extension")
        .default_value(".png")
        .choices(".png", ".jpg")
        .store_into(args.output_ext);

    prog.add_argument("-vf", "--video-format")
        .help("specify the output video's extension")
        .default_value(".mp4")
        .choices(".mp4", ".avi")
        .store_into(args.video_format);

    prog.add_argument("-rs", "--resize-scale")
        .help("specify the input frames' scale post-resizing -- (0 - 1)")
        .default_value(1.0f)
        .store_into(args.scale) // Broken when it's at the bottom, because why wouldn't it be.
        .scan<'g', float>();

    prog.add_argument("-fss", "--frame-save-step")
        .help("specify the number of frames after which a new frame will be saved")
        .default_value(10)
        .scan<'i', int>()
        .store_into(args.frame_save_step);

    prog.add_argument("-fc", "--frame-count")
        .help("specify the first #N frames to use for background generation")
        .scan<'i', int>();

    prog.add_argument("-mt", "--mahalanobis-threshold")
        .help("specify the threshold to be used to compute the mask via Mahalanobis Distance")
        .default_value(5)
        .scan<'i', int>()
        .store_into(args.mn_threshold);

    prog.add_argument("-ks", "--kernel-size")
        .help("specify the size of the kernel to be used for dilation/erosion")
        .default_value(3)
        .scan<'i', int>()
        .store_into(args.kernel_size);

    prog.add_argument("-mi", "--iterations")
        .help("specify the number of iterations the morphological operations should run for")
        .default_value(1)
        .scan<'i', int>()
        .store_into(args.kernel_size);

    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception &err) {
        auto errmsg = std::format("{}\n\n{}", err.what(), prog.usage());
        return std::unexpected(errmsg);
    }

    if (prog.is_used("-fc")) {
        int i{prog.get<int>("-fc")};

        if (i < 0)
            return std::unexpected(std::format("Frame count can not be negative: {}", i));

        // Can't directly `.store_into()` std::optional<float>
        args.frame_count = i;
    }

    if (prog.is_used("-ks")) {
        int i{prog.get<int>("-ks")};
        if (i < 3 || i > 255 || i % 2 != 1)
            return std::unexpected(std::format("Kernel size can range from 3 to 255 and must be odd: {}", i));
    }

    if (prog.is_used("-fss")) {
        int i{prog.get<int>("-fss")};
        if (i < 0)
            return std::unexpected(std::format("Frame save step can not be -ve: {}", i));
    }

    if (prog.is_used("-mt")) {
        int i{prog.get<int>("-mt")};
        if (i <= 0)
            return std::unexpected(std::format("Mahalanobis Distance' threshold must be +ve: {}", i));
    }

    if (prog.is_used("-mi")) {
        int i{prog.get<int>("-mi")};
        if (i <= 0)
            return std::unexpected(std::format("Morphological operation iteration count must be +ve: {}", i));
    }

    if (prog.is_used("-rs")) {
        float f{prog.get<float>("-rs")};
        if (f <= 0 || f > 1)
            return std::unexpected(std::format("Resize scale must be between (0, 1): {}", f));
    }

    return args;
}
