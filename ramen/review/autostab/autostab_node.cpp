// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/track/autostab_node.hpp>

#include<utility>

#include<boost/bind.hpp>

#include<QProgressDialog>

#include<ramen/assert.hpp>

#include<ramen/params/button_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/separator_param.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/render/context_guard.hpp>

#include<ramen/manipulators/quad_manipulator.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

#include<iostream>

namespace ramen
{
namespace image
{
namespace
{

struct track_callback
{
	track_callback( QProgressDialog *p, int value) : progress_( p), value_( value) {}
	~track_callback()
	{
		if( !progress_->wasCanceled())
			progress_->setValue( value_);
	}
	
	bool operator()( int x)
	{
		progress_->setValue( x);
		
		for( int i = 0; i < 10; ++i)
			ui::user_interface_t::Instance().process_events();
		
		return progress_->wasCanceled();
	}
		
private:

	QProgressDialog *progress_;
	int value_;
};

struct cancel_callback
{
	cancel_callback( QProgressDialog *p) : progress_( p) {}
	
	bool operator()()
	{
		for( int i = 0; i < 10; ++i)
			ui::user_interface_t::Instance().process_events();
			
		return progress_->wasCanceled();
	}
	
private:
	
	QProgressDialog *progress_;
};

std::vector<cv::KeyPoint> manip_ref_keyp, manip_keyp;

} // unnamed

class autostab_manipulator_t : public manipulator_t
{
public:

	autostab_manipulator_t() : manipulator_t() {}
	
private:
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const
	{
		const ui::viewer::image_viewer_strategy_t *view = dynamic_cast<const ui::viewer::image_viewer_strategy_t*>( event.view);
		RAMEN_ASSERT( view);
		
		gl_point_size( 4);
		gl_color3f( 1, 0.5, 0);
		gl_begin( GL_POINTS);
			for( int i = 0; i < manip_ref_keyp.size(); ++i)
			{
				gl_vertex2f( manip_ref_keyp[i].pt.x * view->subsample() * view->aspect_ratio(),
							 manip_ref_keyp[i].pt.y * view->subsample());
			}
		gl_end();

		gl_color3f( 0, 0, 1);
		gl_begin( GL_POINTS);
			for( int i = 0; i < manip_keyp.size(); ++i)
			{
				gl_vertex2f( manip_keyp[i].pt.x * view->subsample() * view->aspect_ratio(),
							 manip_keyp[i].pt.y * view->subsample());
			}
		gl_end();
	}
};

autostab_node_t::autostab_node_t() : cv_autoalign_node_t()
{ 
	set_name("autostab");
    param_set().param_changed.connect( boost::bind( &autostab_node_t::param_changed, this, _1));
}

autostab_node_t::autostab_node_t( const autostab_node_t& other) : cv_autoalign_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &autostab_node_t::param_changed, this, _1));
}

void autostab_node_t::cloned()
{
	cv_autoalign_node_t::cloned();
	get_similarity_params();
	get_perspective_params();
	get_corner_pin_params();
}

void autostab_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> pop( new popup_param_t( "Range"));
    pop->set_id( "range");
    pop->menu_items() = boost::assign::list_of( "Global")( "Custom");
    add_param( pop);	

	std::auto_ptr<float2_param_t> vec2( new float2_param_t( "Range"));
	vec2->set_id( "custom_range");
	vec2->set_default_value( Imath::V2f( 1, 100));
	vec2->set_round_to_int( true);
	vec2->set_enabled( false);
	vec2->set_static( true);
	add_param( vec2);

    pop.reset( new popup_param_t( "Mode"));
    pop->set_id( "mode");
    pop->menu_items() = boost::assign::list_of( "Absolute")( "Incremental");
	pop->set_enabled( false);
    add_param( pop);

	std::auto_ptr<float_param_t> vec( new float_param_t( "Ref Frame"));
	vec->set_id( "ref_frame");
	vec->set_default_value( 1);
	vec->set_round_to_int( true);
	vec->set_static( true);
	add_param( vec);

	pop.reset( new popup_param_t( "Alpha"));
	pop->set_id( "alpha");
    pop->menu_items() = boost::assign::list_of( "None")( "Alpha")( "Inverted Alpha");
    add_param( pop);
	
    std::auto_ptr<button_param_t> but( new button_param_t( "Track..."));
    but->set_id( "track");
    add_param( but);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    pop.reset( new popup_param_t( "Trasform"));
    pop->set_id( "xform");
    pop->menu_items() = boost::assign::list_of( "T")( "TR")( "TS")( "TRS")( "Perspective");
	pop->set_default_value( 4);
    add_param( pop);
	
    pop.reset( new popup_param_t( "Filter"));
    pop->set_id( "filter");
    pop->menu_items() = boost::assign::list_of( "Point")( "Bilinear")( "Bicubic");
    pop->set_default_value( 2);
    add_param( pop);

	create_similarity_params();
	create_perspective_params();
	create_corner_pin_params();
}

