// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/clipboard.hpp>

#include<algorithm>
#include<limits>

#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

namespace ramen
{
namespace anim
{
namespace
{
		
struct named_curves_less
{
	bool operator()( const clipboard_t::named_curve_type& a, const clipboard_t::named_curve_type& b) const
	{ 
		return a.first < b.first;
	}
};
	
template<class Curve>
void do_copy_keys( const Curve& src, Curve& dst)
{
	RAMEN_ASSERT( dst.empty());
	dst.set_extrapolation( src.extrapolation());
	
	for( int i = 0; i < src.size(); ++i)
	{
		if( !src[i].selected())
			continue;

		dst.insert( src[i]);
	}
}

void do_copy_keys( const anim::float_curve_t& src, anim::float_curve_t& dst)
{
	RAMEN_ASSERT( dst.empty());
	dst.set_extrapolation( src.extrapolation());
	dst.set_default_auto_tangents( src.default_auto_tangents());
	dst.set_scale( src.scale());
	dst.set_offset( src.offset());
	dst.set_range( src.get_min(), src.get_max());
	
	for( int i = 0; i < src.size(); ++i)
	{
		if( !src[i].selected())
			continue;

		dst.insert( src[i]);
	}
}

template<class Curve>
void do_copy_curve( const Curve& src, Curve& dst) { dst = src;}

template<class Curve>
void paste_keys( const Curve& src, Curve& dst, float offset)
{	
	for( int i = 0; i < src.size(); ++i)
	{
		typename Curve::key_type key( src[i]);
		key.set_time( key.time() + offset);
		dst.insert( key);
	}
}

void paste_keys( const anim::float_curve_t& src, anim::float_curve_t& dst, float offset)
{
	for( int i = 0; i < src.size(); ++i)
	{
		anim::float_key_t key( src[i]);
		key.set_time( key.time() + offset);
		float val = key.value();
		//val = src.relative_to_absolute( val);
		//val = dst.absolute_to_relative( val);
		key.set_value( val);
		dst.insert( key);
	}
}

/***************************************************************************/

struct copy_keys_visitor : public boost::static_visitor<>
{
	copy_keys_visitor( boost::shared_ptr<any_curve_t>& dst) : dst_( dst) {}

	template<class Curve>
	void operator()( const Curve *src)
	{
		dst_.reset( new any_curve_t( Curve()));
		do_copy_keys( *src, boost::get<Curve>( *dst_));
	}
	
private:
	
	boost::shared_ptr<any_curve_t>& dst_;
};

/***************************************************************************/

struct copy_curve_visitor : public boost::static_visitor<>
{
	copy_curve_visitor( boost::shared_ptr<any_curve_t>& dst) : dst_( dst) {}

	template<class Curve>
	void operator()( const Curve *src)
	{
		dst_.reset( new any_curve_t( Curve()));
		do_copy_curve( *src, boost::get<Curve>( *dst_));
	}
	
private:
	
	boost::shared_ptr<any_curve_t>& dst_;
};

/***************************************************************************/

struct get_first_key_time_visitor : public boost::static_visitor<>
{
	get_first_key_time_visitor() : frame( 1) {}

	template<class Curve>
	void operator()( const Curve& src) { frame = src.start_time();}
	
	float frame;
};

/***************************************************************************/

struct can_paste_visitor : public boost::static_visitor<>
{
	can_paste_visitor( any_curve_t& src, bool paste_curve_mode) : src_( src), paste_curve_mode_( paste_curve_mode)
	{
		result = false;
	}

	void operator()( const float_curve_t *c) const
	{ 
		const float_curve_t *scurve = boost::get<const float_curve_t>( &src_);
		result = scurve;
	}

	void operator()( const shape_curve2f_t *c) const
	{
		const shape_curve2f_t *scurve = boost::get<const shape_curve2f_t>( &src_);
		
		if( !scurve)
			return;
		
		if( c->empty() || paste_curve_mode_)
		{
			result = true;
			return;
		}
		
		result = ( scurve->keys().front().size() == c->keys().front().size());
	}
	
	mutable bool result;

private:

	const any_curve_t& src_;
	bool paste_curve_mode_;
};

/***************************************************************************/

struct paste_keys_visitor : public boost::static_visitor<>
{
	paste_keys_visitor( any_curve_t& src, float offset) : src_( src)
	{
		offset_ = offset;
	}

