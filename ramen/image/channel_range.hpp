// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CHANNEL_RANGE_HPP
#define RAMEN_IMAGE_CHANNEL_RANGE_HPP

namespace ramen
{
namespace image
{

class channel_range_t
{
public:

    channel_range_t() {}

    float operator()( float x) const;

    float center;
    float tol_lo, tol_hi;
    float soft_lo, soft_hi;
};

} // namespace
} // namespace

#endif
