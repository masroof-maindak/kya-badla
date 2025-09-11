#include <exception>

#include <argparse/argparse.hpp>
#include <opencv2/opencv.hpp>

struct ArgConfig {
	std::string input_dir;
	std::string output_dir;
	std::string input_ext;
	std::string output_ext;
	std::string video_format;
};

ArgConfig parse_args(int argc, char *argv[]) {
	argparse::ArgumentParser prog("chng");

	prog.add_argument("-o", "--output-dir")
		.required()
		.help("specify the output dir.");

	prog.add_argument("-i", "--input-dir")
		.required()
		.help("specify the input dir.");

	prog.add_argument("-ie", "--input-ext")
		.help("specify the input frames' extension (png, jpg, jpeg).")
		.default_value("png");

	prog.add_argument("-oe", "--output-ext")
		.help("specify the output frames' extension (png, jpg).")
		.default_value("png");

	prog.add_argument("-vf", "--video-format")
		.help("specify the output video's extension (mp4, avi).")
		.default_value("mp4");

	try {
		prog.parse_args(argc, argv);
	} catch (const std::exception &err) {
		std::cerr << err.what() << std::endl;
		std::cerr << prog;
		throw err;
	}

	ArgConfig params{
		.input_dir	  = prog.get<std::string>("--output-dir"),
		.output_dir	  = prog.get<std::string>("--input-dir"),
		.input_ext	  = prog.get<std::string>("--input-ext"),
		.output_ext	  = prog.get<std::string>("--input-ext"),
		.video_format = prog.get<std::string>("--video-format"),
	};

	return params;
}

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
