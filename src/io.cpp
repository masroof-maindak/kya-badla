#include <chng/io.h>

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <print>

using namespace cv;

std::vector<cv::Mat> read_frames(std::string_view input_dir,
								 std::string_view input_ext,
								 std::optional<float> resize_perc) {
	std::vector<cv::Mat> video{};

	const std::filesystem::path dir_path{input_dir};

	try {
		for (auto const &entry :
			 std::filesystem::directory_iterator{dir_path}) {

			if (entry.is_directory())
				continue;

			if (entry.path().extension() != input_ext)
				continue;

			cv::Mat img{cv::imread(entry.path(), IMREAD_COLOR_BGR)};

			if (img.data == nullptr) {
				std::println(stderr, "[LOADER] Failed to read/parse img: `{}`.",
							 entry.path().string());
			}

			if (resize_perc.has_value()) {
				cv::Mat resized{};
				float f = resize_perc.value();
				cv::resize(img, resized, cv::Size(), f, f, cv::INTER_LINEAR);
				img = resized;
			}

			video.emplace_back(img);
		}
	} catch (const std::filesystem::filesystem_error &err) {
		// TODO: return std::unexpected
		throw err;
	}

	return video;
}
