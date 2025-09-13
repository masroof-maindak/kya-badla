#ifndef IO_H
#define IO_H

#include <kybdl/utils.h>

#include <opencv2/core/mat.hpp>

#include <expected>
#include <filesystem>
#include <string_view>

std::expected<Video, std::string> read_frames(std::string_view input_dir, std::string_view input_ext,
                                              float resize_scale);

std::expected<std::filesystem::path, std::string> save_frames(const Video &video, const std::string &out_dir,
                                                              const std::string &phase, std::string_view out_ext,
                                                              int frame_save_step);

std::expected<std::filesystem::path, std::string> save_image(const cv::Mat &img, const std::string &out_dir,
                                                             const std::string &phase, std::string_view out_ext);

#endif // IO_H
