/* poly.h: definitions for polygon package */

#ifndef POLY_HDR
#define POLY_HDR

#include<assert.h>
#include<math.h>

#include<limits>

#include<OpenEXR/ImathMath.h>

#define POLY_NMAX 10		/* max #sides to a polygon; change if needed */
							/* note that poly_clip, given an n-gon as input, might output an (n+6)gon */
							/* POLY_NMAX=10 is thus appropriate if input polygons are triangles or quads */

struct Poly_vert
{
    float sx, sy, sz, sw;	/* screen space position (sometimes homo.) */
    float x, y, z;			/* world space position */
    float u, v, q;			/* texture position (sometimes homogeneous) */
    float r, g, b, a;		/* (red,green,blue,alpha) color */
    float nx, ny, nz;		/* world space normal vector */

	float dudx, dudy;		/* texture coords derivatives */
	float dvdx, dvdy;		/* texture coords derivatives */
};

/* update poly.c if you change this structure */
/* note: don't put > 32 floats in Poly_vert struct, or mask will overflow */

struct Poly
{
	int n;						/* number of sides */
	unsigned long mask;			/* interpolation mask for vertex elems */
	Poly_vert vert[POLY_NMAX];	/* vertices */
};

/*
 * mask is an interpolation mask whose kth bit indicates whether the kth
 * float in a Poly_vert is relevant.
 * For example, if the valid attributes are sx, sy, and sz, then set
 *	mask = POLY_MASK(sx) | POLY_MASK(sy) | POLY_MASK(sz);
 */

struct  Poly_box
{
    float x0, x1;		/* left and right */
    float y0, y1;		/* top and bottom */
    float z0, z1;		/* near and far */
};

struct Window
{
    int x0, y0;			/* xmin and ymin */
    int x1, y1;			/* xmax and ymax (inclusive) */
};

#define POLY_MASK(elem) (1 << ( &poly_dummy->elem - (float *) poly_dummy))

#define POLY_CLIP_OUT 0		/* polygon entirely outside box */
#define POLY_CLIP_PARTIAL 1	/* polygon partially inside */
#define POLY_CLIP_IN 2		/* polygon entirely inside box */

extern Poly_vert *poly_dummy;	/* used superficially by POLY_MASK macro */

// prototypes
void copy_poly( Poly *src, Poly *dst);

void incrementalize_y( float *p1, float *p2, float *p, float *dp, int y, unsigned long mask);
void incrementalize_x( float *p1, float *p2, float *p, float *dp, int x, unsigned long mask);
void increment( float *p, float *dp, unsigned long mask);

// clipping
void poly_clip_to_halfspace( Poly *p, Poly *q, int index, float sign, float k);
int poly_clip_to_box( Poly *p1, Poly_box *box);
int poly_clip_near_far( Poly *p1, float znear, float zfar);
int poly_clip_y( Poly *p1, float ymin, float ymax);
int poly_clip_xy( Poly *p1, float xmin, float ymin, float xmax, float ymax);

// scan conversion

/* scanline: output scanline by sampling polygon at Y=y+.5 */

template<class PixelFun>
void scanline( int y, Poly_vert *l, Poly_vert *r, Window *win, PixelFun pfun, unsigned long mask)
{
    int x, lx, rx;
    Poly_vert p, dp;

    mask &= ~POLY_MASK(sx);		/* stop interpolating screen x */
    lx = Imath::Math<float>::ceil(l->sx - float( 0.5));
	
    if( lx<win->x0)
		lx = win->x0;
	
    rx = Imath::Math<float>::floor(r->sx - float( 0.5));
	
    if( rx>win->x1)
		rx = win->x1;
	
    if( lx>rx)
		return;
	
    incrementalize_x( (float *) l, (float *) r, (float *) &p, (float *) &dp, lx, mask);
	
    for( x = lx; x <= rx; ++x)
	{
		pfun( x, y, &p);
		increment( (float *) &p, (float *) &dp, mask);
    }
}

/*
 * poly_scan: Scan convert a polygon, calling pixelproc at each pixel with an
 * interpolated Poly_vert structure.  Polygon can be clockwise or ccw.
 * Polygon is clipped in 2-D to win, the screen space window.
 *
 * Scan conversion is done on the basis of Poly_vert fields sx and sy.
 * These two must always be interpolated, and only they have special meaning
 * to this code; any other fields are blindly interpolated regardless of
 * their semantics.
 *
 * The pixelproc subroutine takes the arguments:
 *
 *	pixelproc(x, y, point)
 *	int x, y;
 *	Poly_vert *point;
 *
 * All the fields of point indicated by p->mask will be valid inside pixelproc
 * except sx and sy.  If they were computed, they would have values
 * sx=x+.5 and sy=y+.5, since sampling is done at pixel centers.
 */

template<class PixelFun>
void poly_scan( Poly *p, Window *win, PixelFun pfun)
{
	assert( p->n <= POLY_NMAX);
	assert( sizeof( Poly_vert) / sizeof( float) <= 32);
	
    int i, li, ri, y, ly, ry, top, rem;
    unsigned long mask;
    float ymin;
    Poly_vert l, r, dl, dr;

    ymin = std::numeric_limits<float>::max();
	
    for (i=0; i<p->n; i++)		/* find top vertex (y points down) */
	{
		if (p->vert[i].sy < ymin)
		{
		    ymin = p->vert[i].sy;
		    top = i;
		}
	}

    li = ri = top;												/* left and right vertex indices */
    rem = p->n;													/* number of vertices remaining */
    y = Imath::Math<float>::ceil( ymin - float( 0.5));			/* current scan line */
    ly = ry = y-1;												/* lower end of left & right edges */
    mask = p->mask & ~POLY_MASK(sy);							/* stop interpolating screen y */

    while( rem > 0) /* scan in y, activating new edges on left & right */
	{
		/* as scan line passes over new vertices */

		while( ly <= y && rem > 0)
		{	/* advance left edge? */
			rem--;
			i = li-1;			/* step ccw down left side */
			if( i < 0)
				i = p->n-1;
			
			incrementalize_y( (float *)&p->vert[li], (float *)&p->vert[i], (float *) &l, (float*) &dl, y, mask);
			ly = Imath::Math<float>::floor( p->vert[i].sy + float( 0.5));
			li = i;
		}
		
		while( ry <= y && rem > 0)	/* advance right edge? */
		{
		    rem--;
		    i = ri+1;			/* step cw down right edge */
		    if( i>=p->n)
				i = 0;
			
		    incrementalize_y( (float *)&p->vert[ri], (float *)&p->vert[i], (float *)&r, (float *)&dr, y, mask);
		    ry = Imath::Math<float>::floor(p->vert[i].sy + float( 0.5));
		    ri = i;
		}

		while( y < ly && y < ry)	    /* do scanlines till end of l or r edge */
	    {
			if( y >= win->y0 && y <= win->y1)
			{
				if (l.sx<=r.sx)
				    scanline(y, &l, &r, win, pfun, mask);
				else
				    scanline(y, &r, &l, win, pfun, mask);
			}
			
		    y++;
		    increment( (float *)&l, (float *)&dl, mask);
		    increment( (float *)&r, (float *)&dr, mask);
		}
    }
}

#endif
