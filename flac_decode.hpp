#pragma once

#  if _MSC_VER >= 1800
#    include <inttypes.h>
#  elif _MSC_VER >= 1600
/* Visual Studio 2010 has decent C99 support */
#    include <stdint.h>
#    define PRIu64 "llu"
#    define PRId64 "lld"
#    define PRIx64 "llx"
#  else
#    include <climits>
#    ifndef UINT32_MAX
#      define UINT32_MAX _UI32_MAX
#    endif
#    define PRIu64 "I64u"
#    define PRId64 "I64d"
#    define PRIx64 "I64x"
#  endif

#include <stdio.h>
#include <stdlib.h>

#include "FLAC++/decoder.h"


static FLAC__bool write_little_endian_uint16(FILE *f, FLAC__uint16 x)
{
    return
            fputc(x, f) != EOF &&
            fputc(x >> 8, f) != EOF
            ;
}

static FLAC__bool write_little_endian_int16(FILE *f, FLAC__int16 x)
{
    return write_little_endian_uint16(f, (FLAC__uint16)x);
}

static FLAC__bool write_little_endian_uint32(FILE *f, FLAC__uint32 x)
{
    return
            fputc(x, f) != EOF &&
            fputc(x >> 8, f) != EOF &&
            fputc(x >> 16, f) != EOF &&
            fputc(x >> 24, f) != EOF
            ;
}

class flac_decode : public FLAC::Decoder::File
{
private:
    flac_decode(const flac_decode&);
    flac_decode&operator=(const flac_decode&);
protected:
    FILE *f;

    virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
    {
        //const FLAC__uint32 total_size = (FLAC__uint32)(total_samples * channels * (bps/8));
        size_t i;

        // Update data
        channels = flac_decode::get_channels();
        bit_depth = flac_decode::get_bits_per_sample();

        for(i = 0; i < frame->header.blocksize; i++)
        {
            l->emplace_back(buffer[0][i]);
            r->emplace_back(buffer[1][i]);
        }

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }
    virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata)
    {
        /* print some stats */
        if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
        {
            /* save for later */
            total_samples = metadata->data.stream_info.total_samples;
            sample_rate = metadata->data.stream_info.sample_rate;
            channels = metadata->data.stream_info.channels;
            bit_depth = metadata->data.stream_info.bits_per_sample;

            /*
            fprintf(stderr, "sample rate    : %u Hz\n", sample_rate);
            fprintf(stderr, "channels       : %u\n", channels);
            fprintf(stderr, "bits per sample: %u\n", bps);
            fprintf(stderr, "total samples  : %" PRIu64 "\n", total_samples);*/
        }
    }
    virtual void error_callback(::FLAC__StreamDecoderErrorStatus status)
    {
        //fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
    }
public:
    FLAC__uint64 total_samples = 0;
    uint32_t sample_rate = 0;
    uint32_t channels = 0;
    uint32_t bit_depth = 0;

    flac_decode(FILE *f_): FLAC::Decoder::File(), f(f_)
    {
        l = std::make_shared<std::vector<double>>();
        r = std::make_shared<std::vector<double>>();
    }

    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;

};

int flac_test()
{
    bool ok = true;
    flac_decode decoder(nullptr);

    if(!decoder) {
        //fprintf(stderr, "ERROR: allocating decoder\n");
        return 1;
    }

    (void)decoder.set_md5_checking(true);

    FLAC__StreamDecoderInitStatus init_status = decoder.init("/Users/northkillpd/temp/test.flac");
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        //fprintf(stderr, "ERROR: initializing decoder: %s\n", FLAC__StreamDecoderInitStatusString[init_status]);
        ok = false;
    }

    if(ok) {
        ok = decoder.process_until_end_of_stream();
        //fprintf(stderr, "decoding: %s\n", ok? "succeeded" : "FAILED");
        //fprintf(stderr, "   state: %s\n", decoder.get_state().resolved_as_cstring(decoder));
    }

    return 0;
}
