#ifndef FRAME_OPS_H
#define FRAME_OPS_H

#include <opencv2/core/mat.hpp>

#include <vector>

std::vector<cv::Mat> bgr_video_to_grayscale(const std::vector<cv::Mat> &video);

#endif // FRAME_OPS_H
