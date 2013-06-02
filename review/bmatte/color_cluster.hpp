/*
 *  color_cluster.hpp
 *  color_cluster
 *
 */

/*
	TODO
	----
	
		- zero weight samples
		- exceptions: zero_samples, ...
		- better inverse covariance handling
*/

#ifndef FUGU_STATS_COL_CLUSTER_HPP
#define FUGU_STATS_COL_CLUSTER_HPP

#include<vector>
#include<utility>
#include<algorithm>
#include<functional>
#include<stdexcept>

#include<boost/bind.hpp>

#include<boost/iterator/filter_iterator.hpp>

#include<OpenEXR/ImathColor.h>
#include<OpenEXR/ImathMatrix.h>
#include<OpenEXR/ImathPlane.h>

namespace fugu
{

namespace stats
{

// lapack eigenvalues
template<class X> long xspev( X *symm, int size, X *eigvals, X *eigvecs, X *work);
template<> long xspev( float *symm, int size, float *eigvals, float *eigvecs, float *work);
template<> long xspev( double *symm, int size, double *eigvals, double *eigvecs, double *work);

template<class T>
class color_cluster
{
public:

	typedef T					value_type;
	typedef Imath::Color3<T>	color_type;
	typedef Imath::Matrix33<T>	matrix_type;

	class cluster;
		
	class sample
	{
	public:

		sample( const Imath::Color3<T>& col, T weight=T(1)) : c(col), w(weight) {}

		cluster *owner() const	{ return owner_;}
		void set_owner( cluster *cl) { owner_ = cl;}

		Imath::Color3<T> c;
		T w;
		cluster *owner_;
	};

	typedef std::vector<sample>						sample_vector;
	typedef typename sample_vector::iterator		sample_iterator;
	typedef typename sample_vector::const_iterator	const_sample_iterator;

	// cluster selector
	struct cluster_select
	{
		cluster_select( const cluster *c) : c_(c) {}
		bool operator()( const sample& s) const { return s.owner() == c_;}
		const cluster *c_;
	};

	struct set_cluster_owner
	{
		set_cluster_owner( cluster *c) : c_(c) {}
		void operator()( sample& s) const { return s.set_owner( c_);}
		cluster *c_;	
	};

	class cluster
	{
	public:
	
		cluster() : min_var_(0)
		{
		}

		cluster( T min_var) : min_var_( min_var)
		{
		}

		// get / set
				
		int num_samples() const { return num_samples_;}
		
		const color_type& mean() const { return mean_;}

		const matrix_type& covariance() const { return cov_;}
		matrix_type& covariance() { return cov_;}
		
		T variance() const { return max_eigval_;}

		const Imath::Vec3<T>& axis() const	{ return axis_;}

		void invert_covariance() { icov_ = cov_.inverse();}
		const matrix_type& inverse_covariance() const { return icov_;}

		const Imath::Vec3<T>& eigenvals() const	{ return eigvals_;}
		const Imath::Matrix33<T>& eigenvecs() const { return eigvecs_;}

		// for std::max_element
		bool operator<( const cluster& c) const
		{
			return max_eigval_ < c.max_eigval_;
		}

		void calc_stats( const_sample_iterator first, const_sample_iterator last)
		{
		boost::filter_iterator<cluster_select, const_sample_iterator> it( cluster_select( this), first, last);
		boost::filter_iterator<cluster_select, const_sample_iterator> l( cluster_select( this), last, last);

			num_samples_ = 0;
			mean_.setValue( 0, 0, 0);
			sumw_ = 0;

			// calc mean
			for( ; it != l; ++it)
			{
				++num_samples_;
				float w = it->w;					
				mean_.x += w * it->c.x;
				mean_.y += w * it->c.y;
				mean_.z += w * it->c.z;
				sumw_ += w;
			}
			
			
			assert( num_samples_ != 0);
			mean_ /= sumw_;

			// covariance
			cov_ = 0;

			if( num_samples_ > 1)
			{
				it = boost::filter_iterator<cluster_select, const_sample_iterator>( cluster_select( this), first, last);

				// calc cov_
				for( ; it != l; ++it)
				{
					float x = it->c.x - mean_.x;
					float y = it->c.y - mean_.y;
					float z = it->c.z - mean_.z;
					float w = it->w;
		
					cov_[0][0] += w * x * x;
					cov_[1][0] += w * y * x;
					cov_[1][1] += w * y * y;
					cov_[2][0] += w * z * x;
					cov_[2][1] += w * z * y;
					cov_[2][2] += w * z * z;
				}
				
				cov_[0][1] = cov_[1][0];
				cov_[0][2] = cov_[2][0];
				cov_[1][2] = cov_[2][1];
				cov_ /= sumw_;
				
				eigen_decomp();
			}
			else
			{			
				cov_ = Imath::Matrix33<T>( min_var_, 0, 0,
											0, min_var_, 0,
											0, 0, min_var_);

				eigvals_ = Imath::Vec3<T>( min_var_, min_var_, min_var_);
				eigvecs_ = Imath::Matrix33<T>(	1, 0, 0, 
												0, 1, 0,
												0, 0, 1);
				max_eigval_ = min_var_;
			}	
		}
		