void autostab_node_t::param_changed( param_t *p)
{
	if( p == &param( "range"))
	{
		param( "custom_range").set_enabled( get_value<int>( *p) == 1);
		return;
	}

	if( p == &param( "mode"))
	{
		param( "ref_frame").set_enabled( get_value<int>( *p) == 0);
		return;
	}
	
	if( p == &param( "track"))
		track();
}

void autostab_node_t::do_create_manipulators()
{
	std::auto_ptr<manipulator_t> m( new autostab_manipulator_t());
	add_manipulator( m);
	
	/*
	if( corner_[0])
		get_corner_pin_params();
	
	m.reset( new quad_manipulator_t( this, corner_[0], corner_[1], corner_[2], corner_[3]));
	add_manipulator( m);
	*/
}

void autostab_node_t::do_end_active()
{
	manip_ref_keyp.clear();
	manip_keyp.clear();
}

Imath::M33d autostab_node_t::do_calc_transform_matrix_at_frame( float frame, int subsample) const
{	
	try
	{
		switch( get_value<int>( param( "xform")))
		{
			case 0:
			{
				Imath::M33d m( similarity_matrix_at_frame( frame, subsample));
				return remove_SR( m);
			}
		
			case 1:
			{
				Imath::M33d m( similarity_matrix_at_frame( frame, subsample));
				return remove_S( m);
			}
		
			case 2:
			{
				Imath::M33d m( similarity_matrix_at_frame( frame, subsample));
				return remove_R( m);
			}
				
			case 3:
				return similarity_matrix_at_frame( frame, subsample);
		
			case 4:
			{
				Imath::M33d m = perspective_matrix_at_frame( frame, subsample);
				Imath::M33d n = corner_pin_matrix_at_frame( frame, subsample);
				return n;
			}
		}
	}
	catch( ...)
	{
		return Imath::M33d( 0, 0, 0,
							0, 0, 0,
							0, 0, 0);
	}
}

