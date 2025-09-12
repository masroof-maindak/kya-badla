#ifndef FRAME_OPS_H
#define FRAME_OPS_H

#include <opencv2/core/mat.hpp>

#include <expected>
#include <string>
#include <vector>

std::expected<std::vector<cv::Mat>, std::string> bgr_video_to_grayscale(const std::vector<cv::Mat> &video);

std::expected<cv::Mat, std::string> compute_mean(const std::vector<cv::Mat> &video, std::optional<uint> frame_count);

std::expected<cv::Mat, std::string> compute_variance(const std::vector<cv::Mat> &video, const cv::Mat &mean_frame,
                                                     std::optional<uint> frame_count);

#endif // FRAME_OPS_H