		void eigen_decomp()
		{
		T symm[6], work[30];
		long info;

			symm[0] = cov_[0][0];
			symm[1] = cov_[1][0];
			symm[2] = cov_[1][1];
			symm[3] = cov_[2][0];
			symm[4] = cov_[2][1];
			symm[5] = cov_[2][2];

			info = xspev<T>( symm, 3, eigvals_.getValue(), eigvecs_.getValue(), work);
	
			if( info != 0)
				assert( false);	// error, this should never happen as the matrix is symmetric.

			max_eigval_ = std::max( eigvals_.x, std::max( eigvals_.y, eigvals_.z));

			int ix;

			if( max_eigval_ == eigvals_.x)
				ix = 0;
			else
			{
				if( max_eigval_ == eigvals_.y)
					ix = 1;
				else
					ix = 2;
			}
			
			axis_ = Imath::Vec3<T>( eigvecs_[ix][0], eigvecs_[ix][1], eigvecs_[ix][2]);
		}
		
		void split( sample_iterator first, sample_iterator last, cluster *newc)
		{
		boost::filter_iterator<cluster_select, sample_iterator> it( cluster_select( this), first, last);
		boost::filter_iterator<cluster_select, sample_iterator> l( cluster_select( this), last, last);
		Imath::Vec3<T> axis;

			assert( num_samples_ > 1);
			Imath::Plane3<T> split_plane( mean_, axis);

			for( ; it != l; ++it)
			{
				if( split_plane.distanceTo( it->c) < 0)
					it->set_owner( newc);
			}
			
			calc_stats( first, last);
			newc->calc_stats( first, last);
		}
		
	private:
	
		int num_samples_;

		Imath::Color3<T> mean_;
		T sumw_;
		T min_var_;
		
		Imath::Vec3<T> eigvals_;
		Imath::Matrix33<T> eigvecs_;
		T max_eigval_;
		Imath::Vec3<T> axis_;
		
		Imath::Matrix33<T> cov_;
		Imath::Matrix33<T> icov_;
	};

	typedef std::vector<cluster>					cluster_vector;
	typedef typename cluster_vector::iterator		cluster_iterator;
	typedef typename cluster_vector::const_iterator	const_cluster_iterator;

	color_cluster() : max_clusters_(-1), max_var_( T(-1)) {}

	color_cluster( int max_clusters, T max_var) : max_clusters_(max_clusters), max_var_(max_var)
	{
		assert( max_clusters > 0);
		clusters_.reserve( max_clusters_);
	}
	
	// set / get
	
	int max_clusters()	{ return max_clusters_;}
	int num_clusters()	{ return clusters_.size();}
	void max_clusters( int maxc)
	{
		assert( maxc > 0);
		clusters_.clear();
		max_clusters_ = maxc;
		clusters_.reserve( max_clusters_);
	}

	T max_variance()	{ return max_var_;}
	void max_variance( T max_var)
	{
		assert( max_var >= 0);
		max_var_ = max_var;
	}

	T variance()		{ return var_;}

	// samples
	void clear()	{ samples_.clear();}
	void add_sample( const color_type& c, T w = T(1))
	{
		if( w != T(0))
			samples_.push_back( sample( c, w));
	}

	// iterators
	sample_iterator samples_begin()					{ return samples_.begin();}
	sample_iterator samples_end()					{ return samples_.end();}

	const_sample_iterator samples_begin() const		{ return samples_.begin();}
	const_sample_iterator samples_end()	const		{ return samples_.end();}
	
	cluster_iterator clusters_begin()				{ return clusters_.begin();}
	cluster_iterator clusters_end()					{ return clusters_.end();}

	const_cluster_iterator clusters_begin() const	{ return clusters_.begin();}
	const_cluster_iterator clusters_end() const		{ return clusters_.end();}
	
	// cluster
	void cluster_samples()
	{
		assert( max_clusters_ > 0);
		assert( max_var_ > 0);

		if( samples_.empty())
			throw std::runtime_error( "no samples to cluster");

		if( samples_.size() < 2)
			throw std::runtime_error( "not enough samples to cluster, min 2");

		clusters_.clear();
		clusters_.push_back( cluster( max_var_));	// add first cluster

		// assign all samples to first cluster
		std::for_each( samples_begin(), samples_end(), set_cluster_owner( &(clusters_.front())));

		// calc stats for first cluster
		clusters_.front().calc_stats( samples_.begin(), samples_.end());

		while( 1)
		{
			cluster_iterator it( std::max_element( clusters_.begin(), clusters_.end()));
			var_ = it->variance();
			
			if( (var_ <= max_var_) || (num_clusters() == max_clusters_))
				return;	// we are done, exit

			// split biggest cluster in two
			clusters_.push_back( cluster( max_var_));
			it->split( samples_.begin(), samples_.end(), &(clusters_.back()));
		}
	}

private:

	int max_clusters_;
	T max_var_;

	sample_vector samples_;
	cluster_vector clusters_;
	
	T var_;
};

} // namespace
} // namespace

#endif
