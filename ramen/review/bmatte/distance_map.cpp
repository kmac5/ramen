/*
 *  distance_map.cpp
 *
 */

#include"distance_map.hpp"

#include<iostream>

distance_map::distance_map() : kd_(0)
{
	qpt_  = annAllocPt( 2);
	ix_   = new ANNidx[1];
	dists_= new ANNdist[1];
}

distance_map::~distance_map()
{
	delete[] pts_;
	delete[] qpt_;
	delete[] ix_;
	delete kd_;
}

void distance_map::build()
{
	std::cout << "dist map: " << points_.size() << " points.\n";
	pts_ = annAllocPts( points_.size(), 2);
		
	std::vector<Imath::V2i>::iterator it( points_.begin());
	
	for( int i=0; it != points_.end(); ++it, ++i)
	{
		pts_[i][0] = it->x;
		pts_[i][1] = it->y;
	}
		
	kd_ = new ANNkd_tree( pts_, points_.size(), 2);
}

int distance_map::nearest_point( Imath::V2i p, Imath::V2i& np)
{
	qpt_[0] = p.x; qpt_[1] = p.y;
	kd_->annkSearch( qpt_, 1, ix_, dists_, 0);
		
	np.setValue( pts_[ ix_[0]][0], pts_[ ix_[0]][1]);
	return dists_[0];
}

int distance_map::nearest_point( Imath::V2i p, Imath::V2i& np, int& indx)
{
	qpt_[0] = p.x; qpt_[1] = p.y;
	kd_->annkSearch( qpt_, 1, ix_, dists_, 0);
		
	np.setValue( pts_[ ix_[0]][0], pts_[ ix_[0]][1]);

	indx = ix_[0];
	return dists_[0];
}
