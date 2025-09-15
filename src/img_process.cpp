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

// video = vector ofmatrices of CV_8UC3
// masks = vector ofmatrices of CV_8UC3
std::expected<Video, std::string> alpha_blend(const Video &video, const Video &masks, const cv::Mat &mean_frame) {
    if (video.empty())
        return std::unexpected("[ALPHA] Empty video provided.");

    if (video.size() != masks.size())
        return std::unexpected("[ALPHA] Uneven number of masks and source frames.");

    Video blended;
    blended.reserve(video.size());

    double alpha{};
    const double alpha_step{1.0 / video.size()};
    const int rows{video[0].rows};
    const int cols{video[0].cols};

    // new_frame = k*(1-α * binary_mask_3c) + t*(binary_mask_3c * α)
    //
    // 	 k = frames containing person (original)
    // 	 t = frames without person
    // 	 binary_mask_3c: binary mask converted into 3 channels
    // 	 α: is the blending factor varies from 0 to 1

    // FIXME: I have no idea to be honest. Why does the mask get multipllied w/ `t` (mean frame i.e the one without the
    // person) too though?

    for (auto [frame, mask] : std::views::zip(video, masks)) {
        double alpha_add_inv{1.0 - alpha};

        cv::Mat output_frame{};
        output_frame.create(frame.rows, frame.cols, CV_8UC3);

        for (int y = 0; y < rows; y++) {

            const cv::Vec3b *frame_row   = frame.ptr<cv::Vec3b>(y);
            const std::uint8_t *mask_row = mask.ptr<std::uint8_t>(y);
            const std::uint8_t *mean_row = mean_frame.ptr<std::uint8_t>(y);
            // const std::uint8_t *mean_row = frame.ptr<std::uint8_t>(0);
            cv::Vec3b *output_row = output_frame.ptr<cv::Vec3b>(y);

            for (int x = 0; x < cols; x++) {
                const double diminished_mask{alpha_add_inv * (mask_row[x] == 255 ? 1 : 0)}; // (1 - alpha) * binary_mask
                const double amplified_mask{alpha * (mask_row[x] == 255 ? 1 : 0)};          // alpha * binary_mask

                const cv::Vec3b k{frame_row[x]};                          // coloured pixel
                const cv::Vec3b t{mean_row[x], mean_row[x], mean_row[x]}; // convert grayscale to coloured pixel

                output_row[x][0] = static_cast<std::uint8_t>(k[0] * diminished_mask) +
                                   static_cast<std::uint8_t>(t[0] * (amplified_mask));
                output_row[x][1] = static_cast<std::uint8_t>(k[1] * diminished_mask) +
                                   static_cast<std::uint8_t>(t[1] * amplified_mask);
                output_row[x][2] = static_cast<std::uint8_t>(k[2] * diminished_mask) +
                                   static_cast<std::uint8_t>(t[2] * amplified_mask);
            }
        }

        blended.emplace_back(output_frame);
        alpha += alpha_step;
    }

    return blended;
}
