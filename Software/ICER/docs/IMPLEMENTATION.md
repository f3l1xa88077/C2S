# ICER Implementation Responsibilities

This document describes the purpose and responsibilities of each major module and function in the ICER reference implementation


# Compression flow

'''
icer_compress_image_yuv()
    |
    | -- icer_wave_transform_stages() (for each y u and v channels)
    |
    | -- compute ll_mean for each channel using sum
    | -- subtract mean from each pixel in each channel (normalises data to only carry differences between pixels)
    | 
    | -- icer_to_sign_magnitude()
    |
    |
    | -- build list of packet descriptors for each decomp level, subband, bit plane and colour channel
    | -- Y channel has double priority
    |
    | -- qsort packets based on priority
    |
    | -- initialise segment lookup table
    |
    | -- Create partition layout for subbands
    | -- Compress each segment of that subband
    |
    | -- Assemble output stream by traversing generated segment table in order and copy each segment to output buffer

'''