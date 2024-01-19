#pragma once

#include <cstdint> // Necessary for uint32_t
#include <stdio.h>
#include <string.h>

#include "internal_signal.hpp"

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
} WavFile;

class wave
{
private:
    void swap_endian16(uint16_t val) {
        val = (val<<8) | (val>>8);
    }

    void swap_endian32(uint32_t val) {
        val = (val<<24) | ((val<<8) & 0x00ff0000) |
        ((val>>8) & 0x0000ff00) | (val>>24);
    }

    uint8_t wav_load_raw(const char *filePath, WavFile* wavFile) {
        uint8_t rc = 0;
        FILE *fp = fopen(filePath,"rb");
        //printf("wavreader file Path = %s from %s", (fp ? "file opened" : "failed to open file"), filePath);
        if(!fp) {
            rc = 1;
            return rc;
        }

        //chunkId
        size_t len = fread(wavFile->chunkId, sizeof(unsigned char), 4, fp);
        wavFile->chunkId[len] = '\0';
        //printf("wavreader len:%d chunkId: %s", (int) len , wavFile->chunkId);
        if (strcmp((const char *) wavFile->chunkId, "RIFF")) {
            rc = 2;
            return rc;
        }

        //chunkSize
        fread(&wavFile->chunkSize, sizeof(uint32_t), 1, fp);
        swap_endian32(wavFile->chunkSize);
        //printf("wavreader chunkSize: %d", wavFile->chunkSize);

        //format
        len = fread(wavFile->format, sizeof(unsigned char), 4, fp);
        wavFile->format[len] = '\0';
        //printf("wavreader format: %s", wavFile->format);
        if (strcmp((const char *)wavFile->format,"WAVE")) {
        rc = 3;
        return rc;
        }

        //subchunk1Id
        len = fread(wavFile->subchunk1Id, sizeof(unsigned char), 4, fp);
        wavFile->subchunk1Id[len] = '\0';
        //printf("wavreader subchunk1Id: %s", wavFile->subchunk1Id);
        if (strcmp((const char *)wavFile->subchunk1Id,"fmt ")) {
            rc = 4;
            return rc;
        }

        //subchunk1Size
        fread(&wavFile->subchunk1Size, sizeof(uint32_t), 1, fp);
        swap_endian32(wavFile->subchunk1Size);
        //printf("wavreader subchunk1Size: %d", wavFile->subchunk1Size);

        //audioFormat
        fread(&wavFile->audioFormat, sizeof(uint16_t), 1, fp);
        swap_endian16(wavFile->audioFormat);
        //printf("wavreader audioFormat: %d", wavFile->audioFormat);

        //numChannels
        fread(&wavFile->numChannels, sizeof(uint16_t), 1, fp);
        swap_endian16(wavFile->numChannels);
        //printf("wavreader numChannels: %d", wavFile->numChannels);

        //sampleRate
        fread(&wavFile->sampleRate, sizeof(uint32_t), 1, fp);
        swap_endian32(wavFile->sampleRate);
        //printf("wavreader sampleRate: %d", wavFile->sampleRate);

        //byteRate
        fread(&wavFile->byteRate, sizeof(uint32_t), 1, fp);
        swap_endian32(wavFile->byteRate);
        //printf("wavreader byteRate: %d", wavFile->byteRate);

        //blockAlign
        fread(&wavFile->blockAlign, sizeof(uint16_t), 1, fp);
        swap_endian16(wavFile->blockAlign);
        //printf("wavreader blockAlign: %d", wavFile->blockAlign);

        //bitsPerSample
        fread(&wavFile->bitsPerSample, sizeof(uint16_t), 1, fp);
        swap_endian16(wavFile->bitsPerSample);
        //printf("wavreader bitsPerSample: %d", wavFile->bitsPerSample);

        //subchunk2Id
        fread(wavFile->subchunk2Id, sizeof(unsigned char), 4, fp);
        wavFile->subchunk2Id[len] = '\0';
        //printf("wavreader subchunk2Id: %s", wavFile->subchunk2Id);

        fread(&wavFile->subchunk2Size, sizeof(uint32_t), 1, fp);
        swap_endian32(wavFile->subchunk2Size);
        //printf("wavreader subchunk2Size: %d", wavFile->subchunk2Size);

        wavFile->data = (unsigned char *) malloc (sizeof(unsigned char) * wavFile->subchunk2Size); //set aside sound buffer space
        fread(wavFile->data, sizeof(unsigned char), wavFile->subchunk2Size, fp); //read in our whole sound data chunk
        fclose(fp);

        return rc;
    }

