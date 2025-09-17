#include <kybdl/morph.h>

#include <algorithm>
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

static void copy_into_padded(cv::Mat &padded, const cv::Mat &src, const int padding) {
    // Main body
    for (int y = 0; y < src.rows; y++) {
        const std::uint8_t *original_row = src.ptr<std::uint8_t>(y);
        std::uint8_t *padded_row         = padded.ptr<std::uint8_t>(y + padding);

        for (int x = 0; x < src.cols; x++)
            padded_row[x + padding] = original_row[x];
    }

    /*
     * CHECK: Premature optimisation is the root of all evil...?
     * I was expecting my supposed cache-conscious solution to perform better than the naive solution I've now
     * implemented in it's stead, but it was a solid 800ns seconds slower than the 1200ns run-time of the 'naive'
     * approach i.e the one with zero regards to cache evictions...
     *
     * Why is this?
     *
     * NOTE: Fret not, my epic cache-conscious implementation beats the naive one by about 100ns w/ optimisations
     * enabled so I'll take that as a win to be honest, and am bringing it back.
     */

    const std::uint8_t *original_north_row = src.ptr<std::uint8_t>(0);
    const std::uint8_t *original_south_row = src.ptr<std::uint8_t>(src.rows - 1);

    // Leftover full rows: top
    for (int y = 0; y < padding; y++) {
        std::uint8_t *padded_north_row = padded.ptr<std::uint8_t>(y);

        for (int x = 0; x < padding; x++)
            padded_north_row[x] = original_north_row[0];

        for (int x = 0; x < src.cols; x++)
            padded_north_row[x + padding] = original_north_row[x];

        for (int x = 0; x < padding; x++)
            padded_north_row[padded.cols - padding + x] = original_north_row[src.cols - 1];
    }

    // Leftover full rows: bottom
    for (int y = 0; y < padding; y++) {
        std::uint8_t *padded_south_row = padded.ptr<std::uint8_t>(padded.rows - padding + y);

        for (int x = 0; x < padding; x++)
            padded_south_row[x] = original_south_row[0];

        for (int x = 0; x < src.cols; x++)
            padded_south_row[x + padding] = original_south_row[x];

        for (int x = 0; x < padding; x++)
            padded_south_row[padded.cols - padding + x] = original_south_row[src.cols - 1];
    }

    // Leftover horizontal fringes
    for (int y = 0; y < src.rows; y++) {

        const std::uint8_t *original_row = src.ptr<std::uint8_t>(y);
        std::uint8_t *padded_row         = padded.ptr<std::uint8_t>(y + padding);

        for (int x = 0; x < padding; x++) {
            padded_row[x]                         = original_row[0];
            padded_row[padded.cols - padding + x] = original_row[src.cols - 1];
        }
    }

    return;
}

static cv::Mat create_padded_shell(const cv::Mat &img, int padding) {
    cv::Mat padded{};
    padded.create(img.rows + 2 * padding, img.cols + 2 * padding, img.type());
    return padded;
}

static cv::Mat morph_frame(const cv::Mat &frame, const cv::Mat &kernel, int iterations, MorphOp morph_op) {
    cv::Mat morphed{};
    morphed.create(frame.rows, frame.cols, frame.type());

    const std::vector<uint8_t> krnl_flat{kernel.data, kernel.data + (kernel.rows * kernel.cols)};
    const long n_ones{std::count(krnl_flat.begin(), krnl_flat.end(), 1)};
    const int half_dim = kernel.rows / 2;

    std::vector<uint8_t> patch{};
    patch.reserve(kernel.rows * kernel.cols);

    cv::Mat padded{create_padded_shell(frame, half_dim)};
    copy_into_padded(padded, frame, half_dim);
    const int rows{padded.rows};
    const int cols{padded.cols};

    for (int i = 0; i < iterations; i++) {
        for (int y = half_dim; y < rows - half_dim; y++) {
            for (int x = half_dim; x < cols - half_dim; x++) {

                // Convolution patch
                for (int yy = y - half_dim; yy <= y + half_dim; yy++) {
                    const std::uint8_t *padded_row = padded.ptr<std::uint8_t>(yy);
                    for (int xx = x - half_dim; xx <= x + half_dim; xx++)
                        patch.emplace_back(padded_row[xx]);
                }

                bool is_white{};

                /*
                 * NOTE: Either case could probably be optimised further using SIMD-invoking methods on vector since our
                 * kernel of choice _is_ just an all-high square matrix but I am leaving it as is in case we want to
                 * experiment w/ other kernels in the future
                 */

                // ALL pixels in vicinity must be white
                if (morph_op == MorphOp::erode) // CHECK: Find faster solution using vectorised bitwise and?
                    is_white = std::inner_product(krnl_flat.begin(), krnl_flat.end(), patch.begin(), 0) / n_ones == 255;

                // ANY pixel in the vicinity must be white
                else
                    is_white = std::inner_product(krnl_flat.begin(), krnl_flat.end(), patch.begin(), 0) > 0;

                morphed.at<std::uint8_t>(y - half_dim, x - half_dim) = is_white ? 255 : 0;
                patch.clear();
            }
        }

        if (i + 1 < iterations)
            copy_into_padded(padded, morphed, half_dim);
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
        return std::unexpected(std::format("[OPEN] Kernel size {} can't exceed image res {}x{}.", kernel_size,
                                           video[0].rows, video[0].cols));

    auto kernel_expected = create_kernel(kernel_size);
    if (!kernel_expected.has_value())
        return std::unexpected(kernel_expected.error());

    const cv::Mat kernel{kernel_expected.value()};

    Video opened_masks{};
    opened_masks.reserve(video.size());

    const size_t print_msg_step{std::max<size_t>(1, video.size() / 4)};

    /*
     * NOTE: Scrapped the extra vector because this approach (directly dilating the frame after eroding it) is
     * extremely branch-prediction friendly (w.r.t MorphOp) AND memory-efficient as we only have a single
     * intermediate frame as opposed to a whole vector of them.
     */

    for (size_t i = 0; const cv::Mat &frame : video) {
        if (i % print_msg_step == 0)
            std::println("Opened [{}/{}] frames.", i, video.size());
        cv::Mat eroded{morph_frame(frame, kernel, iterations, MorphOp::erode)};
        opened_masks.emplace_back(morph_frame(eroded, kernel, iterations, MorphOp::dilate));
        i++;
    }

    return opened_masks;
}
