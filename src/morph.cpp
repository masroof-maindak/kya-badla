#include <kybdl/morph.h>

#include <expected>
#include <format>

static std::expected<cv::Mat, std::string> create_kernel(int dim) {
    if (dim < 3 || dim > 255 || dim % 2 != 1)
        return std::unexpected(std::format("[KERNEL] size can range from 3 to 255 and must be odd: {}", dim));

    return cv::Mat{dim, dim, CV_8UC1, cv::Scalar::all(1)};
}

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

    const int half_dim = kernel.rows / 2;

    for (int i = 0; i < iterations; i++) {
        // TODO: convolve
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

    Video eroded_masks{};
    eroded_masks.reserve(video.size());

    for (const cv::Mat &frame : video)
        eroded_masks.emplace_back(morph_frame(frame, kernel, iterations, MorphOp::erode));

    Video opened_masks{};
    opened_masks.reserve(video.size());

    for (const cv::Mat &frame : eroded_masks)
        opened_masks.emplace_back(morph_frame(frame, kernel, iterations, MorphOp::dilate));

    return opened_masks;
}
