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
#include <string_view>

template <typename T> T unwrap(std::expected<T, std::string> expected_val, const std::string &err_msg) {
    if (!expected_val.has_value())
        throw std::runtime_error(err_msg + ": " + expected_val.error());

    return expected_val.value();
}

inline void clear_video(Video &v, std::string_view video_name) {
    for (cv::Mat &f : v)
        f.release();
    v.clear();
    std::println("Freed video: {}", video_name);
}

int main(int argc, char *argv[]) {
    try {
        ArgConfig args = unwrap(parse_args(argc, argv), "Error parsing arguments");

        Video video_colour = unwrap(read_frames(args.input_dir, args.input_ext, args.scale), "Error reading frames");
        std::println("Loaded original video.");

        Video video_gray = unwrap(bgr_video_to_grayscale(video_colour), "Error converting to grayscale");
        std::println("Converted video to grayscale.");

        unwrap(save_frames(video_gray, args.output_dir, "gray", args.output_ext, args.frame_save_step),
               "Error saving gray frames");
        std::println("Saved gray frames.");

        auto mean = unwrap(compute_mean(video_gray, args.frame_count), "Error computing mean");
        unwrap(save_image(mean, args.output_dir, "mean", args.output_ext), "Error saving mean image");
        std::println("Computed and saved mean.");

        auto variance = unwrap(compute_variance(video_gray, mean, args.frame_count), "Error computing variance");
        unwrap(save_image(variance, args.output_dir, "variance", args.output_ext), "Error saving variance image");
        std::println("Computed and saved variance.");

        Video masks = unwrap(compute_masks(video_gray, mean, variance, args.mn_threshold), "Error computing masks");
        std::println("Succesfully computed Mahalanobis distance masks w/ threshold {}.", args.mn_threshold);

        clear_video(video_gray, "grayscale");

        unwrap(save_frames(masks, args.output_dir, "mask", args.output_ext, args.frame_save_step),
               "Error saving masked frames");
        std::println("Saved masked frames.");

        Video masks_opened = unwrap(open_masks(masks, args.kernel_size, args.iterations), "Error opening frame masks");
        std::println("Opened all masks.");

        clear_video(masks, "masks");

        unwrap(save_frames(masks_opened, args.output_dir, "mask-opened", args.output_ext, args.frame_save_step),
               "Error saving opened mask frames");
        std::println("Saved opened mask frames.");

        /*
         * CHECK: Connected Components -- do we even need this; we already have a lot of control over the output thanks
         * to all the CLI arguments we can set, and there's practically no noise after the aformentioned operations
         * anyway.
         */

        Video final{};

        if (args.remove_via_blend) {
            Video video_blended = unwrap(alpha_blend(video_colour, masks_opened, mean), "Error during alpha blending");
            std::println("Alpha blending complete.");

            clear_video(masks_opened, "opened masks");
            clear_video(video_colour, "colour");

            final = std::move(video_blended);

            unwrap(save_frames(video_blended, args.output_dir, "blend", args.output_ext, args.frame_save_step),
                   "Failed to save blended video");
            std::println("Saved blended frames.");
        } else {
            final = std::move(masks_opened);
        }

        mean.release();
        variance.release();

        unwrap(save_as_video(final, args.output_dir, args.video_format), "Error saving image vector as video.");
        std::println("Saved output video.");

        clear_video(final, args.remove_via_blend ? "blended" : "opened masks");
    } catch (const std::runtime_error &e) {
        std::println(stderr, "Error: {}", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