	template<class Curve>
	void operator()( Curve *dst)
	{
		paste_keys( boost::get<const Curve>( src_), *dst, offset_);
	}
	
	void operator()( anim::float_curve_t *dst)
	{
		paste_keys( boost::get<const anim::float_curve_t>( src_), *dst, offset_);
		dst->recalc_tangents_and_coefficients();
	}
	
private:

	any_curve_t& src_;
	float offset_;
};

} // unnamed

clipboard_t& clipboard_t::instance()
{
    static clipboard_t c;
    return c;
}

clipboard_t::clipboard_t() : copy_curves_mode_( true), copying_( false) {}
clipboard_t::~clipboard_t() {}

bool clipboard_t::empty() const { return contents_.empty();}

void clipboard_t::clear() { contents_.clear();}

// anim editor

void clipboard_t::begin_copy()
{
	copying_ = true;
	clear();
}

void clipboard_t::copy_curve( const std::string& name, const anim::any_curve_ptr_t& c)
{
	RAMEN_ASSERT( copying_);
	
	copy_curves_mode_ = true;
	contents_.push_back( named_curve_type( name, boost::shared_ptr<any_curve_t>()));
	copy_curve_visitor v( contents_.back().second);
	boost::apply_visitor( v, c);
}

void clipboard_t::copy_keys( const std::string& name, const anim::any_curve_ptr_t& c)
{
	RAMEN_ASSERT( copying_);
	
	copy_curves_mode_ = false;
	contents_.push_back( named_curve_type( name, boost::shared_ptr<any_curve_t>()));
	copy_keys_visitor v( contents_.back().second);
	boost::apply_visitor( v, c);
}

void clipboard_t::end_copy()
{
	copying_ = false;
	std::sort( contents_.begin(), contents_.end(), named_curves_less());
}

bool clipboard_t::can_paste( const std::string& name, const anim::any_curve_ptr_t& c)
{
	return find_compatible_curve( name, c) != -1;
}

void clipboard_t::paste( const std::string& name, anim::any_curve_ptr_t& c, float frame)
{
	RAMEN_ASSERT( !copying_);
	
	int i = find_compatible_curve( name, c);
	RAMEN_ASSERT( i != -1);
	
	float offset = 0;

	if( !copy_curves_mode_)
	{
		get_first_key_time_visitor v;
		boost::apply_visitor( v, *contents_[i].second.get());
		offset = frame - v.frame;
	}
	
	paste_keys_visitor v( *contents_[i].second.get(), offset);
	boost::apply_visitor( v, c);
}

int clipboard_t::find_compatible_curve( const std::string& name, const anim::any_curve_ptr_t& c)
{
	if( empty())
		return -1;
	
	int min_distance = std::numeric_limits<int>::max();
	int best_index = -1;	
	
	for( int i = 0; i < contents_.size(); ++i)
	{
		if( contents_[i].second && contents_[i].second->which() == c.which())
		{
			can_paste_visitor v( *contents_[i].second.get(), copy_curves_mode_);
			boost::apply_visitor( v, c);
				
			if( v.result)
			{
				int dist = distance_fun_( name, contents_[i].first);
				
				if( dist < min_distance)
				{
					min_distance = dist;
					best_index = i;
				}
			}
		}
	}

	return best_index;
}

// param spinboxes
bool clipboard_t::can_paste()
{
	if( empty() || !contents_[0].second)
		return false;

	return boost::get<const float_curve_t>( contents_[0].second.get());
}

void clipboard_t::copy( const float_curve_t& c)
{
	RAMEN_ASSERT( copying_);
	
	boost::shared_ptr<any_curve_t> p( new any_curve_t( float_curve_t()));
	contents_.push_back( named_curve_type( "curve", p));
	do_copy_curve( c, *boost::get<float_curve_t>( contents_[0].second.get()));
	copy_curves_mode_ = true;
}

void clipboard_t::paste( float_curve_t& dst)
{
	RAMEN_ASSERT( !copying_);	
	RAMEN_ASSERT( !empty());

	paste_keys( *boost::get<float_curve_t>( contents_[0].second.get()), dst, 0);
	dst.recalc_tangents_and_coefficients();
}

} // namespace
} // namespace
