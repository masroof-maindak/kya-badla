#include <kybdl/stats.h>

#include <cmath>

std::expected<cv::Mat, std::string> compute_mean(const Video &video, std::optional<int> frame_count) {
    if (video.empty())
        return std::unexpected("[MEAN] Empty video provided.");

    if (frame_count.has_value() && frame_count.value() > video.size())
        return std::unexpected("[MEAN] Invalid frame_count.");

    const size_t n_frames = frame_count.has_value() ? frame_count.value() : video.size();
    const int rows        = video[0].rows;
    const int cols        = video[0].cols;

    cv::Mat mean_frame{};
    mean_frame.create(rows, cols, CV_32FC1);

    /*
     * CHECK: A two-pass approach with the first pass iterating over all frames and
     * simply summing all the values into the mean_frame's corresponding index, and
     * the second one iterating over the pixels of _just_ the mean_frame to compute
     * that pixel's average value would probably be more efficient.
     *
     * Another (minor) way this would be more performant would be by allowing us to
     * replace `.at()` calls with array-based indexing following pointer extraction
     * from `.ptr()`.
     */

    for (int y = 0; y < rows; y++) {

        float *mean_row = mean_frame.ptr<float>(y);

        for (int x = 0; x < cols; x++) {
            int sum{};

            auto it  = video.begin();
            auto end = frame_count.has_value() ? std::next(it, frame_count.value()) : video.end();

            for (; it != end; it++) {
                const cv::Mat &frame = *it;
                sum += frame.at<std::uint8_t>(y, x);
            }

            float result = static_cast<float>(sum) / n_frames;
            mean_row[x]  = result;
        }
    }

    return mean_frame;
}

std::expected<cv::Mat, std::string> compute_variance(const Video &video, const cv::Mat &mean_frame,
                                                     std::optional<int> frame_count) {
    if (video.empty())
        return std::unexpected("[VARIANCE] Empty video provided.");

    if (frame_count.has_value() && frame_count.value() > video.size())
        return std::unexpected("[VARIANCE] Invalid frame_count.");

    const size_t n_frames = frame_count.has_value() ? frame_count.value() : video.size();
    const int rows        = mean_frame.rows;
    const int cols        = mean_frame.cols;

    cv::Mat variance_frame{};
    variance_frame.create(rows, cols, CV_32FC1);

    for (int y = 0; y < rows; y++) {

        const float *mean_row = mean_frame.ptr<float>(y);
        float *variance_row   = variance_frame.ptr<float>(y);

        for (int x = 0; x < cols; x++) {
            int sum{};

            auto it  = video.begin();
            auto end = frame_count.has_value() ? std::next(it, frame_count.value()) : video.end();

            for (; it != end; it++) {
                const cv::Mat &frame   = *it;
                std::uint8_t intensity = frame.at<std::uint8_t>(y, x);
                float difference       = intensity - mean_row[x];
                sum += (difference * difference);
            }

            float result    = static_cast<float>(sum) / n_frames;
            variance_row[x] = result;
        }
    }

    return variance_frame;
}

std::expected<Video, std::string> compute_masks(const Video &video, const cv::Mat &mean_frame,
                                                const cv::Mat &variance_frame, int threshold) {
    if (video.empty())
        return std::unexpected("[MASKS] Empty video provided.");

    Video masks{};
    masks.reserve(video.size());

    for (const cv::Mat &frame : video) {
        cv::Mat mask{};
        mask.create(frame.rows, frame.cols, CV_8UC1);

        for (int y = 0; y < frame.rows; y++) {

            const std::uint8_t *video_row = frame.ptr<cv::uint8_t>(y);
            const float *mean_row         = mean_frame.ptr<float>(y);
            const float *variance_row     = variance_frame.ptr<float>(y);
            std::uint8_t *mask_row        = mask.ptr<std::uint8_t>(y);

            for (int x = 0; x < frame.cols; x++) {

                std::uint8_t intensity = video_row[x];
                float difference       = intensity - mean_row[x];
                float numerator        = difference * difference;
                float internal         = numerator / variance_row[x];
                float sqrt             = sqrtf(internal);

                mask_row[x] = sqrt > threshold ? 255 : 0;
            }
        }

        masks.emplace_back(mask);
    }

    return masks;
}
