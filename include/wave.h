#pragma once

#include "internal_signal.h"
#include "decoders/flac_decode.h"
#include "decoders/mp3_decode.h"

//#include "dohm_signal_orig.hpp"

typedef struct wav_file {
    unsigned char chunkId[4];
    uint32_t chunkSize;
    unsigned char format[4];
    unsigned char subchunk1Id[4];
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    unsigned char subchunk2Id[4];
    uint32_t subchunk2Size;
    unsigned char * data;
    std::string extra;
} WavFile;

class wave
{
private:
    void swap_endian16(uint16_t val);
    void swap_endian32(uint32_t val);

    uint8_t wav_load_raw(const char *filePath, WavFile* wavFile);

    bool alloc;

public:
    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;
    WavFile wav_file;
    std::string notes;

    void load_flac(const char *path, bool normalize);
    void load_wav(const char *path, bool normalize);
    void load_mp3(const char *path, bool normalize);

    virtual ~wave();
};
