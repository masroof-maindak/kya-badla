#include <kybdl/morph.h>

#include <cstdint>
#include <expected>
#include <format>
#include <numeric>
#include <print>

static std::expected<cv::Mat, std::string> create_kernel(int dim) {
    if (dim < 3 || dim > 255 || dim % 2 != 1)
        return std::unexpected(std::format("[KERNEL] size can range from 3 to 255 and must be odd: {}", dim));

    return cv::Mat{dim, dim, CV_8UC1, cv::Scalar::all(1)};
}

// FIXME: invalid memory access

static cv::Mat create_padded_copy(const cv::Mat &img, int kernel_size) {
    cv::Mat padded{};
    padded.create(kernel_size - 1, kernel_size - 1, img.type());
    const int half_dim = kernel_size / 2;

    // Main body
    for (int y = 0; y < img.rows; y++) {

        const std::uint8_t *original_row = img.ptr<std::uint8_t>(y);
        std::uint8_t *padded_row         = padded.ptr<std::uint8_t>(y + half_dim);

        for (int x = 0; x < img.cols; x++) {
            padded_row[x + half_dim] = original_row[x];
        }
    }

    std::println(stderr, "here");

    // CHECK: Premature optimisation is the root of all evil...?

    // Leftover full rows
    const std::uint8_t *original_north_row = img.ptr<std::uint8_t>(0);
    const std::uint8_t *original_south_row = img.ptr<std::uint8_t>(img.rows - 1);

    for (int y = 0; y > half_dim; y++) {
        std::uint8_t *padded_north_row = padded.ptr<std::uint8_t>(y);

        for (int x = 0; x > half_dim; x++)
            padded_north_row[x] = original_north_row[0];

        for (int x = 0; x < img.cols; x++)
            padded_north_row[x + half_dim] = original_north_row[x];

        for (int x = 0; x < half_dim; x++)
            padded_north_row[padded.cols - half_dim + x] = original_north_row[img.cols - 1];

        std::uint8_t *padded_south_row = padded.ptr<std::uint8_t>(padded.cols - half_dim + y);

        for (int x = 0; x > half_dim; x++)
            padded_south_row[x] = original_south_row[0];

        for (int x = 0; x < img.cols; x++)
            padded_south_row[x + half_dim] = original_south_row[x];

        for (int x = 0; x < half_dim; x++)
            padded_south_row[padded.cols - half_dim + x] = original_south_row[img.cols - 1];
    }

    // Leftover horizontal fringes
    for (int y = 0; y < img.rows; y++) {

        const std::uint8_t *original_row = img.ptr<std::uint8_t>(y);
        std::uint8_t *padded_row         = padded.ptr<std::uint8_t>(y + half_dim);

        for (int x = 0; x < half_dim; x++)
            padded_row[x] = original_row[0];

        for (int x = 0; x < half_dim; x++)
            padded_row[padded.cols - half_dim + x] = original_row[img.cols - 1];
    }

    return padded;
}

static cv::Mat morph_frame(const cv::Mat &frame, const cv::Mat &kernel, int iterations, MorphOp morph_op) {
    const cv::Mat padded{create_padded_copy(frame, kernel.rows)};

    cv::Mat morphed{};
    morphed.create(frame.rows, frame.cols, frame.type());

    const std::vector<uint8_t> krnl_flat{kernel.data, kernel.data + (kernel.rows * kernel.cols)};
    const long n_ones{std::count(krnl_flat.begin(), krnl_flat.end(), 1)};
    const int half_dim = kernel.rows / 2;

    std::vector<uint8_t> patch{};
    patch.reserve(kernel.rows * kernel.cols);

    for (int i = 0; i < iterations; i++) {

        for (int y = half_dim; y < frame.rows - half_dim; y++) {
            for (int x = half_dim; x < frame.cols - half_dim; x++) {

                for (int yy = y - half_dim; y <= y + half_dim; y++) {

                    const std::uint8_t *frame_row = frame.ptr<std::uint8_t>(yy);

                    for (int xx = x - half_dim; x <= x + half_dim; x++) {
                        patch.emplace_back(frame_row[x]);
                    }
                }

                std::uint8_t is_white{};

                // NOTE: Either case could probably be optimised further using SIMD-invoking methods on vector since our
                // kernel of choice _is_ just an all-high square matrix but I am leaving it as is in case we want to
                // experiment w/ other kernels in the future

                // ALL pixels in vicinity must be white
                if (morph_op == MorphOp::erode)
                    is_white = std::inner_product(krnl_flat.begin(), krnl_flat.end(), patch.begin(), 0) / n_ones == 255;

                // ANY pixel in the vicinity must be white
                else
                    is_white = std::inner_product(krnl_flat.begin(), krnl_flat.end(), patch.begin(), 0) > 0;

                morphed.at<std::uint8_t>(y, x) = is_white == 1 ? 255 : 0;
                patch.clear();
            }
        }

        // TODO: test removing frame's const qualifier and trying out the following
        // frame = morphed;
    }

    return morphed;
}

std::expected<Video, std::string> open_masks(const Video &video, int kernel_size, int iterations) {
    if (video.empty())
        return std::unexpected("[OPEN] Empty video provided.");

    if (iterations <= 0)
        return std::unexpected(std::format("[OPEN] Invalid iteration count provided: {}", iterations));

    // NOTE: accounts for padding!
    if (kernel_size > video[0].rows || kernel_size > video[0].cols)
        return std::unexpected(std::format("[OPEN] Kernel size {} is too large for image dimensions {}x{}.",
                                           kernel_size, video[0].rows, video[0].cols));

    auto kernel_expected = create_kernel(kernel_size);
    if (!kernel_expected.has_value())
        return std::unexpected(kernel_expected.error());

    const cv::Mat kernel{kernel_expected.value()};

    Video opened_masks{};
    opened_masks.reserve(video.size());

    /*
     * NOTE: Scrapped the extra vector because this approach (directly dilating the frame after eroding it) is
     * extremely branch-prediction friendly (w.r.t MorphOp) AND memory-efficient as we only have a single
     * intermediate frame as opposed to a whole vector of them.
     */

    for (const cv::Mat &frame : video) {
        cv::Mat eroded{morph_frame(frame, kernel, iterations, MorphOp::erode)};
        opened_masks.emplace_back(morph_frame(eroded, kernel, iterations, MorphOp::dilate));
    }

    return opened_masks;
}
