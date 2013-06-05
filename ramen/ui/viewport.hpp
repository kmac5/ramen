// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_VIEWPORT_HPP
#define	RAMEN_VIEWPORT_HPP

#include<OpenEXR/ImathMatrix.h>

#include<ImathExt/ImathBoxAlgo.h>

namespace ramen
{
namespace ui
{

class viewport_t
{
public:

    viewport_t();

    bool y_down() const         { return y_down_;}
    void set_y_down( bool b)    { y_down_ = b;}

    const Imath::Box2f& world() const   { return world_;}
    const Imath::Box2i& device() const  { return device_;}
	
    float zoom_x() const;
    float zoom_y() const;

    Imath::V2f screen_to_world( const Imath::V2i& p) const;
    Imath::V2i world_to_screen( const Imath::V2f& p) const;

    Imath::V2f screen_to_world_dir( const Imath::V2f& v) const;

    Imath::Box2f screen_to_world( const Imath::Box2i& b) const;
    Imath::Box2i world_to_screen( const Imath::Box2f& b) const;

    Imath::M33f world_to_screen_matrix() const;
    Imath::M33f screen_to_world_matrix() const;

    void reset();
    void reset( int w, int h);
    void reset( const Imath::Box2i& device);
    void reset( const Imath::Box2i& device, const Imath::Box2f& world);

    void resize( const Imath::Box2i& device);
    void resize( int w, int h);

    void scroll( const Imath::V2i& inc);
    void scroll_to_center_point( const Imath::V2f& center);

    void zoom( const Imath::V2f& center, float factor);
    void zoom( const Imath::V2f& center, float xfactor, float yfactor);

private:

    Imath::Box2i device_;
    Imath::Box2f world_;
    bool y_down_;
};

} // ui
} // ramen

#endif
