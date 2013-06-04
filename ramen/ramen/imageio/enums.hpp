// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_ENUMS_HPP
#define RAMEN_IMAGEIO_ENUMS_HPP

namespace ramen
{
namespace imageio
{

enum channel_type
{
    half_channel_type = 0,
    float_channel_type,
    ubyte_channel_type,
    ushort_channel_type,
	uint_channel_type
};

enum channels
{
    rgba_channels = 0,
    rgb_channels,
    alpha_channel
};

enum compression_type
{
    none_compression = 0,
    rle_compression,
    zips_compression,
    zip_compression,
    piz_compression,
    pxr24_compression,
    b44_compression,
    b44a_compression,
	lzw_compression
};

} // namespace
} // namespace

#endif
