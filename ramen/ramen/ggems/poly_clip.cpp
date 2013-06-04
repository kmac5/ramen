/*
 * Generic Convex Polygon Scan Conversion and Clipping
 * by Paul Heckbert
 * from "Graphics Gems", Academic Press, 1990
 */

/*
 * poly_clip.c: homogeneous 3-D convex polygon clipper
 *
 * Paul Heckbert	1985, Dec 1989
 */

#include<assert.h>
#include<string.h>

#include"poly_scan.hpp"

#define SWAP(a, b, temp)	{temp = a; a = b; b = temp;}
#define COORD( vert, i) (( float *)(vert))[i]

#define CLIP_AND_SWAP(elem, sign, k, p, q, r) { \
    poly_clip_to_halfspace(p, q, &v->elem - (float *)v, sign, sign*k); \
    if (q->n==0) {p1->n = 0; return POLY_CLIP_OUT;} \
    SWAP(p, q, r); \
}

/*
 * poly_clip_to_box: Clip the convex polygon p1 to the screen space box
 * using the homogeneous screen coordinates (sx, sy, sz, sw) of each vertex,
 * testing if v->sx/v->sw > box->x0 and v->sx/v->sw < box->x1,
 * and similar tests for y and z, for each vertex v of the polygon.
 * If polygon is entirely inside box, then POLY_CLIP_IN is returned.
 * If polygon is entirely outside box, then POLY_CLIP_OUT is returned.
 * Otherwise, if the polygon is cut by the box, p1 is modified and
 * POLY_CLIP_PARTIAL is returned.
 *
 * Given an n-gon as input, clipping against 6 planes could generate an
 * (n+6)gon, so POLY_NMAX in poly.h must be big enough to allow that.
 */

int poly_clip_to_box( Poly *p1, Poly_box *box)
{
	assert( p1->n + 6 <= POLY_NMAX);
	assert( sizeof( Poly_vert) / sizeof( float) <= 32);
	
    int x0out = 0, x1out = 0, y0out = 0, y1out = 0, z0out = 0, z1out = 0;
    int i;
    Poly_vert *v;
    Poly p2, *p, *q, *r;

    /* count vertices "outside" with respect to each of the six planes */
    for (v = p1->vert, i = p1->n; i > 0; --i, ++v)
	{
		if( v->sx < box->x0*v->sw)
			x0out++;	/* out on left */
		
		if( v->sx > box->x1*v->sw)
			x1out++;	/* out on right */
		
		if( v->sy < box->y0*v->sw)
			y0out++;	/* out on top */
		
		if( v->sy > box->y1*v->sw)
			y1out++;	/* out on bottom */
		
		if( v->sz < box->z0*v->sw)
			z0out++;	/* out on near */
		
		if( v->sz > box->z1*v->sw)
			z1out++;	/* out on far */
    }

    /* check if all vertices inside */
    if( x0out+x1out+y0out+y1out+z0out+z1out == 0)
		return POLY_CLIP_IN;

    /* check if all vertices are "outside" any of the six planes */
    if( x0out==p1->n || x1out==p1->n || y0out==p1->n || y1out==p1->n || z0out==p1->n || z1out==p1->n)
	{
	    p1->n = 0;
	    return POLY_CLIP_OUT;
	}

    /*
     * now clip against each of the planes that might cut the polygon,
     * at each step toggling between polygons p1 and p2
     */
    p = p1;
    q = &p2;
	
    if( x0out)
		CLIP_AND_SWAP(sx, -1., box->x0, p, q, r);
	
    if( x1out)
		CLIP_AND_SWAP(sx,  1., box->x1, p, q, r);
	
    if( y0out)
		CLIP_AND_SWAP(sy, -1., box->y0, p, q, r);
	
    if( y1out)
		CLIP_AND_SWAP(sy,  1., box->y1, p, q, r);
	
    if( z0out) 
		CLIP_AND_SWAP(sz, -1., box->z0, p, q, r);
	
    if( z1out)
		CLIP_AND_SWAP(sz,  1., box->z1, p, q, r);

    /* if result ended up in p2 then copy it to p1 */
    if (p==&p2)
		bcopy( &p2, p1, sizeof(Poly)-(POLY_NMAX-p2.n)*sizeof(Poly_vert));
	
    return POLY_CLIP_PARTIAL;
}

