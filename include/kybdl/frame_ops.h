#ifndef FRAME_OPS_H
#define FRAME_OPS_H

#include <opencv2/core/mat.hpp>

#include <vector>

std::vector<cv::Mat> bgr_video_to_grayscale(const std::vector<cv::Mat> &video);

cv::Mat compute_mean(const std::vector<cv::Mat> &video,
					 std::optional<uint> frame_count);

cv::Mat compute_variance(const std::vector<cv::Mat> &video,
						 const cv::Mat &mean_frame,
						 std::optional<uint> frame_count);

#endif // FRAME_OPS_H
