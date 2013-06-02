/*

	contour_map
	
*/

#include<algorithm>
#include<iostream>

#include"contour_map.hpp"

using namespace std;

void cm_cell::add_point( int x, int y)
{
cm_point p;

	p.set( x, y);
	points.push_back( p);
}

void cm_cell::nearest_point( int x, int y, int *nx, int *ny, int *d)
{
int i,j;
int k,dist;
int d0,d1;
	
	if( !points.empty())
	{
		i = x / csize; j = y / csize;

		if( i - cx > 0)
			d0 = (i - cx - 1) * csize;
		else
			d0 = 0;

		if( j - cy > 0)
			d1 = (j - cy - 1) * csize;
		else
			d1 = 0;

		dist = (d0 * d0) + (d1 * d1);

		if( dist > *d)
			return;
		
		for( k=0;k<points.size();k++)
		{
			dist = ((x - points[k].x) * (x - points[k].x)) + ((y - points[k].y) * (y - points[k].y));
			
			if( dist < *d)
			{
				*d = dist;
				*nx = points[k].x;
				*ny = points[k].y;
			}
		}
	}
}

contour_map::contour_map( int iwidth, int iheight, int cell_size)
{
	width = iwidth; height = iheight;
	csize = cell_size;
	nx = (width / csize) + 1;
	ny = (height / csize) + 1;

	grid = new cm_cell* [nx*ny];

	for( int i=0;i<nx*ny;i++)
		grid[i] = 0;
}

contour_map::contour_map( Gan_Image *mask, unsigned long val, int cell_size)
{
	width = mask->width; height = mask->height;
	csize = cell_size;
	nx = (width / csize) + 1;
	ny = (height / csize) + 1;

	grid = new cm_cell* [nx*ny];

	for( int i=0;i<nx*ny;i++)
		grid[i] = 0;
	
	build( this, mask, val);
}

contour_map::~contour_map()
{
	for( int i=0;i<nx*ny;i++)
		delete grid[i];
	
	delete[] grid;
}

void contour_map::add_point( int x, int y)
{
int i,j;

	i = x / csize; j = y / csize;
	
	if( !grid[(j * nx) + i])
		grid[(j * nx) + i] = new cm_cell( i, j, csize);
	
	grid[(j * nx) + i]->add_point( x, y);
}

int contour_map::nearest_point( int x, int y, int *xn, int *yn)
{
int i,j,window_size=0,max_window;
int d = (width * width) + (height * height);
int mind, adj_left, adj_right;

	i = x / csize; j = y / csize;
	
	max_window = max( nx-i, max( ny-j, max( i, j)));

	if( grid[(j * nx) + i])
		grid[(j * nx) + i]->nearest_point( x, y, xn, yn, &d);

	while( window_size < max_window)
	{
		window_size++; adj_left = adj_right = 0;
		mind = (window_size-1)*csize*(window_size-1)*csize;

		if( mind > d)
			return d;
		
		if( j-window_size >= 0)
		{
			adj_left = 1;
			for( int s = max( i-window_size, 0); s < min(i+window_size+1, nx); s++)
				if( grid[((j-window_size) * nx) + s])
					grid[((j-window_size) * nx) + s]->nearest_point( x, y, xn, yn, &d);
		}

		if( j+window_size < ny)
		{
			adj_right = 1;
			for( int s = max( i-window_size, 0); s < min(i+window_size+1, nx); s++)
				if( grid[((j+window_size) * nx) + s])
					grid[((j+window_size) * nx) + s]->nearest_point( x, y, xn, yn, &d);
		}

		if( i-window_size >= 0)
			for( int s = max( j-window_size+adj_left, 0); s < min(j+window_size+1-adj_right, ny); s++)
				if( grid[(j * nx) + i-window_size])
					grid[(j * nx) + i-window_size]->nearest_point( x, y, xn, yn, &d);
		
		if( i+window_size < nx)
			for( int s = max( j-window_size+adj_left, 0); s < min(j+window_size+1-adj_right, ny); s++)
				if( grid[(j * nx) + i+window_size])
					grid[(j * nx) + i+window_size]->nearest_point( x, y, xn, yn, &d);
	}
	
	return d;
}

void contour_map::build( contour_map *map, Gan_Image *mask, unsigned long val)
{
	// clear the last map	
	for( int i=0;i<map->nx*map->ny;i++)
	{
		if( map->grid[i])
			(map->grid[i])->clear();
	}
	
	if( mask->format != GAN_GREY_LEVEL_IMAGE)
	{
		cout << "Contour map error, the mask should be grey-level" << endl;
		return;
	}
	
	switch( mask->type)
	{
		case GAN_UCHAR:
			unsigned char *p0,*p1,*p2,v;
			int s,t;
			
				v = (unsigned char) val;

				for( unsigned int j=0;j<mask->height;j++)
				{
					if( j != 0)
						p0 = gan_image_get_pixptr_gl_uc( mask, j-1, 0);

					p1 = gan_image_get_pixptr_gl_uc( mask , j, 0);
		
					if( j != mask->height-1)
						p2 = gan_image_get_pixptr_gl_uc( mask, j+1, 0);
				
					for( unsigned int i=0;i<mask->width;i++)
					{
						if( *p1 == v)
						{
							if((i!=0 && *(p1-1)!=v) || (i!=mask->width-1 && (*(p1+1)!=v)) || (j!=0 && *p0!=v) || (j!=mask->height-1 && *p2!=v))
							{
								s = i / map->csize; t = j / map->csize;
	
								if( !map->grid[(t * map->nx) + s])
									map->grid[(t * map->nx) + s] = new cm_cell( s, t, map->csize);
	
								(map->grid[(t * map->nx) + s])->add_point( i, j);
							}
						}

						p0++; p1++; p2++;
					}
				}
		break;

		default:
			cout << "Contour map error, format not supported" << endl;
			return;
		break;
	}
}
