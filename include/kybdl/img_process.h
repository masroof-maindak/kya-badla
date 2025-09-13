#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <opencv2/core/mat.hpp>

#include <expected>
#include <string>
#include <vector>

/**
 * @param video Input video as a vector of matrices of type CV_8UC3 (BGR uint8_t).
 * @return A vector of matrices of type CV_8UC1 (grayscale uint8_t), or an error string.
 */
std::expected<std::vector<cv::Mat>, std::string> bgr_video_to_grayscale(const std::vector<cv::Mat> &video);

#endif // IMG_PROCESS_H
