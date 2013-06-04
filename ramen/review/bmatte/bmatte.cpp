/*
 *  bmatte.cpp
 *  bmatte
 *
 */

#include"bmatte.hpp"

#include<algorithm>
#include<limits>
#include<iostream>

#include<Accelerate/Accelerate.h>

// utils
template<class T> Imath::Vec3<T> nhmultMatrixVec3( const Imath::Vec3<T>& v, const Imath::Matrix33<T>& m);

bmatte::bmatte( const blitz::Array<Imath::Color3f,2>& image, const blitz::Array<uint8_t,2> tri, blitz::Array<Imath::Color3f,2> fg, 
				blitz::Array<Imath::Color3f,2> bg, blitz::Array<float,2> alpha, const adobe::dictionary_t& params)
						: image_(image), mask_(tri), fg_(fg), bg_(bg),alpha_(alpha)

{
	adobe::get_value<bool>( params, adobe::name_t("use_back"), use_back_);

	// get from the dictionary
	adobe::get_value<int>( params, adobe::name_t("kwin_size"), known_window_size_);
	adobe::get_value<int>( params, adobe::name_t("uwin_size"), unknown_window_size_);
	adobe::get_value<int>( params, adobe::name_t("min_samples"), min_samples_);
	
	int maxk;
	adobe::get_value<int>( params, adobe::name_t("maxk"), maxk);
	
	double cth;
	adobe::get_value<double>( params, adobe::name_t("ctheresh"), cth);
			
	fg_cluster_.max_clusters( maxk);
	fg_cluster_.max_variance( cth);

	bg_cluster_.max_clusters( maxk);
	bg_cluster_.max_variance( cth);
	
	adobe::get_value<double>( params, adobe::name_t("cvar"), cam_var_);
	cam_var2_ = cam_var_ * cam_var_;
}

void bmatte::init_arrays()
{
uint8_t val;
	
	computed_.resize( image_.rows(), image_.cols());

	mask_.thereshold( 15, 240);

	for( int j=0; j < image_.rows(); ++j)
	{
		for( int i=0; i < image_.cols(); ++i)
		{
			val = mask_( j, i);
			computed_( j, i) = 0;
			
			if( val == bg_val)
			{
				bg_( j, i) = image_( j, i);
				fg_( j, i) = Imath::Color3f( .0f, .0f, .0f);
				alpha_( j, i) = .0f;
			}
			else
			{
				if( val == fg_val)
				{
					fg_( j, i) = image_( j, i);
					bg_( j, i) = Imath::Color3f( .0f, .0f, .0f);
					alpha_( j, i) = 1.0f;
				}
				else
					alpha_( j, i) = 0.5f;
			}
		}
	}
}

void bmatte::build_dmap( uint8_t val, distance_map& dmap)
{
	for( int j=1; j < mask_.height()-1; ++j)
	{
		for( int i=1; i < mask_.width()-1; ++i)
		{
			if( mask_(j, i) == val)
				if( mask_(j-1, i) != val || mask_(j+1, i) != val || mask_(j, i-1) != val || mask_(j, i+1) != val)
					dmap.add_point( Imath::V2i( i, j));
		}
	}
	
	dmap.build();
}

void bmatte::sample_fg( Imath::V2i p)
{
Imath::V2i np;
int xmin, xmax, ymin, ymax;
Imath::Color3f col;
int kw, numsamples = 0;

	fg_map_.nearest_point( p, np);
	kw = known_window_size_;
		
	while( 1)
	{
		xmin = std::max( 1, np.x - kw);
		ymin = std::max( 1, np.y - kw);
		xmax = std::min( np.x + kw, image_.cols() - 1);
		ymax = std::min( np.y + kw, image_.rows() - 1);
	
		for( int j=ymin;j<ymax;++j)
		{
			for( int i=xmin;i<xmax;++i)
			{
				if( mask_( j, i) == fg_val)
				{
					col = fg_( j, i);
					fg_cluster_.add_sample( color_type( col.x, col.y, col.z) , lut_( i - p.x, j - p.y));
					++numsamples;
				}
			}
		}
		
		if( numsamples >= min_samples_)
			return;
		
		kw *= 2;
		fg_cluster_.clear();
	}
}

void bmatte::sample_bg( Imath::V2i p)
{
Imath::V2i np;
int xmin, xmax, ymin, ymax;
Imath::Color3f col;
int kw, numsamples = 0;

	if( use_back_)
	{
		bg_color_ = bg_( p.y, p.x);
		return;
	}
	
	bg_map_.nearest_point( p, np);

	kw = known_window_size_;

	while( 1)
	{
		xmin = std::max( 1, np.x - kw);
		ymin = std::max( 1, np.y - kw);
		xmax = std::min( np.x + kw, image_.cols() - 1);
		ymax = std::min( np.y + kw, image_.rows() - 1);
	
		for( int j=ymin;j<ymax;++j)
		{
			for( int i=xmin;i<xmax;++i)
			{
				if( mask_( j, i) == bg_val)
				{
					col = bg_( j, i);
					bg_cluster_.add_sample( color_type( col.x, col.y, col.z), lut_( i - p.x, j - p.y));
					++numsamples;
				}
			}
		}
		
		if( numsamples >= min_samples_)
			return;
		
		kw *= 2;
		bg_cluster_.clear();
	}
}

