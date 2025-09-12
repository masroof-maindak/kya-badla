#include <filesystem>
#include <kybdl/io.h>

#include <opencv2/opencv.hpp>

#include <print>

std::expected<std::vector<cv::Mat>, std::string> read_frames(std::string_view input_dir, std::string_view input_ext,
                                                             float resize_scale) {
    std::vector<cv::Mat> video{};

    const std::filesystem::path dir_path{input_dir};

    try {
        for (auto const &entry : std::filesystem::directory_iterator{dir_path}) {

            if (entry.is_directory())
                continue;

            if (entry.path().extension() != input_ext)
                continue;

            cv::Mat img{cv::imread(entry.path(), cv::IMREAD_COLOR_BGR)};

            if (img.empty()) {
                std::println(stderr, "[LOADER] Failed to read/parse img: `{}`.", entry.path().string());
                return std::unexpected("Failed to read/parse image: " + entry.path().string());
            }

            if (resize_scale != 1.0) {
                cv::Mat resized{};
                float f{resize_scale};
                cv::resize(img, resized, cv::Size(), f, f, cv::INTER_LINEAR);
                img = resized;
            }

            video.emplace_back(img);
        }
    } catch (const std::filesystem::filesystem_error &err) {
        return std::unexpected(err.what());
    }

    if (video.empty())
        return std::unexpected("No frames found in the input directory.");

    return video;
}

std::expected<std::filesystem::path, std::string> save_frames(const std::vector<cv::Mat> &video,
                                                              const std::string &out_dir, const std::string &phase,
                                                              std::string_view out_ext, int frame_save_step) {

    std::filesystem::path dir{out_dir + "/tmp/" + phase + "/"};

    if (!std::filesystem::exists(dir)) {
        std::error_code e;
        if (!std::filesystem::create_directories(dir, e))
            return std::unexpected("Failed to create directory: " + e.message());
    }

    for (size_t i = 0; const cv::Mat &frame : video) {
        if (i % frame_save_step == 0) {
            auto fname = std::format("{}shot{}{}", dir.string(), i, out_ext);
            // TODO: handle imwrite errors
            cv::imwrite(fname, frame);
        }

        i++;
    }

    return dir;
}

std::expected<std::filesystem::path, std::string> save_image(const cv::Mat &img, const std::string &out_dir,
                                                             const std::string &phase, std::string_view out_ext) {

    std::filesystem::path dir{out_dir + "/tmp/" + phase + "/"};

    if (!std::filesystem::exists(dir)) {
        std::error_code e;
        if (!std::filesystem::create_directories(dir, e))
            return std::unexpected("Failed to create directory: " + e.message());
    }

    auto fname = std::format("{}img{}", dir.string(), out_ext);
    // TODO: handle imwrite errors
    cv::imwrite(fname, img);

    return dir;
}
