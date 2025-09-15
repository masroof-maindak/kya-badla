#include <kybdl/img_process.h>
#include <kybdl/utils.h>

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cstdint>
#include <ranges>

std::expected<Video, std::string> bgr_video_to_grayscale(const Video &video) {
    if (video.empty())
        return std::unexpected("[GRAYSCALE] Empty video provided.");

    Video video_gray{};
    video_gray.reserve(video.size());

    for (const cv::Mat &frame : video) {

        if (frame.empty() || frame.type() != CV_8UC3)
            return std::unexpected("[GRAYSCALE] Invalid type/empty frame.");

        cv::Mat gray_frame{};
        gray_frame.create(frame.rows, frame.cols, CV_8UC1);

        for (int y = 0; y < frame.rows; y++) {
            const cv::Vec3b *frame_row = frame.ptr<cv::Vec3b>(y);
            std::uint8_t *gray_row     = gray_frame.ptr<std::uint8_t>(y);

            for (int x = 0; x < frame.cols; x++) {
                const cv::Vec3b &px = frame_row[x];
                auto mm             = std::minmax({px[0], px[1], px[2]});
                std::uint8_t mean   = (mm.first + mm.second) / 2;
                gray_row[x]         = mean;
            }
        }

        video_gray.emplace_back(gray_frame);
    }

    return video_gray;
}

std::expected<Video, std::string> alpha_blend(const Video &video, const Video &masks, const cv::Mat &mean_frame) {
    if (video.empty())
        return std::unexpected("[ALPHA] Empty video provided.");

    if (video.size() != masks.size())
        return std::unexpected("[ALPHA] Uneven number of masks and source frames.");

    if (mean_frame.type() != CV_32FC1)
        return std::unexpected("[ALPHA] Mean frame is not single-channel float");

    Video blended;
    blended.reserve(video.size());

    double alpha{};
    const double alpha_step{1.0 / video.size()};
    const int rows{video[0].rows};
    const int cols{video[0].cols};

    for (auto [frame, mask] : std::views::zip(video, masks)) {
        double alpha_add_inv{1.0 - alpha};

        cv::Mat output_frame{};
        output_frame.create(frame.rows, frame.cols, CV_8UC3);

        // Generate bitwise not of mask
        cv::Mat mask_inv{};
        mask.copyTo(mask_inv);
        std::uint8_t *mask_inv_ptr = mask_inv.data;
        std::uint8_t *mask_ptr     = mask.data;
        const int size             = mask.rows * mask.cols;
        for (int i = 0; i < size; i++)
            mask_inv_ptr[i] = mask_ptr[i] ^ 255;

        for (int y = 0; y < rows; y++) {

            const cv::Vec3b *frame_row       = frame.ptr<cv::Vec3b>(y);
            const std::uint8_t *mask_row     = mask.ptr<std::uint8_t>(y);
            const std::uint8_t *not_mask_row = mask_inv.ptr<std::uint8_t>(y);
            const float *mean_row            = mean_frame.ptr<float>(y);
            cv::Vec3b *output_row            = output_frame.ptr<cv::Vec3b>(y);

            for (int x = 0; x < cols; x++) {
                const double amplifying_mask{alpha * (mask_row[x] == 255 ? 1 : 0)};          // a * binary_mask
                const double diminishing_mask{alpha_add_inv * (mask_row[x] == 255 ? 1 : 0)}; // (1 - a) * binary_mask

                const std::uint8_t mean_val = static_cast<std::uint8_t>(mean_row[x]);

                const cv::Vec3b coloured_px{frame_row[x]};
                const cv::Vec3b mean_px{mean_val, mean_val, mean_val};

                const cv::Vec3b coloured_diminishing_fg{coloured_px * diminishing_mask};
                const cv::Vec3b grayscale_amplifying_fg{mean_px * amplifying_mask};
                const cv::Vec3b coloured_bg{coloured_px * (not_mask_row[x] == 255 ? 1 : 0)};

                output_row[x] = coloured_diminishing_fg + grayscale_amplifying_fg + coloured_bg;
            }
        }

        blended.emplace_back(output_frame);
        alpha += alpha_step;
    }

    return blended;
}
