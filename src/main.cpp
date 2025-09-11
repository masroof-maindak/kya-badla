#include <exception>

#include <chng/args.h>

#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
	ArgConfig params{};

	try {
		params = std::move(parse_args(argc, argv));
	} catch (std::exception &_) {
		return 1;
	}

	// Load image
	// cv::Mat img{cv::imread(argv[1], cv::IMREAD_COLOR_BGR)};
	// if (img.data == nullptr) {
	// 	std::println(stderr, "Failed to read/parse foreground. Exiting");
	// 	return 2;
	// }
	//
	// // Convert to grayscale before saving
	// cv::Mat result{};
	// cv::cvtColor(img, result, cv::COLOR_BGR2GRAY);
	// cv::imwrite("out.jpg", result);

	return 0;
}
