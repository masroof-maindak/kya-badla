#include <kybdl/args.h>
#include <kybdl/img_process.h>
#include <kybdl/io.h>
#include <kybdl/stats.h>

#include <opencv2/opencv.hpp>

#include <print>

int main(int argc, char *argv[]) {
    // Arg parsing
    auto args_expected = parse_args(argc, argv);
    if (!args_expected.has_value()) {
        std::println(stderr, "Error parsing arguments: {}", args_expected.error());
        return 1;
    }
    auto args = args_expected.value();

    // Video loading
    auto video_colour_expected = read_frames(args.input_dir, args.input_ext, args.scale);
    if (!video_colour_expected.has_value()) {
        std::println(stderr, "Error reading frames: {}", video_colour_expected.error());
        return 1;
    }
    auto video_colour = video_colour_expected.value();
    std::println("Loaded original video.");

    // Video recolouring
    auto video_gray_expected = bgr_video_to_grayscale(video_colour);
    if (!video_gray_expected.has_value()) {
        std::println(stderr, "Error converting to grayscale: {}", video_gray_expected.error());
        return 1;
    }
    auto video_gray = video_gray_expected.value();
    std::println("Converted video to grayscale.");

    // Clear original video
    video_colour.clear();
    std::println("Freed colour video.");

    // Save frames - grayscale
    auto save_gray_expected = save_frames(video_gray, args.output_dir, "gray", args.output_ext, args.frame_save_step);
    if (!save_gray_expected.has_value()) {
        std::println(stderr, "Error saving gray frames: {}", save_gray_expected.error());
        return 1;
    }
    std::println("Saved gray frames.");

    // Mean
    auto mean_expected = compute_mean(video_gray, args.frame_count);
    if (!mean_expected.has_value()) {
        std::println(stderr, "Error computing mean: {}", mean_expected.error());
        return 1;
    }
    auto mean = mean_expected.value();

    // Save mean frame
    auto save_mean_expected = save_image(mean, args.output_dir, "mean", args.output_ext);
    if (!save_mean_expected.has_value()) {
        std::println(stderr, "Error saving mean image: {}", save_mean_expected.error());
        return 1;
    }
    std::println("Computed and saved mean.");

    // Variance
    auto variance_expected = compute_variance(video_gray, mean, args.frame_count);
    if (!variance_expected.has_value()) {
        std::println(stderr, "Error computing variance: {}", variance_expected.error());
        return 1;
    }
    auto variance = variance_expected.value();

    // Save variance frame
    auto save_variance_expected = save_image(variance, args.output_dir, "variance", args.output_ext);
    if (!save_variance_expected.has_value()) {
        std::println(stderr, "Error saving variance image: {}", save_variance_expected.error());
        return 1;
    }
    std::println("Computed and saved variance.");

    // Mahalanobis Distance
    auto masks_expected = compute_mask(video_gray, mean, variance, args.mn_threshold);
    if (!masks_expected.has_value()) {
        std::println(stderr, "Error computing Mahalanobis distance masks: {}", masks_expected.error());
        return 1;
    }
    auto masks = masks_expected.value();
    std::println("Succesfully computed Mahalanobis distance masks w/ threshold {}.", args.mn_threshold);

    auto save_masks_expected = save_frames(masks, args.output_dir, "mask", args.output_ext, args.frame_save_step);
    if (!save_masks_expected.has_value()) {
        std::println(stderr, "Error saving masked frames: {}", save_masks_expected.error());
        return 1;
    }
    std::println("Saved masked frames.");

    // TODO: dilation/erosion

    return 0;
}
