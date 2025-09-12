#ifndef IO_H
#define IO_H

#include <opencv2/core/mat.hpp>

#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

std::vector<cv::Mat> read_frames(std::string_view input_dir,
								 std::string_view input_ext,
								 std::optional<float> resize_scale);

std::filesystem::path save_frames(const std::vector<cv::Mat> &video,
								  const std::string &out_dir,
								  const std::string &phase,
								  std::string_view out_ext, size_t mod_step);

#endif // IO_H
