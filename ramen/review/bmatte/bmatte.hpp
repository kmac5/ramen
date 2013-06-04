/*
 *  bmatte.h
 *  bmatte
 *
 */

#ifndef BMATTE_HPP
#define BMATTE_HPP

#include<map>
#include<string>

#include<adobe/dictionary.hpp>

#include<OpenEXR/ImathColor.h>

#include"trimap.hpp"
#include"distance_map.hpp"

#include"color_cluster.hpp"

#include"gauss_lut2d.hpp"

class bmatte
{
public:

	// typedef's
	typedef Imath::Color3<double>	color_type;
	typedef Imath::Vec3<double>		vector_type;
	typedef Imath::Matrix33<double>	matrix_type;
	
	typedef fugu::stats::color_cluster<double>	color_cluster_type;
	
	bmatte( const blitz::Array<Imath::Color3f,2>& image, const blitz::Array<uint8_t,2> tri,
				blitz::Array<Imath::Color3f,2> fg, blitz::Array<Imath::Color3f,2> bg,
				blitz::Array<float,2> alpha, const adobe::dictionary_t& params);

	void init_arrays();
	void build_dmap( uint8_t val, distance_map& dmap);

	void sample_fg( Imath::V2i p);	
	void sample_bg( Imath::V2i p);	
	double sample_uk( Imath::V2i p);

	void operator()();

	void optimize( color_type c, color_type& f, color_type& b, double& a);

	float optimize_pair( color_cluster_type::cluster_iterator it, color_cluster_type::cluster_iterator it2,
							const color_type& c, color_type& fg, color_type& bg, double& alpha);
	
	// likelihoods
	double lc( const color_type& c, const color_type& f, const color_type& b, double a);
	double lf( const color_type& f, const color_type& fmean, const matrix_type icov);
	double lb( const color_type& b, const color_type& bmean, const matrix_type icov);

	bool lin_solve( const blitz::TinyMatrix<double,6,6>& a, blitz::TinyVector<double,6>& b);
	
private:

	blitz::Array<Imath::Color3f,2> image_;

	blitz::Array<Imath::Color3f,2> fg_, bg_;
	blitz::Array<float,2> alpha_;

	trimap mask_;
	blitz::Array<uint8_t,2> computed_;
	
	gauss_lut2d<double> lut_;
	
	distance_map fg_map_, bg_map_;
	color_cluster_type fg_cluster_, bg_cluster_;

	// clean bg case
	bool use_back_;
	Imath::Color3f bg_color_;

	int known_window_size_, unknown_window_size_;
	int min_samples_;
	
	double cam_var_, cam_var2_;
};

#endif
