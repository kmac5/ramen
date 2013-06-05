// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/xnodes/distortx_node.hpp>

#include<boost/bind.hpp>

#include<ramen/params/se_expr_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/string_param.hpp>

#include<ramen/image/processing.hpp>
#include<ramen/image/color_bars.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum
{
    border_black  = 0,
    border_tile,
    border_mirror
};

struct se_expr_warp_fun
{
	se_expr_warp_fun( distortx_node_t *node, const std::string str,
					  const Imath::V2f max_amplitude, const render::context_t& context) : expr_( str, node, base_expr_node_t::warp_context)
	{
		RAMEN_ASSERT( node);
		node_ = node;
		format_ = node_->format();
		max_amplitude_ = max_amplitude;
		context_ = context;
		init_expression();
	}

	se_expr_warp_fun( const se_expr_warp_fun& other) : expr_( other.string(), other.node_, base_expr_node_t::warp_context)
	{
		node_ = other.node_;
		format_ = node_->format();
		max_amplitude_ = other.max_amplitude_;
		context_ = other.context_;
		init_expression();
	}

    Imath::V2f operator()( const Imath::V2f& p) const
	{
		Imath::V2f q( apply_expression( p));
		return Imath::V2f( Imath::clamp( q.x, p.x - max_amplitude_.x, p.x + max_amplitude_.x),
						   Imath::clamp( q.y, p.y - max_amplitude_.y, p.y + max_amplitude_.y));
	}

private:

	const std::string& string() const { return expr_.getExpr();}

	void init_expression()
	{
		expr_.setup_variables( node_, context_);
		xvar_ = &expr_.vars["x"].val;
		yvar_ = &expr_.vars["y"].val;
		uvar_ = &expr_.vars["u"].val;
		vvar_ = &expr_.vars["v"].val;
	}

	Imath::V2f apply_expression( const Imath::V2f& p) const
	{
		Imath::V2f q( image_to_uv( p));

		*xvar_ = p.x;
		*yvar_ = p.y;
		*uvar_ = q.x;
		*vvar_ = q.y;

		SeVec3d result = expr_.evaluate();
		q = Imath::V2f( result[0], result[1]);
		return uv_to_image( q);
	}

	Imath::V2f image_to_uv( const Imath::V2f& p) const
	{
		return Imath::V2f( ( p.x - format_.min.x) / ( float) format_.size().x,
						   ( p.y - format_.min.y) / ( float) format_.size().y);
	}

	Imath::V2f uv_to_image( const Imath::V2f& p) const
	{
		return Imath::V2f( p.x * format_.size().x + format_.min.x,
						   p.y * format_.size().y + format_.min.y);
	}

	distortx_node_t *node_;
	Imath::Box2i format_;
	Imath::V2f max_amplitude_;
	base_expr_node_t::image_expression_t expr_;
	render::context_t context_;

	double *xvar_, *yvar_;
	double *uvar_, *vvar_;
};

struct mask_fun
{
    mask_fun( const image::const_image_view_t& mask, const Imath::Box2i& area) : mask_( mask), area_( area) {}

    float operator()( const Imath::V2f& p) const
    {
        using namespace boost::gil;

        if( ( p.x >= area_.min.x) && ( p.x <= area_.max.x))
        {
            if( ( p.y >= area_.min.y) && ( p.y <= area_.max.y))
                return get_color( mask_( p.x - area_.min.x, p.y - area_.min.y), alpha_t());
        }

        return 0.0f;
    }

private:

    const image::const_image_view_t& mask_;
    Imath::Box2i area_;
};

template<class WarpFun, class MaskFun>
struct masked_warp_fun
{
public:

    masked_warp_fun( WarpFun f, MaskFun mf) : f_( f), mf_( mf) {}

    Imath::V2f operator()( const Imath::V2f& p) const
    {
        float mask = mf_( p);

        if( mask != 0.0f)
        {
            Imath::V2f q( f_( p));
            return Imath::lerp( p, q, mask);
        }

        return p;
    }

private:

    WarpFun f_;
    MaskFun mf_;
};

} // unnamed

distortx_node_t::distortx_node_t() : base_expr_node_t()
{
    set_name( "distortx");
    add_input_plug( "front", false, ui::palette_t::instance().color("front plug"), "Front");
    add_input_plug( "mask", true, ui::palette_t::instance().color("matte plug"), "Mask");
}

distortx_node_t::distortx_node_t( const distortx_node_t& other) : base_expr_node_t( other)
{
	create_expression();
}

