/*
 *  distance_map.hpp
 *  bmatte
 *
 */

#include<vector>

#include<OpenEXR/ImathVec.h>

#include<ANN/ANN.h>

class distance_map
{
public:

	distance_map();
	~distance_map();

	void add_point( Imath::V2i p) { points_.push_back( p);}

	void build();

	int nearest_point( Imath::V2i p, Imath::V2i& np);
	int nearest_point( Imath::V2i p, Imath::V2i& np, int &index);

private:

	std::vector<Imath::V2i> points_;

	ANNpointArray pts_;
	ANNpoint qpt_;
	ANNidxArray ix_;
	ANNdistArray dists_;
	ANNkd_tree *kd_;
};
