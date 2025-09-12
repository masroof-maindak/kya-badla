#include <kybdl/frame_ops.h>

#include <opencv2/opencv.hpp>

#include <cstdint>

std::expected<std::vector<cv::Mat>, std::string>
bgr_video_to_grayscale(const std::vector<cv::Mat> &video) {
	std::vector<cv::Mat> ret{};
	ret.reserve(video.size());

	for (const cv::Mat &frame : video) {

		if (frame.empty() || frame.type() != CV_8UC3)
			return std::unexpected("[GRAYSCALE] Invalid type/empty frame.");

		cv::Mat tmp{};
		tmp.create(frame.rows, frame.cols, CV_8UC1);

		for (int y = 0; y < frame.rows; y++) {
			const cv::Vec3b *src = frame.ptr<cv::Vec3b>(y);
			std::uint8_t *dst	 = tmp.ptr<std::uint8_t>(y);

			for (int x = 0; x < frame.cols; x++) {
				const cv::Vec3b &px = src[x];
				auto mm				= std::minmax({px[0], px[1], px[2]});
				std::uint8_t mean	= (mm.first + mm.second) / 2;
				dst[x]				= mean;
			}
		}

		ret.emplace_back(tmp);
	}

	return ret;
}

std::expected<cv::Mat, std::string>
compute_mean(const std::vector<cv::Mat> &video,
			 std::optional<uint> frame_count) {
	if (video.empty())
		return std::unexpected("Empty video provided.");

	if (frame_count.has_value() && frame_count.value() > video.size())
		return std::unexpected("Invalid frame_count.");

	const size_t n_frames =
		frame_count.has_value() ? frame_count.value() : video.size();
	const int rows = video.at(0).rows;
	const int cols = video.at(0).cols;

	cv::Mat ret{};
	ret.create(rows, cols, CV_32FC1);

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			int sum{};

			auto it	 = video.begin();
			auto end = frame_count.has_value()
						   ? std::next(it, frame_count.value())
						   : video.end();

			for (; it != end; it++) {
				const cv::Mat &frame = *it;
				sum += frame.at<std::uint8_t>(y, x);
			}

			float result		= static_cast<float>(sum) / n_frames;
			ret.at<float>(y, x) = result;
		}
	}

	return ret;
}

std::expected<cv::Mat, std::string>
compute_variance(const std::vector<cv::Mat> &video, const cv::Mat &mean_frame,
				 std::optional<uint> frame_count) {
	if (video.empty())
		return std::unexpected("Empty video provided.");

	if (frame_count.has_value() && frame_count.value() > video.size())
		return std::unexpected("Invalid frame_count.");

	const size_t n_frames =
		frame_count.has_value() ? frame_count.value() : video.size();
	const int rows = mean_frame.rows;
	const int cols = mean_frame.cols;

	cv::Mat ret{};
	ret.create(rows, cols, CV_32FC1);

	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			int sum{};

			auto it	 = video.begin();
			auto end = frame_count.has_value()
						   ? std::next(it, frame_count.value())
						   : video.end();

			for (; it != end; it++) {
				const cv::Mat &frame   = *it;
				std::uint8_t intensity = (frame.at<std::uint8_t>(y, x));
				float difference	   = intensity - mean_frame.at<float>(y, x);
				sum += (difference * difference);
			}

			float variance		= static_cast<float>(sum) / n_frames;
			ret.at<float>(y, x) = variance;
		}
	}

	return ret;
}
