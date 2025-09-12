#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <opencv2/core/mat.hpp>

#include <expected>
#include <string>
#include <vector>

std::expected<std::vector<cv::Mat>, std::string> bgr_video_to_grayscale(const std::vector<cv::Mat> &video);

#endif // IMG_PROCESS_H
