#pragma once

#include <memory>
#include <vector>
#include <filesystem>

class mp3_decode
{
private:

public:
    int bitrate;
    int sample_rate;
    int bit_depth;
    int channels;
    size_t sample_count;

    mp3_decode();
    void decode_file(const std::filesystem::path& path);

    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;
};