/*
 * poly_clip_to_halfspace: clip convex polygon p against a plane,
 * copying the portion satisfying sign*s[index] < k*sw into q,
 * where s is a Poly_vert* cast as a float*.
 * index is an index into the array of floats at each vertex, such that
 * s[index] is sx, sy, or sz (screen space x, y, or z).
 * Thus, to clip against xmin, use
 *	poly_clip_to_halfspace(p, q, XINDEX, -1., -xmin);
 * and to clip against xmax, use
 *	poly_clip_to_halfspace(p, q, XINDEX,  1.,  xmax);
 */

void poly_clip_to_halfspace( Poly *p, Poly *q, int index, float sign, float k)
{
    unsigned long m;
    float *up, *vp, *wp;
    Poly_vert *v;
    int i;
    Poly_vert *u;
    float t, tu, tv;

    q->n = 0;
    q->mask = p->mask;

    /* start with u=vert[n-1], v=vert[0] */
    u = &p->vert[p->n-1];
    tu = sign*COORD(u, index) - u->sw*k;

    for (v= &p->vert[0], i=p->n; i>0; i--, u=v, tu=tv, v++)
	{
		/* on old polygon (p), u is previous vertex, v is current vertex */
		/* tv is negative if vertex v is in */
		tv = sign*COORD(v, index) - v->sw*k;
		
		if (tu<=0. ^ tv<=0.)
		{
			/* edge crosses plane; add intersection point to q */
			t = tu/(tu-tv);
			up = (float *)u;
			vp = (float *)v;
			wp = (float *)&q->vert[q->n];
			
			for( m=p->mask; m!=0; m>>=1, up++, vp++, wp++)
			{
				if( m & 1)
					*wp = *up + t * ( *vp - *up);
			}

			q->n++;
		}

		if (tv<=0.)		/* vertex v is in, copy it to q */
			q->vert[q->n++] = *v;
	}
}

/*
 * poly_clip_near_far: Clip the convex polygon p1 against znear and zfar
 */

int poly_clip_near_far( Poly *p1, float znear, float zfar)
{
	assert( p1->n + 2 <= POLY_NMAX);
	assert( sizeof( Poly_vert) / sizeof( float) <= 32);
	
    int z0out = 0, z1out = 0;
    int i;
    Poly_vert *v;
    Poly p2, *p, *q, *r;

    /* count vertices "outside" with respect to each of the six planes */
    for( v = p1->vert, i = p1->n; i > 0; --i, ++v)
	{
		if( v->sz < znear * v->sw)
			z0out++;	/* out on near */
		
		if( v->sz > zfar * v->sw)
			z1out++;	/* out on far */
    }

    /* check if all vertices inside */
    if( z0out + z1out == 0)
		return POLY_CLIP_IN;

    /* check if all vertices are "outside" any of the six planes */
    if( z0out == p1->n || z1out == p1->n)
	{
	    p1->n = 0;
	    return POLY_CLIP_OUT;
	}

    /*
     * now clip against each of the planes that might cut the polygon,
     * at each step toggling between polygons p1 and p2
     */
    p = p1;
    q = &p2;
		
    if( z0out) 
		CLIP_AND_SWAP(sz, -1., znear, p, q, r);
	
    if( z1out)
		CLIP_AND_SWAP(sz,  1., zfar, p, q, r);

    /* if result ended up in p2 then copy it to p1 */
    if( p==&p2)
		bcopy( &p2, p1, sizeof(Poly)-(POLY_NMAX-p2.n)*sizeof(Poly_vert));

    return POLY_CLIP_PARTIAL;
}


