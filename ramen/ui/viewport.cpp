// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ui/viewport.hpp>

namespace ramen
{
namespace ui
{

viewport_t::viewport_t() : y_down_( false) {}
	
float viewport_t::zoom_x() const { return ( device_.size().x)  / ( world_.size().x);}
float viewport_t::zoom_y() const { return ( device_.size().y)  / ( world_.size().y);}

Imath::V2f viewport_t::screen_to_world( const Imath::V2i& p) const
{
	int y = p.y;

	if( y_down_)
		y = device_.min.y + ( device_.max.y - y);

	return Imath::V2f( ((p.x - device_.min.x) / zoom_x()) + world_.min.x,
						((y - device_.min.y) / zoom_y()) + world_.min.y);
}

Imath::V2i viewport_t::world_to_screen( const Imath::V2f& p) const
{
	int x = ((p.x - world_.min.x) * zoom_x()) + device_.min.x;
	int y = ((p.y - world_.min.y) * zoom_y()) + device_.min.y;

	if( y_down_)
		y = device_.min.y + ( device_.max.y - y);
	
	return Imath::V2i( x, y);
}

Imath::V2f viewport_t::screen_to_world_dir( const Imath::V2f& v) const
{
	return Imath::V2f( v.x / zoom_x(),
					   v.y / zoom_y());
}

Imath::Box2f viewport_t::screen_to_world( const Imath::Box2i& b) const
{
	return Imath::Box2f( screen_to_world( Imath::V2i( b.min.x, b.min.y)),
						 screen_to_world( Imath::V2i( b.max.x, b.max.y)));
}

Imath::Box2i viewport_t::world_to_screen( const Imath::Box2f& b) const
{
	return Imath::Box2i( world_to_screen( Imath::V2f( b.min.x, b.min.y)),
						 world_to_screen( Imath::V2f( b.max.x, b.max.y)));
}


void viewport_t::reset()
{
	world_ = Imath::Box2f( Imath::V2i( device_.min.x, device_.min.y),
						   Imath::V2i( device_.max.x, device_.max.y));
}

void viewport_t::reset( int w, int h)
{ 
	reset( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w-1, h-1)));
}

void viewport_t::reset( const Imath::Box2i& device)
{
	device_ = device;
	reset();
}

void viewport_t::reset( const Imath::Box2i& device, const Imath::Box2f& world)
{
	world_ = world;
	device_ = device;
}

void viewport_t::resize( const Imath::Box2i& device)
{
	world_.max.x = world_.min.x + ( device.size().x / zoom_x());
	world_.max.y = world_.min.y + ( device.size().y / zoom_y());
	device_ = device;
}

void viewport_t::resize( int w, int h) { resize( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w - 1, h - 1)));}

void viewport_t::scroll( const Imath::V2i& inc)
{
	if( y_down_)
        world_ = ImathExt::offsetBy( world_, Imath::V2f( inc.x / zoom_x(), -inc.y / zoom_y()));
	else
        world_ = ImathExt::offsetBy( world_, Imath::V2f( inc.x / zoom_x(),  inc.y / zoom_y()));
}

void viewport_t::scroll_to_center_point( const Imath::V2f& center)
{
    world_ = ImathExt::offsetBy( world_, center - world_.center());
}

void viewport_t::zoom( const Imath::V2f& center, float factor)
{ 
	zoom( center, factor, factor);
}

void viewport_t::zoom( const Imath::V2f& center, float xfactor, float yfactor)
{
    world_ = ImathExt::offsetBy( world_, Imath::V2f( -center.x, -center.y));
	world_.min.x *= xfactor;
	world_.min.y *= yfactor;
	world_.max.x *= xfactor;
	world_.max.y *= yfactor;
    world_ = ImathExt::offsetBy( world_, Imath::V2f( center.x, center.y));
}
	
Imath::M33f viewport_t::world_to_screen_matrix() const
{
    Imath::M33f m;

    m = Imath::M33f().setTranslation( Imath::V2f( -world_.min.x, -world_.min.y)) *
        Imath::M33f().setScale( Imath::V2f( zoom_x(), zoom_y())) *
        Imath::M33f().setTranslation( Imath::V2f( device_.min.x, device_.min.y));

    if( y_down_)
    {
        m = m * Imath::M33f().setTranslation( Imath::V2f( 0, -device_.min.y)) *
                Imath::M33f().setScale( Imath::V2f( 1, -1)) *
                Imath::M33f().setTranslation( Imath::V2f( 0, device_.max.y));
    }

    return m;
}

Imath::M33f viewport_t::screen_to_world_matrix() const
{
    Imath::M33f m( world_to_screen_matrix());
    return m.inverse();
}

} // ui
} // ramen
