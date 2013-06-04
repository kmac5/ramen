/*

	TODO:
	
		- more optimizations & better sampling scheme
		
*/

#ifndef __CONTOUR_MAP__
#define __CONTOUR_MAP__

#include<vector>
#include<algorithm>

#include<OpenEXR/ImathVec.h>

class cm_cell
{
public:

	Imath::V2i c;
	int csize;
	std::vector<Imath::V2i> points;

public:

	cm_cell( Imath::V2i p, int cell_size=64) : c(p), csize( cell_size) { }

	void clear( void) { points.clear(); }

	void add_point( Imath::V2i p) { points.push_back( p);}
	
	void nearest_point( Imath::V2i p, Imath::V2i& np, int& d)
	{
	int i,j;
	int k,dist;
	int d0,d1;
	
		if( !points.empty())
		{
			i = p.x / csize; j = p.y / csize;

			if( i - c.x > 0)
				d0 = (i - c.x - 1) * csize;
			else
				d0 = 0;

			if( j - c.y > 0)
				d1 = (j - c.y - 1) * csize;
			else
				d1 = 0;

			dist = (d0 * d0) + (d1 * d1);

			if( dist > d)
				return;
		
			for( k=0;k<points.size();k++)
			{
				dist = ((p.x - points[k].x) * (p.x - points[k].x)) + ((p.y - points[k].y) * (p.y - points[k].y));
			
				if( dist < d)
				{
					d = dist;
					np = points[k];
				}
			}
		}
	}
};

class contour_map
{
public:

	int width, height, csize;
	int nx,ny;
	cm_cell **grid;
	
public:

	contour_map( int iwidth, int iheight, int cell_size = 64)
	{
		width = iwidth; height = iheight;
		csize = cell_size;
		nx = (width / csize) + 1;
		ny = (height / csize) + 1;

		grid = new cm_cell* [nx*ny];

		for( int i=0;i<nx*ny;i++)
			grid[i] = 0;
	}

	~contour_map()
	{
		for( int i=0;i<nx*ny;i++)
			delete grid[i];
	
		delete[] grid;
	}

	void add_point( Imath::V2i p)
	{
	int i,j;

		i = p.x / csize; j = p.y / csize;
	
		if( !grid[(j * nx) + i])
			grid[(j * nx) + i] = new cm_cell( Imath::V2i( i, j), csize);
	
		grid[(j * nx) + i]->add_point( p);
	}

	int nearest_point( Imath::V2i p, Imath::V2i& np)
	{
	int i,j,window_size=0,max_window;
	int d = (width * width) + (height * height);
	int mind, adj_left, adj_right;

		i = p.x / csize; j = p.y / csize;
	
		max_window = std::max( nx-i, std::max( ny-j, std::max( i, j)));

		if( grid[(j * nx) + i])
			grid[(j * nx) + i]->nearest_point( p, np, d);

		while( window_size < max_window)
		{
			window_size++; adj_left = adj_right = 0;
			mind = (window_size-1)*csize*(window_size-1)*csize;

			if( mind > d)
				return d;
		
			if( j-window_size >= 0)
			{
				adj_left = 1;
				for( int s = std::max( i-window_size, 0); s < std::min(i+window_size+1, nx); s++)
					if( grid[((j-window_size) * nx) + s])
						grid[((j-window_size) * nx) + s]->nearest_point( p, np, d);
			}

			if( j+window_size < ny)
			{
				adj_right = 1;
				for( int s = std::max( i-window_size, 0); s < std::min(i+window_size+1, nx); s++)
					if( grid[((j+window_size) * nx) + s])
						grid[((j+window_size) * nx) + s]->nearest_point( p, np, d);
			}

			if( i-window_size >= 0)
				for( int s = std::max( j-window_size+adj_left, 0); s < std::min(j+window_size+1-adj_right, ny); s++)
					if( grid[(j * nx) + i-window_size])
						grid[(j * nx) + i-window_size]->nearest_point( p, np, d);
		
			if( i+window_size < nx)
				for( int s = std::max( j-window_size+adj_left, 0); s < std::min(j+window_size+1-adj_right, ny); s++)
					if( grid[(j * nx) + i+window_size])
						grid[(j * nx) + i+window_size]->nearest_point( p, np, d);
		}
	
		return d;
	}
};

#endif
