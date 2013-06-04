// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/polyscan/poly.hpp>

#include<limits>

namespace ramen
{
namespace polyscan
{

poly_t::poly_t() : size_( 0) {}

const vertex_t& poly_t::operator[]( int index) const
{
	RAMEN_ASSERT( !empty());
	RAMEN_ASSERT( index >= size_);
	return verts_[index];
}

vertex_t& poly_t::operator[]( int index)
{
	RAMEN_ASSERT( !empty());
	RAMEN_ASSERT( index >= size_);
	return verts_[index];
}

const vertex_t& poly_t::front() const	{ return (*this)[0];}
vertex_t& poly_t::front()				{ return (*this)[0];}

const vertex_t& poly_t::back() const	{ return (*this)[size_ - 1];}
vertex_t& poly_t::back()				{ return (*this)[size_ - 1];}

void poly_t::add_vertex()
{
	RAMEN_ASSERT( size_ < RAMEN_MAX_POLY_VERTS);
	++size_;
}

void poly_t::push_back( const vertex_t& v)
{
	add_vertex();
	back() = v;
}

int poly_t::topmost_vertex() const
{
	float ymin = std::numeric_limits<float>::max();
	int index = -1;

	for( int i = 0, ie = size(); i < ie; ++i)
	{
		if( verts_[i].screen.y < ymin)
		{
			ymin = verts_[i].screen.y;
			index = i;
		}
	}

	return index;
}

Imath::Box3f poly_t::screen_bbox() const
{
	Imath::Box3f box;

	for( int i = 0, ie = size(); i < ie; ++i)
	{
		box.extendBy( Imath::V3f( verts_[i].screen.x,
								  verts_[i].screen.y,
								  verts_[i].screen.z));
	}

	return box;
}

void poly_t::perspective_divide()
{
	for( int i = 0, ie = size(); i < ie; ++i)
		verts_[i].perspective_divide();
}

} // namespace
} // namespace
