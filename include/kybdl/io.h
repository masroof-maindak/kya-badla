#ifndef IO_H
#define IO_H

#include <opencv2/core/mat.hpp>

#include <expected>
#include <filesystem>
#include <optional>
#include <string_view>
#include <vector>

std::expected<std::vector<cv::Mat>, std::string>
read_frames(std::string_view input_dir, std::string_view input_ext,
			std::optional<float> resize_scale);

std::expected<std::filesystem::path, std::string>
save_frames(const std::vector<cv::Mat> &video, const std::string &out_dir,
			const std::string &phase, std::string_view out_ext,
			size_t mod_step);

std::expected<std::filesystem::path, std::string>
save_image(const cv::Mat &img, const std::string &out_dir,
		   const std::string &phase, std::string_view out_ext);

#endif // IO_H
