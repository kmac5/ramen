// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_VIEWER_EMPTY_IMAGE_STRATEGY_HPP
#define	RAMEN_UI_VIEWER_EMPTY_IMAGE_STRATEGY_HPP

#include<ramen/ui/viewer/image_view/image_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

class empty_image_strategy_t : public image_strategy_t
{
public:

	empty_image_strategy_t( const Imath::Box2i& display_window);

	virtual bool update_pixels( const image::buffer_t& pixels, const Imath::Box2i& display_window, const Imath::Box2i& data_window);

	virtual void draw() const;

	virtual boost::optional<Imath::Color4f> color_at( const Imath::V2i& p) const;
};

} // viewer
} // ui
} // ramen

#endif