int poly_clip_y( Poly *p1, float ymin, float ymax)
{
	assert( p1->n + 6 <= POLY_NMAX);
	assert( sizeof( Poly_vert) / sizeof( float) <= 32);
	
    int y0out = 0, y1out = 0;
    int i;
    Poly_vert *v;
    Poly p2, *p, *q, *r;

    /* count vertices "outside" with respect to each of the six planes */
    for (v = p1->vert, i = p1->n; i > 0; --i, ++v)
	{
		if( v->sy < ymin * v->sw)
			y0out++;	/* out on top */
		
		if( v->sy > ymax * v->sw)
			y1out++;	/* out on bottom */		
    }

    /* check if all vertices inside */
    if( y0out+ y1out == 0)
		return POLY_CLIP_IN;

    /* check if all vertices are "outside" any of the six planes */
    if( y0out==p1->n || y1out==p1->n)
	{
	    p1->n = 0;
	    return POLY_CLIP_OUT;
	}

    /*
     * now clip against each of the planes that might cut the polygon,
     * at each step toggling between polygons p1 and p2
     */
    p = p1;
    q = &p2;
	
    if( y0out)
		CLIP_AND_SWAP(sy, -1., ymin, p, q, r);
	
    if( y1out)
		CLIP_AND_SWAP(sy,  1., ymax, p, q, r);
	
    /* if result ended up in p2 then copy it to p1 */
    if (p==&p2)
		bcopy( &p2, p1, sizeof(Poly)-(POLY_NMAX-p2.n)*sizeof(Poly_vert));
	
    return POLY_CLIP_PARTIAL;
}

int poly_clip_xy( Poly *p1, float xmin, float ymin, float xmax, float ymax)
{
	assert( p1->n + 6 <= POLY_NMAX);
	assert( sizeof( Poly_vert) / sizeof( float) <= 32);
	
    int x0out = 0, x1out = 0, y0out = 0, y1out = 0;
    int i;
    Poly_vert *v;
    Poly p2, *p, *q, *r;

    /* count vertices "outside" with respect to each of the six planes */
    for (v = p1->vert, i = p1->n; i > 0; --i, ++v)
	{
		if( v->sx < xmin * v->sw)
			x0out++;	/* out on left */
		
		if( v->sx > xmax * v->sw)
			x1out++;	/* out on right */
		
		if( v->sy < ymin * v->sw)
			y0out++;	/* out on top */
		
		if( v->sy > ymax * v->sw)
			y1out++;	/* out on bottom */
    }

    /* check if all vertices inside */
    if( x0out + x1out + y0out + y1out == 0)
		return POLY_CLIP_IN;

    /* check if all vertices are "outside" any of the six planes */
    if( x0out==p1->n || x1out==p1->n || y0out==p1->n || y1out==p1->n)
	{
	    p1->n = 0;
	    return POLY_CLIP_OUT;
	}

    /*
     * now clip against each of the planes that might cut the polygon,
     * at each step toggling between polygons p1 and p2
     */
    p = p1;
    q = &p2;
	
    if( x0out)
		CLIP_AND_SWAP(sx, -1., xmin, p, q, r);
	
    if( x1out)
		CLIP_AND_SWAP(sx,  1., xmax, p, q, r);
	
    if( y0out)
		CLIP_AND_SWAP(sy, -1., ymin, p, q, r);
	
    if( y1out)
		CLIP_AND_SWAP(sy,  1., ymax, p, q, r);

    /* if result ended up in p2 then copy it to p1 */
    if (p==&p2)
		bcopy( &p2, p1, sizeof(Poly)-(POLY_NMAX-p2.n)*sizeof(Poly_vert));
	
    return POLY_CLIP_PARTIAL;
}
