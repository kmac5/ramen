// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_POLYSCAN_POLY_HPP
#define RAMEN_POLYSCAN_POLY_HPP

#include<boost/static_assert.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/assert.hpp>

#include<ramen/polyscan/vertex.hpp>

#define RAMEN_MAX_POLY_VERTS 10

namespace ramen
{
namespace polyscan
{

class poly_t
{
public:

	poly_t();

	int size() const { return size_;}

	bool empty() const { return size_ == 0;}

	void clear() { size_ = 0;}

	const vertex_t& operator[]( int index) const;
	vertex_t& operator[]( int index);

	const vertex_t& front() const;
	vertex_t& front();

	const vertex_t& back() const;
	vertex_t& back();

	void add_vertex();
	void push_back( const vertex_t& v);

	int topmost_vertex() const;

	Imath::Box3f screen_bbox() const;

	void perspective_divide();

private:

	BOOST_STATIC_ASSERT(( sizeof( vertex_t) % 16 == 0));

	int size_;
	vertex_t verts_[RAMEN_MAX_POLY_VERTS];
};

} // namespace
} // namespace

#endif
