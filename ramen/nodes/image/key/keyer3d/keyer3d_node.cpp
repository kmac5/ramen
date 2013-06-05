// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/key/keyer3d/keyer3d_node.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/gil/extension/algorithm/tbb/tbb_transform.hpp>
#include<ramen/gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/nodes/image/key/keyer3d/toolbar.hpp>
#include<ramen/nodes/image/key/keyer3d/manipulator.hpp>
#include<ramen/nodes/image/key/keyer3d/keyer3d_commands.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace image
{

namespace
{

struct key3d_fun
{
    key3d_fun( const keyer3d::km_keyer_t& keyer) : keyer_( keyer) {}

    image::pixel_t operator()( const image::pixel_t& p) const
    {
		static image::pixel_t one( 1, 1, 1, 1);
		return (*this)( p, one);
    }

    image::pixel_t operator()( const image::pixel_t& p, const image::pixel_t& msk) const
    {
		Imath::Color3f col( boost::gil::get_color( p, boost::gil::red_t()),
							boost::gil::get_color( p, boost::gil::green_t()),
							boost::gil::get_color( p, boost::gil::blue_t()));
		
		float a = keyer_.calc_alpha( col);
		float m = boost::gil::get_color( msk, boost::gil::alpha_t());
		return image::pixel_t( col.x, col.y, col.z, a * m);
    }
	
private:

	const keyer3d::km_keyer_t& keyer_;
};

} // unnamed

const int keyer3d_node_t::max_clusters;

keyer3d_node_t::keyer3d_node_t() : keyer_node_t(), toolbar_( 0), keyer_( max_clusters)
{
    set_name("keyer3d");
	
	#ifndef NDEBUG
		knum_ = 0;
		for( int i = 0; i < max_clusters; ++i)
		{
			kcenter_[i] = 0;
			ktol_[i] = 0;
			ksoft_[i] = 0;
		}
	#endif
}

keyer3d_node_t::keyer3d_node_t( const keyer3d_node_t& other) : keyer_node_t( other), keyer_( other.keyer_), toolbar_( 0)
{

	#ifndef NDEBUG
		knum_ = 0;
		for( int i = 0; i < max_clusters; ++i)
		{
			kcenter_[i] = 0;
			ktol_[i] = 0;
			ksoft_[i] = 0;
		}
	#endif
}

void keyer3d_node_t::do_create_params()
{	
	std::auto_ptr<float_param_t> p( new float_param_t( "Global Tol"));
	p->set_id( "gtol");
	p->set_default_value( 1);
	p->set_range( 0, 10);
	p->set_step( 0.025);
	p->set_static( true);
	add_param( p);

	p.reset( new float_param_t( "Global Soft"));
	p->set_id( "gsoft");
	p->set_default_value( 1);
	p->set_range( 0, 10);
	p->set_step( 0.025);
	p->set_static( true);
	add_param( p);
	
	#ifndef NDEBUG
		// secret params
		{
			std::auto_ptr<float_param_t> p( new float_param_t( "k"));
			p->set_id( "k");
			p->set_default_value( 0);
			p->set_range( 0, max_clusters);
			p->set_round_to_int( true);
			//p->set_secret( true);
			p->set_enabled( false);
			p->set_static( true);
			p->set_can_undo( false);
			p->set_persist( false);
			p->set_include_in_hash( false);

			knum_ = p.get();
			add_param( p);
		
			for( int i = 0; i < max_clusters; ++i)
			  create_cluster_params( i);
		}
	#endif
}

void keyer3d_node_t::do_create_manipulators()
{
	std::auto_ptr<manipulator_t> m( new keyer3d::manipulator_t());
	add_manipulator( m);
}

std::auto_ptr<QWidget> keyer3d_node_t::create_toolbar()
{
	RAMEN_ASSERT( !toolbar_);
		
	toolbar_ = new keyer3d::toolbar_t( *this);
	return std::auto_ptr<QWidget>( toolbar_);
}

void keyer3d_node_t::do_connected( node_t *src, int port)
{
	if( port == 0)
		get_input_frame();
}

void keyer3d_node_t::do_begin_active()	{ get_input_frame();}
void keyer3d_node_t::do_end_active()	{ free_input_frame();}

void keyer3d_node_t::do_calc_hash_str( const render::context_t& context)
{
	image_node_t::do_calc_hash_str( context);
	keyer().add_to_hash_str( hash_generator());
}

void keyer3d_node_t::do_calc_bounds( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
    if( input( 1) && !is_active())
		set_bounds( ImathExt::intersect( in0->bounds(), input_as<image_node_t>( 1)->bounds()));
    else
		set_bounds( in0->bounds());
}

void keyer3d_node_t::do_calc_defined( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
    if( input( 1) && !is_active())
    {
		Imath::Box2i def( ImathExt::intersect( in0->defined(), input_as<image_node_t>( 1)->defined()));
		set_defined( def);
    }
    else
		set_defined( in0->defined());
}

void keyer3d_node_t::do_process( const render::context_t& context)
{
	image_node_t *in0 = input_as<image_node_t>( 0);
	
	float gtol = get_value<float>( param( "gtol"));
	float gsoft = get_value<float>( param( "gsoft"));
	
	keyer().set_tol_soft_factors( gtol, gsoft);
	
    if( !input(1))
    {
		Imath::Box2i area( ImathExt::intersect( defined(), in0->defined()));

		if( area.isEmpty())
		    return;

		boost::gil::tbb_transform_pixels(  in0->const_subimage_view( area),
											subimage_view( area), key3d_fun( keyer()));
    }
    else
    {
		image_node_t *in1 = input_as<image_node_t>( 1);
		
		Imath::Box2i area( ImathExt::intersect( ImathExt::intersect( defined(), in0->defined()), in1->defined()));

		if( area.isEmpty())
		    return;

		boost::gil::tbb_transform2_pixels(  in0->const_subimage_view( area),
											in1->const_subimage_view( area),
											subimage_view( area), key3d_fun( keyer()));
    }
}

void keyer3d_node_t::do_read( const serialization::yaml_node_t& node, const std::pair<int,int>& version)
{ 
	keyer().read( node);
	update_clusters_params();
}

void keyer3d_node_t::do_write( serialization::yaml_oarchive_t& out) const { keyer().write( out);}

void keyer3d_node_t::create_cluster_params( int i)
{
	#ifndef NDEBUG
		RAMEN_ASSERT( i < max_clusters);
		
		std::string kcenter( "k0_center");
		std::string ktol( "k0_tol");
		std::string ksoft( "k0_soft");
	
		kcenter[1] = i + '0';
		std::auto_ptr<color_param_t> c( new color_param_t( kcenter));
		c->set_id( kcenter);
		c->set_default_value( Imath::Color4f( 0, 0, 0, 0));
		c->set_is_rgba( false);
		c->set_static( true);
		//c->set_secret( true);
		c->set_enabled( false);
		c->set_can_undo( false);
		c->set_persist( false);
		c->set_include_in_hash( false);		
		kcenter_[i] = c.get();
		add_param( c);
			
		ktol[1] = i + '0';
		std::auto_ptr<float_param_t> f( new float_param_t( ktol));
		f->set_id( ktol);
		f->set_default_value( 0);
		f->set_min( 0);
		f->set_static( true);
		//f->set_secret( true);
		f->set_enabled( false);
		f->set_can_undo( false);
		f->set_persist( false);
		f->set_include_in_hash( false);		
		ktol_[i] = f.get();
		add_param( f);		
	
		ksoft[1] = i + '0';
		f.reset( new float_param_t( ksoft));
		f->set_id( ksoft);
		f->set_default_value( 0);
		f->set_min( 0);
		f->set_static( true);
		//f->set_secret( true);
		f->set_enabled( false);
		f->set_can_undo( false);
		f->set_persist( false);
		f->set_include_in_hash( false);
		ksoft_[i] = f.get();
		add_param( f);
	#endif
}

void keyer3d_node_t::update_clusters_params()
{
	#ifndef NDEBUG
		knum_->set_value( keyer().clusters().size());
		knum_->update_widgets();
		
		for( int i = 0; i < keyer().clusters().size(); ++i)
		{
			Imath::Color3f col( keyer().clusters()[i].center);
			kcenter_[i]->set_value( Imath::Color4f( col.x, col.y, col.z, 1));
			ktol_[i]->set_value( keyer().clusters()[i].r_tol);
			ksoft_[i]->set_value( keyer().clusters()[i].r_soft);
	
			kcenter_[i]->update_widgets();
			ktol_[i]->update_widgets();
			ksoft_[i]->update_widgets();
		}
	#endif
}

bool keyer3d_node_t::add_tolerance( const Imath::Color3f& col)
{
	float a = keyer().calc_alpha( col);

	if( a > 0.0f)
	{
		keyer().add_tolerance( col);
		update_clusters_params();
		return true;
	}
	
	return false;
}

bool keyer3d_node_t::add_tolerance( const std::vector<Imath::Color3f>& s)
{
	bool any_change = false;
	
	BOOST_FOREACH( const Imath::Color3f& col, s)
	{
		float a = keyer().calc_alpha( col);
	
		if( a > 0.0f)
		{
			keyer().add_tolerance( col);
			any_change = true;
		}
	}
	
	if( any_change)
		update_clusters_params();
	
	return any_change;
}

bool keyer3d_node_t::remove_tolerance( const Imath::Color3f& col)
{
	float a = keyer().calc_alpha( col);

	if( a < 1.0f)
	{
		keyer().remove_tolerance( col);
		update_clusters_params();
		return true;
	}
	
	return false;
}

bool keyer3d_node_t::remove_tolerance( const std::vector<Imath::Color3f>& s)
{
	bool any_change = false;
	
	BOOST_FOREACH( const Imath::Color3f& col, s)
	{
		float a = keyer().calc_alpha( col);
	
		if( a < 1.0f)
		{
			keyer().remove_tolerance( col);
			any_change = true;
		}
	}
	
	if( any_change)
		update_clusters_params();
	
	return any_change;
}

void keyer3d_node_t::add_softness( const Imath::Color3f& col, float amount)
{
	keyer().add_softness( col, amount);
	update_clusters_params();
}

// factory

node_t *create_keyer3d_node() { return new keyer3d_node_t();}

const node_metaclass_t *keyer3d_node_t::metaclass() const { return &keyer3d_node_metaclass();}

const node_metaclass_t& keyer3d_node_t::keyer3d_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.keyer3d";
        info.major_version = 1;
        info.minor_version = 0;
		info.menu = "Image";
        info.submenu = "Future";
        info.menu_item = "3D Keyer";
        info.create = &create_keyer3d_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( keyer3d_node_t::keyer3d_node_metaclass());

} // namespace
} // namespace