double bmatte::sample_uk( Imath::V2i p)
{
Imath::V2i np;
int xmin, xmax, ymin, ymax;
double w, a;
double all_a=0;
int num_a=0;
Imath::Color3f col;

	xmin = std::max( 1, p.x - unknown_window_size_);
	ymin = std::max( 1, p.y - unknown_window_size_);
	xmax = std::min( p.x + unknown_window_size_, image_.cols() - 1);
	ymax = std::min( p.y + unknown_window_size_, image_.rows() - 1);
	
	for( int j=ymin;j<ymax;++j)
	{
		for( int i=xmin;i<xmax;++i)
		{
			if( computed_( j, i))
			{
				a = alpha_( j, i);
				all_a += a;
				++num_a;

				w = lut_( i-p.x, j-p.y);
				
				col = fg_( j, i);
				fg_cluster_.add_sample( color_type( col.x, col.y, col.z), w * a * a);

				if( !use_back_)
				{
					col = bg_( j, i);
					bg_cluster_.add_sample( color_type( col.x, col.y, col.z), w * (1.0 - a) * (1.0 - a));
				}
			}
		}
	}
	
	if( num_a)
		return all_a / (double) num_a;
	else
		return 0.5;
}

void bmatte::operator()()
{
color_type c,f,b;
double a;

	init_arrays();
	build_dmap( 255, fg_map_);

	if( !use_back_)
		build_dmap( 0, bg_map_);

	std::cout << "Matting started: " << mask_.num_uk() << " pixels\n";

	while( mask_.shrink())
	{
		trimap::contour_iterator it( mask_.contour_begin());
		for( ; it != mask_.contour_end(); ++it)
		{
			fg_cluster_.clear();
			bg_cluster_.clear();

			Imath::V2i p( it->second, it->first);
			
			sample_fg( p);
			sample_bg( p);
			
			a = sample_uk( p);
						
			fg_cluster_.cluster_samples();
			
			if( !use_back_)
				bg_cluster_.cluster_samples();
			
			c = image_( p.y, p.x);
			optimize( c, f, b, a);

			if( a < 0.01)
			{
				a = 0.0;
				b = c;
			}
			else
			{
				if( a > 0.99)
				{
					a = 1.0;
					f = c;
				}
			}

			fg_( p.y, p.x) = f;
			bg_( p.y, p.x) = b;
			alpha_( p.y, p.x) = a;
			computed_( p.y, p.x) = 1;
		}
		
		std::cout << "*";
		std::cout.flush();
	}
	
	std::cout << "\n";
}

void bmatte::optimize( color_type c, color_type& f, color_type& b, double& a)
{
color_cluster_type::cluster_iterator it( fg_cluster_.clusters_begin()), it2( bg_cluster_.clusters_begin());
double likelihood, best_likelihood = -100000.0;
color_type fg, bg;
double alpha, mean_alpha = a;

	for( ; it != fg_cluster_.clusters_end(); ++it)
	{
		matrix_type& m = it->covariance();
		m[0][0] += cam_var2_; m[1][1] += cam_var2_; m[2][2] += cam_var2_;
		it->invert_covariance();
	}

	if( !use_back_)
	{
		for( ; it2 != bg_cluster_.clusters_end(); ++it2)
		{
			matrix_type& m = it2->covariance();
			m[0][0] += cam_var2_; m[1][1] += cam_var2_; m[2][2] += cam_var2_;
			it2->invert_covariance();
		}
		
		for( it = fg_cluster_.clusters_begin(); it != fg_cluster_.clusters_end(); ++it)
		{
			for( it2 = bg_cluster_.clusters_begin(); it2 != bg_cluster_.clusters_end(); ++it2)
			{
				alpha = mean_alpha;
				likelihood = optimize_pair( it, it2, c, fg, bg, alpha);

				if( likelihood > best_likelihood)
				{
					best_likelihood = likelihood;
					f = fg;
					b = bg;
					a = alpha;
				}
			}
		}
	}
	else
		; // one sample case
}

