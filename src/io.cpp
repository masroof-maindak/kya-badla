#include <kybdl/io.h>

#include <opencv2/opencv.hpp>

#include <print>

std::vector<cv::Mat> read_frames(std::string_view input_dir,
								 std::string_view input_ext,
								 std::optional<float> resize_scale) {
	std::vector<cv::Mat> video{};

	const std::filesystem::path dir_path{input_dir};

	try {
		for (auto const &entry :
			 std::filesystem::directory_iterator{dir_path}) {

			if (entry.is_directory())
				continue;

			if (entry.path().extension() != input_ext)
				continue;

			cv::Mat img{cv::imread(entry.path(), cv::IMREAD_COLOR_BGR)};

			if (img.data == nullptr) {
				std::println(stderr, "[LOADER] Failed to read/parse img: `{}`.",
							 entry.path().string());
			}

			if (resize_scale.has_value()) {
				cv::Mat resized{};
				float f = resize_scale.value();
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

std::filesystem::path save_frames(const std::vector<cv::Mat> &video,
								  const std::string &out_dir,
								  const std::string &phase,
								  std::string_view out_ext, size_t mod_step) {

	std::filesystem::path dir{out_dir + "/tmp/" + phase + "/"};

	std::error_code e;

	// TODO: handle error code variable in erroneous case
	std::filesystem::create_directories(dir, e);

	for (size_t i = 0; const auto &frame : video) {
		if (i % mod_step == 0) {
			auto fname = std::format("{}shot{}{}", dir.string(), i, out_ext);

			// TODO: handle imwrite errors -- manually imdecode + write bin
			cv::imwrite(fname, frame);
		}

		i++;
	}

	return dir;
}
