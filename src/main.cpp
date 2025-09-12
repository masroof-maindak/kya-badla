#include <chng/args.h>
#include <chng/frame_ops.h>
#include <chng/io.h>

#include <opencv2/opencv.hpp>

#include <print>

int main(int argc, char *argv[]) {
	ArgConfig args{};

	try {
		args = std::move(parse_args(argc, argv));
	} catch (std::exception &_) {
		return 1;
	}

	auto video_colour = read_frames(args.input_dir, args.input_ext, args.scale);
	std::println("Loaded original video.");

	auto video_gray = bgr_video_to_grayscale(video_colour);
	std::println("Converted video to grayscale.");

	video_colour.clear();
	std::println("Freed colour video.");

	cv::imwrite("aa.jpg", video_gray[0]);

	return 0;
}
