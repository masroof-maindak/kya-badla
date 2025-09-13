#ifndef STATS_H
#define STATS_H

#include <kybdl/utils.h>

#include <opencv2/core/mat.hpp>

#include <expected>
#include <optional>
#include <string>

/**
 * @param video Input grayscale video as a vector of matrices of type CV_8UC1 (uint8_t).
 * @param frame_count Optional number of first N frames to use.
 * @return A grayscale HxW matrix of type CV_32FC1 (float) representing the mean frame, or an error string.
 */
std::expected<cv::Mat, std::string> compute_mean(const Video &video, std::optional<int> frame_count);

/**
 * @param video Input grayscale video as a vector of matrices of type CV_8UC1 (uint8_t).
 * @param mean_frame Mean frame as a grayscale HxW matrix of type CV_32FC1 (float).
 * @param frame_count Optional number of first N frames to use.
 * @return A grayscale HxW matrix of type CV_32FC1 (float) representing the variance frame, or an error string.
 */
std::expected<cv::Mat, std::string> compute_variance(const Video &video, const cv::Mat &mean_frame,
                                                     std::optional<int> frame_count);

/**
 * @param video Input grayscale video as a vector of matrices of type CV_8UC1 (uint8_t).
 * @param mean_frame Mean frame as a grayscale HxW matrix of type CV_32FC1 (float).
 * @param variance_frame Variance frame as a grayscale HxW matrix of type CV_32FC1 (float).
 * @param threshold Threshold value for Mahalanobis distance.
 * @return A vector of matrices of type CV_8UC1 (binary uint8_t) representing the change masks, or an error string.
 */
std::expected<Video, std::string> compute_masks(const Video &video, const cv::Mat &mean_frame,
                                                const cv::Mat &variance_frame, int threshold);

#endif // STATS_H
