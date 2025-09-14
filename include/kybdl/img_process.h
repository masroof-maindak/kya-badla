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

std::expected<Video, std::string> alpha_blend(const Video &video, const Video &masks);

#endif // IMG_PROCESS_H
