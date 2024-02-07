#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

#include "decoders/mp3_decode.h"

mp3_decode::mp3_decode()
{
    l = std::make_unique<std::vector<double>>();
    r = std::make_unique<std::vector<double>>();
}

void mp3_decode::decode_file(const std::filesystem::path& path)
{
    mp3dec_t mp3d;
    mp3dec_file_info_t info;
    if (mp3dec_load(&mp3d, path.c_str(), &info, NULL, NULL))
    {
        /* error */
    }

    for (size_t i = 0; i < info.samples; ++i)
    {
        l->emplace_back(info.buffer[i]);
        r->emplace_back(info.buffer[ info.channels == 1 ? i : ++i ]);
    }

    this->bitrate = info.avg_bitrate_kbps;
    this->bit_depth = 16;
    this->sample_rate = info.hz;
    this->channels = info.channels;
    this->sample_count = l->size();

    free(info.buffer);
}

