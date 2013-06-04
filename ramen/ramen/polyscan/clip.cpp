// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/polyscan/clip.hpp>

#include<algorithm>

#include<ramen/assert.hpp>

namespace ramen
{
namespace polyscan
{
namespace
{

void clip_to_halfspace( poly_t *p, poly_t *q, int index, float sign, float k)
{
	q->clear();

	vertex_t *u = &( p->back());
	float tu = sign * u->screen[index] - u->screen.w * k;

	vertex_t *v = &( p->front());
	float tv;

	for( int i = p->size(); i > 0; --i)
	{
		tv = sign * v->screen[index] - v->screen.w * k;

		if( tu <= 0 ^ tv <= 0)
		{
			float t = tu / ( tu - tv);
			q->add_vertex();
			q->back().interpolate( *u, *v, t);
		}

		if( tv <= 0)
			q->push_back( *v);

		u = v;
		tu = tv;
		++v;
	}
}

} // unnamed

clip_result_t clip( poly_t& poly, const Imath::Box3f& box)
{
	RAMEN_ASSERT( poly.size() + 6 <= RAMEN_MAX_POLY_VERTS);

    int x0out = 0, x1out = 0, y0out = 0, y1out = 0, z0out = 0, z1out = 0;

	for( int i = 0, ie = poly.size(); i < ie; ++i)
	{
		if( poly[i].screen.x < box.min.x * poly[i].screen.w)
			++x0out;

		if( poly[i].screen.x > box.max.x * poly[i].screen.w)
			++x1out;

		if( poly[i].screen.y < box.min.y * poly[i].screen.w)
			++y0out;

		if( poly[i].screen.y > box.max.y * poly[i].screen.w)
			++y1out;

		if( poly[i].screen.z < box.min.z * poly[i].screen.w)
			++z0out;

		if( poly[i].screen.z > box.max.z * poly[i].screen.w)
			++z1out;
	}

    if( x0out+x1out+y0out+y1out+z0out+z1out == 0)
		return poly_clip_in;

    if( x0out==poly.size() || x1out==poly.size() || y0out==poly.size() || y1out==poly.size() || z0out==poly.size() || z1out==poly.size())
	{
	    poly.clear();
	    return poly_clip_out;
	}

	poly_t tmp;
	poly_t *p = &poly;
	poly_t *q = &tmp;

	if( x0out)
	{
		clip_to_halfspace( p, q, 0, -1, -box.min.x);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( x1out)
	{
		clip_to_halfspace( p, q, 0, 1, box.max.x);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( y0out)
	{
		clip_to_halfspace( p, q, 1, -1, -box.min.y);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( y1out)
	{
		clip_to_halfspace( p, q, 1, 1, box.max.y);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( z0out)
	{
		clip_to_halfspace( p, q, 2, -1, -box.min.z);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( z1out)
	{
		clip_to_halfspace( p, q, 2, 1, box.max.z);

		if( q->empty())
		{
			p->clear();
			return poly_clip_out;
		}

		std::swap( p, q);
	}

	if( p == &tmp)
		*q = tmp;

	return poly_clip_partial;
}

} // namespace
} // namespace
