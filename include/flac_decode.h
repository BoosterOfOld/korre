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

#include <cstdio>
#include <cstdlib>

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

    virtual ::FLAC__StreamDecoderWriteStatus write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[]);
    virtual void metadata_callback(const ::FLAC__StreamMetadata *metadata);
    virtual void error_callback(::FLAC__StreamDecoderErrorStatus status);
public:
    FLAC__uint64 total_samples = 0;
    uint32_t sample_rate = 0;
    uint32_t channels = 0;
    uint32_t bit_depth = 0;

    flac_decode(FILE *f_);

    std::shared_ptr<std::vector<double>> l;
    std::shared_ptr<std::vector<double>> r;
};