void distortx_node_t::do_create_params()
{
	std::auto_ptr<se_expr_param_t> s( new se_expr_param_t( "Warp Expr", boost::bind( &base_expr_node_t::make_expression, this)));
	s->set_id( expr_param_name());
	s->set_default_value( "[$u, $v, 0]");
	add_param( s);
	
	std::auto_ptr<float2_param_t> q( new float2_param_t( "Amplitude"));
	q->set_id( "amplitude");
	q->set_proportional( true);
	q->set_min( 0);
	q->set_default_value( Imath::V2f( 0, 0));
	q->set_step( 0.05f);
	add_param( q);

    std::auto_ptr<popup_param_t> r( new popup_param_t( "Borders"));
    r->set_id( "borders");
    r->menu_items() = boost::assign::list_of( "Black")( "Tile")( "Mirror");
    add_param( r);

	create_local_var_params();
	create_expression();
}

Imath::V2f distortx_node_t::warp_amplitude() const
{
	return get_value<Imath::V2f>( param( "amplitude"));
}

void distortx_node_t::do_calc_bounds( const render::context_t& context)
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));
    Imath::Box2i b( input_as<image_node_t>( 0)->bounds());

    if( !b.isEmpty())
    {
        b.min.x -= amplitude.x;
        b.min.y -= amplitude.y;
        b.max.x += amplitude.x;
        b.max.y += amplitude.y;
        
        if( input( 1))
        {
            Imath::Box2i b2( input_as<image_node_t>( 1)->bounds());
            b2.min.x -= amplitude.x;
            b2.min.y -= amplitude.y;
            b2.max.x += amplitude.x;
            b2.max.y += amplitude.y;
            b2.extendBy( input_as<image_node_t>( 0)->bounds());
            b = ImathExt::intersect( b, b2);
        }
    }

    set_bounds( b);
}

void distortx_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    Imath::V2f amplitude = get_value<Imath::V2f>( param( "amplitude"));

    Imath::Box2i roi( interest());
    
    if( input( 1))
        input_as<image_node_t>( 1)->add_interest( roi);

	image_node_t *in = input_as<image_node_t>( 0);
	
	if( get_value<int>( param( "borders")) == border_black)
	{
	    roi.min.x -= amplitude.x;
	    roi.min.y -= amplitude.y;
	    roi.max.x += amplitude.x;
	    roi.max.y += amplitude.y;
	    in->add_interest( roi);
	}
	else
		in->add_interest( in->format());
}

void distortx_node_t::do_process( const render::context_t& context)
{
	if( !expr_.get() || !expr_->isValid())
	{
		image::make_color_bars( image_view());
		return;
	}

	bool thread_safe = true; // while testing
	//bool thread_safe = expr_->isThreadSafe();

	std::string warp_expr = get_value<std::string>( param( expr_param_name()));
	Imath::V2f amp( get_value<Imath::V2f>( param( "amplitude")));
	amp /= context.subsample;

	se_expr_warp_fun f( this, warp_expr, amp, context);

    image_node_t *in  = input_as<image_node_t>( 0);
    image_node_t *msk = input_as<image_node_t>( 1);

    if( msk)
    {
        mask_fun m( msk->const_image_view(), msk->defined());
        masked_warp_fun<se_expr_warp_fun, mask_fun> mf( f, m);

        switch( get_value<int>( param( "borders")))
        {
			case border_black:
				image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, thread_safe);
			break;
	
			case border_tile:
				image::warp_bilinear_tile( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, thread_safe);
			break;
	
			case border_mirror:
				image::warp_bilinear_mirror( in->defined(), in->const_image_view(), defined(), image_view(), mf, false, thread_safe);
			break;
        }
    }
    else
    {
        switch( get_value<int>( param( "borders")))
        {
			case border_black:
				image::warp_bilinear( in->defined(), in->const_image_view(), defined(), image_view(), f, false, thread_safe);
			break;
	
			case border_tile:
				image::warp_bilinear_tile( in->defined(), in->const_image_view(), defined(), image_view(), f, false, thread_safe);
			break;
	
			case border_mirror:
				image::warp_bilinear_mirror( in->defined(), in->const_image_view(), defined(), image_view(), f, false, thread_safe);
			break;
        }
    }
}

expressions::se_expression_t *distortx_node_t::make_expression() const
{
	std::string warp_expr = get_value<std::string>( param( expr_param_name()));
	return new image_expression_t( warp_expr, this, warp_context);
}

// factory
node_t *create_distortx_node() { return new distortx_node_t();}

const node_metaclass_t *distortx_node_t::metaclass() const { return &distortx_node_metaclass();}

const node_metaclass_t& distortx_node_t::distortx_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
		info.id = "image.builtin.distortx";
		info.major_version = 1;
		info.minor_version = 0;
		info.menu = "Image";
		info.submenu = "Distort";
		info.menu_item = "DistortX";
        info.help = "Distort the input image, using a SeExpr expression.";
		info.create = &create_distortx_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( distortx_node_t::distortx_node_metaclass());

} // namespace
} // namespace
