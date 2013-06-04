// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_POLYSCAN_CLIP_HPP
#define RAMEN_POLYSCAN_CLIP_HPP

#include<ramen/polyscan/poly.hpp>

#include<OpenEXR/ImathBox.h>

namespace ramen
{
namespace polyscan
{

enum clip_result_t
{
	poly_clip_in,
	poly_clip_out,
	poly_clip_partial
};

clip_result_t clip( poly_t& poly, const Imath::Box3f& box);

} // namespace
} // namespace

#endif
