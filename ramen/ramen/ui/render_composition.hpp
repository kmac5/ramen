// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_RENDER_COMPOSITION_HPP
#define	RAMEN_UI_RENDER_COMPOSITION_HPP

#include<ramen/app/composition_fwd.hpp>

#include<boost/function.hpp>

namespace ramen
{
namespace ui
{

void render_composition( composition_t& comp, int start, int end, int proxy_level,
						 int subsample, int mb_extra_samples, float mb_shutter_factor, bool selected_only);

} // namespace
} // namespace

#endif
