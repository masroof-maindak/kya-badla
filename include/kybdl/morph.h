#ifndef MORPH_H
#define MORPH_H

#include <kybdl/utils.h>

#include <expected>
#include <string>

#include <opencv2/core/mat.hpp>

enum class MorphOp {
    erode,
    dilate,
};

/**
 * @param dim Dimension of the kernel (must be odd and between 3 and 255).
 * @return A binary, square kernel matrix of type CV_8UC1, or an error string.
 */
static std::expected<cv::Mat, std::string> create_kernel(int dim);

/**
 * @param img Input binarily-thresholded image of type CV_8UC1.
 * @param dim Dimension of the kernel used for padding.
 * @return A padded copy of the input image, of type CV_8UC1.
 */
static cv::Mat create_padded_copy(const cv::Mat &img, int dim);

/**
 * @param frame Padded, binarily-thresholded frame of type CV_8UC1.
 * @param kernel Binary, square kernel to use in morphological operation, of type CV_8UC1.
 * @param iterations Number of times the operation is applied.
 * @param morph_op Type of morphological operation (erode or dilate).
 * @return An output binarily-thresholded frame of type CV_8UC1 after the morphological operation.
 */
static cv::Mat morph_frame(const cv::Mat &frame, const cv::Mat &kernel, int iterations, MorphOp morph_op);

/**
 * @brief Performs morphological opening (erosion followed by dilation) on a grayscale video.
 * @param video Input binarily-thresholded video of masks as a vector of matrices of type CV_8UC1 (uint8_t).
 * @param kernel_size Size of the kernel for morphological operations.
 * @param iterations Number of times erosion and dilation are applied.
 * @return A video with opened masks, or an error string.
 */
std::expected<Video, std::string> open_masks(const Video &video, int kernel_size, int iterations);

#endif // MORPH_H