float bmatte::optimize_pair( bmatte::color_cluster_type::cluster_iterator it, bmatte::color_cluster_type::cluster_iterator it2, 
										const color_type& c, color_type& fg, color_type& bg, double& alpha)
{
const matrix_type& finv = it->inverse_covariance();
const matrix_type& binv = it2->inverse_covariance();

blitz::TinyMatrix<double,6,6> A;
blitz::TinyVector<double,6> X;
color_type f( fg), b( bg);
double a = alpha;
double old_a = -1.0;
int iters = 0;

	A = 0;
	for( int j=0;j<3;++j)
	{
		for( int i=0;i<3;++i)
		{
			A( j, i)	 = finv[j][i];
			A( j+3, i+3) = binv[j][i];
		}
	}
			
	while( ( fabs( a - old_a) > 0.005) && iters < 200)
	{
		double a2 = a * a;
		double ainv = (1.0f - a);
		double ainv2 = ainv * ainv;

		for( int j=0;j<3;++j)
		{
			A( j, j)	= finv[j][j] + (a2 / cam_var2_);
			A( j+3, j+3)= binv[j][j] + (ainv2 / cam_var2_);
		
			A( j+3, j)	= a * ainv / cam_var2_;
			A( j, j+3)	= a * ainv / cam_var2_;
		}
		
		vector_type v = nhmultMatrixVec3( it->mean(), it->inverse_covariance());
		X[0] = v.x + (c.x * a / cam_var2_);
		X[1] = v.y + (c.y * a / cam_var2_);
		X[2] = v.z + (c.z * a / cam_var2_);

		v = nhmultMatrixVec3( it2->mean(), it2->inverse_covariance());
		X[3] = v.x + (c.x * ainv / cam_var2_);
		X[4] = v.y + (c.y * ainv / cam_var2_);
		X[5] = v.z + (c.z * ainv / cam_var2_);
		
		if( !lin_solve( A, X))
			return -1000000.0;
		
		f = color_type( X[0], X[1], X[2]);
		b = color_type( X[3], X[4], X[5]);
		old_a = a;

		vector_type fmb( f - b);
		a = ((c - b) ^ fmb) / fmb.length2();
		++iters;
	}
	
	fg = f; bg = b;
	alpha = a;
	
	return lc( c, fg, bg, a) + lf( fg, it->mean(), it->inverse_covariance()) + lb( bg, it2->mean(), it2->inverse_covariance());
}

// log likelihoods
double bmatte::lc( const color_type& c, const color_type& f, const color_type& b, double a)
{
	vector_type  v( c.x - (a * f.x) - ((1.0 - a) * b.x),
							 c.y - (a * f.y) - ((1.0 - a) * b.y),
							 c.z - (a * f.z) - ((1.0 - a) * b.z));

	return -(v.length2() / cam_var2_);
}

double bmatte::lf( const color_type& f, const color_type& fmean, const matrix_type icov)
{
	vector_type v( f - fmean);
	vector_type v2 = v * icov;
	return -(v2 ^ v) * 0.5;
}

double bmatte::lb( const color_type& b, const color_type& bmean, const matrix_type icov)
{
	vector_type v( b - bmean);
	vector_type v2 = v * icov;
	return -(v2 ^ v) * 0.5;
}

bool bmatte::lin_solve( const blitz::TinyMatrix<double,6,6>& a, blitz::TinyVector<double,6>& b)
{
long ipiv[6];
blitz::TinyMatrix<double,6,6> mat;
long info;
__CLPK_integer six=6, one=1;

	// put matrix in fortran order
	for( int j=0;j<6;++j)
		for( int i=0;i<6;++i)
			mat( i,j) = a( j,i);

   dgesv_( &six, &one, mat.data(), &six, ipiv, b.data(), &six, &info);

	if( info != 0 )
		return false;

	return true;
}

template<class T>
Imath::Vec3<T> nhmultMatrixVec3( const Imath::Vec3<T>& v, const Imath::Matrix33<T>& m)
{
	return Imath::Vec3<T>(  (m[0][0] * v.x) + (m[0][1] * v.y) + (m[0][2] * v.z),
						    (m[1][0] * v.x) + (m[1][1] * v.y) + (m[1][2] * v.z),
							(m[2][0] * v.x) + (m[2][1] * v.y) + (m[2][2] * v.z));
}

/*

void print_vector( const Imath::Color3f& c)
{
	std::cout << "[ " << c.x << " " << c.y << " " << c.z << "];\n";
}

void print_matrix( const Imath::M33f& m)
{
	std::cout << "[ " << m[0][0] << " " << m[0][1] << " " << m[0][2] << ";\n"
					  << m[1][0] << " " << m[1][1] << " " << m[1][2] << ";\n" 
					  << m[2][0] << " " << m[2][1] << " " << m[2][2] << "];\n";
}

void print_tiny_vec( const blitz::TinyVector<float,6>& v)
{
	std::cout << "[ ";

	for( int j=0;j<5;++j)
		std::cout << v(j) << " ";
	
	std::cout << v(5) << "];\n";
}

void print_tiny_matrix( const blitz::TinyMatrix<float,6,6>& A)
{
	std::cout << "[ ";

	for( int j=0;j<6;++j)
	{
		for( int i=0;i<5;++i)
			std::cout << A( j, i) << " "; 
		
		std::cout << A( j, 5) << ";\n";
	}
	
	std::cout << "];\n";
}

*/
