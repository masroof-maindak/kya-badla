#ifndef IMG_PROCESS_H
#define IMG_PROCESS_H

#include <kybdl/utils.h>

#include <opencv2/core/mat.hpp>

#include <expected>
#include <string>

/**
 * @param video Input video as a vector of matrices of type CV_8UC3 (BGR uint8_t).
 * @return A vector of matrices of type CV_8UC1 (grayscale uint8_t), or an error string.
 */
std::expected<Video, std::string> bgr_video_to_grayscale(const Video &video);

/**
 * @param video Input video as a vector of matrices of type CV_8UC3 (BGR uint8_t).
 * @param masks Input binarily-thresholded video of masks as a vector of matrices of type CV_8UC1 (uint8_t).
 * @param mean_frame Mean frame as a grayscale HxW matrix of type CV_32FC1 (float).
 * @return A vector of matrices of type CV_8UC1 (grayscale uint8_t), or an error string.
 */
std::expected<Video, std::string> alpha_blend(const Video &video, const Video &masks, const cv::Mat &mean_frame);

#endif // IMG_PROCESS_H