void autostab_node_t::track()
{
	image_node_t *inode = input_as<image_node_t>( 0);

	bool success = false;

	if( !inode)
		return;

	int start, end, ref_frame, tracked = 0;

	if( get_value<int>( param( "range")) == 1)
	{
		Imath::V2f r( get_value<Imath::V2f>( param( "custom_range")));
		start = r.x;
		end = r.y;
	}
	else
	{
		start = composition()->start_frame();
		end = composition()->end_frame();
	}

	if( start > end)
		return;

	QProgressDialog progress( "Tracking", "Cancel", 0, end - start + 1, (QWidget *) ui::user_interface_t::Instance().main_window());
	progress.setWindowModality( Qt::ApplicationModal);
	progress.setWindowTitle( "Tracking");
	progress.show();
		
	track_callback progress_cb( &progress, end - start + 1);
	cancel_callback cancel_cb( &progress);

	if( progress_cb( tracked))
		return;
	
	bool incremental = get_value<int>( param( "mode"));
	
	if( incremental)
		ref_frame = start;
	else
		ref_frame = get_value<float>( param( "ref_frame"));
	
	int use_alpha = get_value<int>( param( "alpha"));
	
	render::context_t context = composition()->current_context();
	render::context_guard_t guard( context, inode);

	context.result_node = inode;
	context.cancel = cancel_cb;

	const int max_features = 700;
	std::vector<cv::KeyPoint> ref_keypoints;
	cv::Mat ref_descriptors;

	try
	{
		if( progress_cb( tracked))
			return;
		
		context.frame = ref_frame;
				
		cv::Mat img, msk;
		get_frame( context, img, msk, use_alpha);
		detect_surf_features( img, msk, ref_keypoints, max_features);
		
		manip_ref_keyp = ref_keypoints;
		
		if( progress.wasCanceled())
			return;

		extract_surf_descriptors( img, ref_keypoints, ref_descriptors);
	}
	catch( cv::Exception& e)
	{
		std::cout << "OpenCV exception: " << e.what() << "\n";
		return;
	}

	++tracked;
	if( progress_cb( tracked))
		return;

	bool editing = false;
	
	for( int i = start; i <= end; ++i)
	{		
		if( i == ref_frame)
		{
			set_similarity_identity( i);
			set_perspective_identity( i);
			set_corner_pin_identity( i);
			continue;
		}

		std::vector<cv::KeyPoint> keypoints;
		cv::Mat descriptors;
		std::vector<cv::DMatch> matches;
		
		context.frame = i;
		
		try
		{
			if( progress_cb( tracked))
				goto cleanup;
			
			cv::Mat img, msk;
			get_frame( context, img, msk, use_alpha);

			if( progress_cb( tracked))
				goto cleanup;

			detect_surf_features( img, msk, keypoints, max_features);
			manip_keyp = keypoints;
		
			extract_surf_descriptors( img, keypoints, descriptors);

			if( progress_cb( tracked))
				goto cleanup;

			cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create( "BruteForce");
			cross_check_matching( matcher, ref_descriptors, descriptors, matches);

			if( !editing)
			{
				param_set().begin_edit();
				editing = true;
			}
			
			align_points( i, ref_keypoints, keypoints, matches);
		}
		catch( cv::Exception& e)
		{
			std::cout << "OpenCV exception: " << e.what() << "\n";
			progress.setValue( end - start + 1);
			goto cleanup;			
		}

		// do something here with the matches

		if( incremental)
		{
			ref_keypoints.swap( keypoints);
			ref_descriptors = descriptors;
			ref_frame = i;
			manip_ref_keyp = ref_keypoints;
		}

		ui::viewer_t::Instance().update();
		
		++tracked;
		if( progress_cb( tracked))
			goto cleanup;		
	}
	
	success = true;
	
cleanup:

	if( editing)
		param_set().end_edit( false);
}

void autostab_node_t::align_points( int frame, const std::vector<cv::KeyPoint>& ref_pts, 
									const std::vector<cv::KeyPoint>& pts, const std::vector<cv::DMatch>& matches)
{
	std::vector<cv::Point2d> to, from;

	for( int j = 0; j < matches.size(); ++j)
	{
		int indx0 = matches[j].queryIdx;
		int indx1 = matches[j].trainIdx;

		cv::Point2d to_pt( ref_pts[indx0].pt.x, ref_pts[indx0].pt.y);
		cv::Point2d from_pt( pts[indx1].pt.x, pts[indx1].pt.y);

		from.push_back( from_pt);
		to.push_back( to_pt);
	}
	
	// affine
	{
		boost::optional<Imath::M33d> m = find_similarity( from, to);

		if( m)
			set_similarity_params( frame, m.get());
		else
			set_similarity_identity( frame);
	}

	// projective
	{
		boost::optional<Imath::M33d> m = find_homography( from, to);

		if( m)
		{
			set_perspective_params( frame, m.get());
			set_corner_pin_params( frame, m.get());
		}
		else
		{
			set_perspective_identity( frame);
			set_corner_pin_identity( frame);
		}
	}
}

image_node_t *create_autostab_node() { return new autostab_node_t();}

const node_metaclass_t *autostab_node_t::metaclass() const { return &autostab_node_metaclass();}

const node_metaclass_t& autostab_node_t::autostab_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.autostab";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Future";
        info.menu_item = "Auto Stabilizer";
        info.create = &create_autostab_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::Instance().register_node( autostab_node_t::autostab_node_metaclass());

} // namespace
} // namespace
