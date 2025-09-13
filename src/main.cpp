#include <kybdl/args.h>
#include <kybdl/img_process.h>
#include <kybdl/io.h>
#include <kybdl/morph.h>
#include <kybdl/stats.h>

#include <expected>
#include <print>
#include <stdexcept>
#include <stdlib.h>
#include <string>

template <typename T> T unwrap(std::expected<T, std::string> expected_val, const std::string &err_msg) {
    if (!expected_val.has_value())
        throw std::runtime_error(err_msg + ": " + expected_val.error());

    return expected_val.value();
}

int main(int argc, char *argv[]) {
    try {
        auto args = unwrap(parse_args(argc, argv), "Error parsing arguments");

        auto video_colour = unwrap(read_frames(args.input_dir, args.input_ext, args.scale), "Error reading frames");
        std::println("Loaded original video.");

        auto video_gray = unwrap(bgr_video_to_grayscale(video_colour), "Error converting to grayscale");
        std::println("Converted video to grayscale.");

        video_colour.clear();
        std::println("Freed colour video.");

        unwrap(save_frames(video_gray, args.output_dir, "gray", args.output_ext, args.frame_save_step),
               "Error saving gray frames");
        std::println("Saved gray frames.");

        auto mean = unwrap(compute_mean(video_gray, args.frame_count), "Error computing mean");
        unwrap(save_image(mean, args.output_dir, "mean", args.output_ext), "Error saving mean image");
        std::println("Computed and saved mean.");

        auto variance = unwrap(compute_variance(video_gray, mean, args.frame_count), "Error computing variance");
        unwrap(save_image(variance, args.output_dir, "variance", args.output_ext), "Error saving variance image");
        std::println("Computed and saved variance.");

        auto masks = unwrap(compute_masks(video_gray, mean, variance, args.mn_threshold), "Error computing masks");
        std::println("Succesfully computed Mahalanobis distance masks w/ threshold {}.", args.mn_threshold);

        unwrap(save_frames(masks, args.output_dir, "mask", args.output_ext, args.frame_save_step),
               "Error saving masked frames");
        std::println("Saved masked frames.");

        auto opened_masks = unwrap(open_masks(masks, args.kernel_size, args.iterations), "Error opening masked frames");
        std::println("Opened masks.");

        masks.clear();
        std::println("Freed old masks");

        unwrap(save_frames(opened_masks, args.output_dir, "mask-opened", args.output_ext, args.frame_save_step),
               "Error saving opened mask frames");
        std::println("Saved opened mask frames.");

        // TODO: connected components

    } catch (const std::runtime_error &e) {
        std::println(stderr, "Error: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
