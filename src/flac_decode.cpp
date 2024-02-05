#include "flac_decode.h"

::FLAC__StreamDecoderWriteStatus flac_decode::write_callback(const ::FLAC__Frame *frame, const FLAC__int32 * const buffer[])
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

void flac_decode::metadata_callback(const ::FLAC__StreamMetadata *metadata)
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

void flac_decode::error_callback(::FLAC__StreamDecoderErrorStatus status)
{
    //fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

flac_decode::flac_decode(FILE *f_): FLAC::Decoder::File(), f(f_)
{
    l = std::make_shared<std::vector<double>>();
    r = std::make_shared<std::vector<double>>();
}
