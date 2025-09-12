#ifndef IO_H
#define IO_H

#include <opencv2/core/mat.hpp>

#include <optional>
#include <string_view>
#include <vector>

std::vector<cv::Mat> read_frames(std::string_view input_dir,
								 std::string_view input_ext,
								 std::optional<float> resize_perc);

#endif // IO_H
