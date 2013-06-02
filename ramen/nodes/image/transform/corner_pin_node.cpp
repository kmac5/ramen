// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/transform/corner_pin_node.hpp>

#include<cmath>

#include<boost/bind.hpp>

#include<OpenEXR/ImathMath.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>
#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/params/float2_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/button_param.hpp>

#include<ramen/image/processing.hpp>

#include<ramen/manipulators/quad_manipulator.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/nodes/image/track/tracker_node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/events.hpp>
#include<ramen/ui/dialogs/apply_corner_track_dialog.hpp>


namespace ramen
{
namespace image
{

corner_pin_node_t::corner_pin_node_t() : xform2d_node_t()
{ 
	set_name("corner_pin");
    param_set().param_changed.connect( boost::bind( &corner_pin_node_t::param_changed, this, _1, _2));
}

corner_pin_node_t::corner_pin_node_t( const corner_pin_node_t& other) : xform2d_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &corner_pin_node_t::param_changed, this, _1, _2));
}

void corner_pin_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> p( new float2_param_t( "Top Left"));
    p->set_id( "topleft");
    p->set_default_value( Imath::V2f( 0.0, 0.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Top Right"));
    p->set_id( "topright");
    p->set_default_value( Imath::V2f( 1.0, 0.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Bot Left"));
    p->set_id( "botleft");
    p->set_default_value( Imath::V2f( 0.0, 1.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

    p.reset( new float2_param_t( "Bot Right"));
    p->set_id( "botright");
    p->set_default_value( Imath::V2f( 1.0, 1.0));
    p->set_numeric_type( numeric_param_t::relative_xy);
    add_param( p);

	create_default_filter_param();

    std::auto_ptr<button_param_t> b( new button_param_t( "Apply Track..."));
    b->set_id( "apply_track");
    add_param( b);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

	create_motion_blur_param();
}

void corner_pin_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	RAMEN_ASSERT( composition());
	
	if( reason == param_t::user_edited && p == &param( "apply_track"))
	{		
		const tracker_node_t *tracker = ui::apply_corner_track_dialog_t::instance().exec();
		
		if( tracker)
		{
			float2_param_t *topleft		= dynamic_cast<float2_param_t*>( &param( "topleft"));
			float2_param_t *topright	= dynamic_cast<float2_param_t*>( &param( "topright"));
			float2_param_t *botleft		= dynamic_cast<float2_param_t*>( &param( "botleft"));
			float2_param_t *botright	= dynamic_cast<float2_param_t*>( &param( "botright"));
			
			float start_frame = composition()->start_frame();
			float end_frame = composition()->end_frame();
			
			param_set().begin_edit();
			
			for( float frame = start_frame; frame <= end_frame; frame += 1.0f)
			{
				boost::optional<Imath::V2f> p = tracker->tracker_pos( 0, frame);
				
				if( p)
					topleft->set_absolute_value_at_frame( p.get(), frame);

				p = tracker->tracker_pos( 1, frame);
				
				if( p)
					topright->set_absolute_value_at_frame( p.get(), frame);
				
				p = tracker->tracker_pos( 2, frame);

				if( p)
					botright->set_absolute_value_at_frame( p.get(), frame);
				
				p = tracker->tracker_pos( 3, frame);
				
				if( p)
					botleft->set_absolute_value_at_frame( p.get(), frame);
			}
			
			param_set().end_edit();
			update_widgets();
		}
	}
}

void corner_pin_node_t::do_create_manipulators()
{
    std::auto_ptr<quad_manipulator_t> m( new quad_manipulator_t( dynamic_cast<float2_param_t*>( &param( "topleft")),
																dynamic_cast<float2_param_t*>( &param( "topright")),
																dynamic_cast<float2_param_t*>( &param( "botleft")), 
																dynamic_cast<float2_param_t*>( &param( "botright"))));
	add_manipulator( m);
}

corner_pin_node_t::matrix3_type corner_pin_node_t::do_calc_transform_matrix_at_frame( float frame, int subsample) const
{
    Imath::Box2i src_area( input_as<image_node_t>()->format());
	src_area.max.x++;
	src_area.max.y++;

    boost::array<Imath::V2d, 4> src_pts;
    src_pts[0].x = src_area.min.x;
    src_pts[0].y = src_area.min.y;
    src_pts[1].x = src_area.max.x;
    src_pts[1].y = src_area.min.y;
    src_pts[2].x = src_area.max.x;
    src_pts[2].y = src_area.max.y;
    src_pts[3].x = src_area.min.x;
    src_pts[3].y = src_area.max.y;

    boost::array<Imath::V2d, 4> dst_pts;
    get_corners_at_frame( dst_pts, frame, 1.0f / subsample);

    if( quad_is_convex( dst_pts))
	{
		boost::optional<matrix3_type> m( quadToQuadMatrix( src_pts, dst_pts));

		if( m)
		    return m.get();
	}
	
	return matrix3_type( 0, 0, 0,
						0, 0, 0,
						0, 0, 0);
}

void corner_pin_node_t::get_corners( boost::array<Imath::V2d, 4>& pts, float scale) const
{
    pts[0] = get_absolute_value<Imath::V2f>( param( "topleft"))  * scale;
    pts[1] = get_absolute_value<Imath::V2f>( param( "topright")) * scale;
    pts[2] = get_absolute_value<Imath::V2f>( param( "botright")) * scale;
    pts[3] = get_absolute_value<Imath::V2f>( param( "botleft"))  * scale;
}

void corner_pin_node_t::get_corners_at_frame( boost::array<Imath::V2d, 4>& pts, float frame, float scale) const
{
    pts[0] = get_absolute_value_at_frame<Imath::V2f>( param( "topleft") , frame) * scale;
    pts[1] = get_absolute_value_at_frame<Imath::V2f>( param( "topright"), frame) * scale;
    pts[2] = get_absolute_value_at_frame<Imath::V2f>( param( "botright"), frame) * scale;
    pts[3] = get_absolute_value_at_frame<Imath::V2f>( param( "botleft") , frame) * scale;
}

// factory
node_t *create_corner_pin_node() { return new corner_pin_node_t();}

const node_metaclass_t *corner_pin_node_t::metaclass() const { return &corner_pin_node_metaclass();}

const node_metaclass_t& corner_pin_node_t::corner_pin_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.corner_pin";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Corner Pin";
        info.create = &create_corner_pin_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( corner_pin_node_t::corner_pin_node_metaclass());

} // namespace
} // namespace
