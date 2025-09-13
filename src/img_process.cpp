#include <kybdl/img_process.h>

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cstdint>

std::expected<std::vector<cv::Mat>, std::string> bgr_video_to_grayscale(const std::vector<cv::Mat> &video) {
    std::vector<cv::Mat> video_gray{};
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
