#include <chng/frame_ops.h>

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <cstdint>
#include <print>
#include <utility>

std::vector<cv::Mat> bgr_video_to_grayscale(const std::vector<cv::Mat> &video) {
	std::vector<cv::Mat> ret{};

	for (const cv::Mat &frame : video) {

		if (frame.empty() || frame.type() != CV_8UC3) {
			std::println(stderr,
						 "[GRAYSCALE] Invalid type/empty frame. Skipping.");
		}

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