    bool alloc;

public:
    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;
    WavFile wav_file;
    std::string notes;

    void load_wav(const char *path, bool normalize)
    {
        wav_load_raw(path, &wav_file);

        l = std::make_shared<std::vector<double>>();
        r = std::make_shared<std::vector<double>>();

        alloc = true;

        if (wav_file.numChannels == 2)
        {
            if (wav_file.bitsPerSample == 16)
            {
                for (int i = 0; i < wav_file.subchunk2Size; i += 4)
                {
                    int16_t li = wav_file.data[i] | wav_file.data[i + 1] << 8;
                    int16_t ri = wav_file.data[i + 2] | wav_file.data[i + 3] << 8;

                    l->emplace_back((double) li);
                    r->emplace_back((double) ri);
                }
            }
            else if (wav_file.bitsPerSample == 24)
            {
                for (int i = 0; i < wav_file.subchunk2Size; i += 6)
                {
                    int32_t li = 0;
                    int32_t ri = 0;

                    auto a = wav_file.data[i + 0];
                    auto b = wav_file.data[i + 1];
                    auto c = wav_file.data[i + 2];

                    li |= wav_file.data[i + 2];
                    li <<= 8;
                    li |= wav_file.data[i + 1];
                    li <<= 8;
                    li |= wav_file.data[i + 0];
                    //li <<= 8; // This is to conjure the sign bit in the 32 bit int
                    //li >>= 8; // Arithmetic right shift will keep the sign bit
                    if (li & 0x800000) li |= ~0xffffff;

                    ri |= wav_file.data[i + 5];
                    ri <<= 8;
                    ri |= wav_file.data[i + 4];
                    ri <<= 8;
                    ri |= wav_file.data[i + 3];
                    //ri <<= 8;
                    //ri >>= 8;
                    if (ri & 0x800000) ri |= ~0xffffff;

                    l->emplace_back((double) li);
                    r->emplace_back((double) ri);
                }
            }
        }
        else if (wav_file.numChannels == 1 || wav_file.numChannels == 0)
        {
            if (wav_file.bitsPerSample == 16)
            {
                for (int i = 0; i < wav_file.subchunk2Size; i += 2)
                {
                    int16_t li = wav_file.data[i] | wav_file.data[i + 1] << 8;

                    l->emplace_back((double) li);
                }
            }
            else if (wav_file.bitsPerSample == 24)
            {
                for (int i = 0; i < wav_file.subchunk2Size; i += 3)
                {
                    /*
                    int32_t li = 0;

                    li |= wav_file.data[i + 0];
                    li <<= 8;
                    li |= wav_file.data[i + 1];
                    li <<= 8;
                    li |= wav_file.data[i + 2];
                    //li <<= 8; // This is to conjure the sign bit in the 32 bit int
                    //li >>= 8; // Arithmetic right shift will keep the sign bit
                    if (li & 0x800000) li |= ~0xffffff;

                    l->emplace_back((double) li);
                    lr->emplace_back((double) li);*/

                    int32_t li = 0;

                    li |= wav_file.data[i + 2];
                    li <<= 8;
                    li |= wav_file.data[i + 1];
                    li <<= 8;
                    li |= wav_file.data[i + 0];
                    if (li & 0x800000) li |= ~0xffffff;

                    l->emplace_back((double) li);
                }
            }
        }
        else
        {
            throw(std::runtime_error("Only 1 or 2 channels are supported."));
        }

        if (normalize)
        {
            g_normalize_to_bit_depth(l, wav_file.bitsPerSample);
            g_normalize_to_bit_depth(r, wav_file.bitsPerSample);
            notes.append("N");
        }
    }

    virtual ~wave()
    {

    }
};
