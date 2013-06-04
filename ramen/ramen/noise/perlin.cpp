// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#include<ramen/noise/perlin.hpp>

#include<cmath>

namespace ramen
{
namespace noise
{

perlin_t::perlin_t( int seed) { init( seed);}

float perlin_t::operator()( float x) const
{ 
	return remap( do_noise1( x));
}

float perlin_t::operator()( float x, float y) const
{
	float v[2];
	v[0] = x; v[1] = y;
	
	return remap( do_noise2( v));
}

float perlin_t::operator()( float x, float y, float z) const
{
	float v[3];
	v[0] = x; v[1] = y; v[2] = z;
	
	return remap( do_noise3( v));
}

float perlin_t::remap( float x) const { return x;}

#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

float perlin_t::do_noise1( float arg) const
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;

	setup(0, bx0,bx1, rx0,rx1);

	sx = smooth_step( rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return lerp(sx, u, v);
}

float perlin_t::do_noise2( float vec[2]) const
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, sx, sy, a, b, t, u, v;
	int i, j;
	const float *q;

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = smooth_step( rx0);
	sy = smooth_step( ry0);

	#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

float perlin_t::do_noise3( float vec[3]) const
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, sy, sz, a, b, c, d, t, u, v;
	const float *q;
	int i, j;

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = smooth_step( rx0);
	sy = smooth_step( ry0);
	sz = smooth_step( rz0);

	#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void perlin_t::normalize2(float v[2]) const
{
	float s = std::sqrt( (double) v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void perlin_t::normalize3(float v[3]) const
{
	float s = std::sqrt( (double) v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void perlin_t::init( int seed)
{
	Imath::Rand48 rng( seed);
	
	int i, j, k;

	for( i = 0 ; i < B ; i++)
	{
		p[i] = i;

		g1[i] = (float)(( rng.nexti() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)(( rng.nexti() % (B + B)) - B) / B;

		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)(( rng.nexti() % (B + B)) - B) / B;
		
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rng.nexti() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) 
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];

		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];

		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

} // noise
} // ramen
