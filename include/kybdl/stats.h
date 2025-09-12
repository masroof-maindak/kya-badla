#ifndef STATS_H
#define STATS_H

#include <opencv2/core/mat.hpp>

#include <expected>
#include <optional>
#include <string>
#include <vector>

std::expected<cv::Mat, std::string> compute_mean(const std::vector<cv::Mat> &video, std::optional<int> frame_count);

std::expected<cv::Mat, std::string> compute_variance(const std::vector<cv::Mat> &video, const cv::Mat &mean_frame,
                                                     std::optional<int> frame_count);

std::expected<std::vector<cv::Mat>, std::string> compute_mask(const std::vector<cv::Mat> &video,
                                                              const cv::Mat &mean_frame, const cv::Mat &variance_frame,
                                                              int threshold);

#endif // STATS_H
