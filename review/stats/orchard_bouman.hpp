// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_STATS_ORCHARD_BOUMAN_HPP
#define RAMEN_STATS_ORCHARD_BOUMAN_HPP

#include<algorithm>
#include<stdexcept>

#include<vector>
#include<list>

#include<boost/static_assert.hpp>
#include<boost/bind.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathPlane.h>

#include<Eigen/Eigenvalues>

#include<ramen/assert.hpp>

namespace ramen
{
namespace stats
{

template<class Vec3Type>
class orchard_bouman_t
{
public:

	typedef Vec3Type						vector_type;
	typedef typename vector_type::BaseType	real_type;
	typedef Eigen::Matrix<real_type, 3, 3>	matrix_type;
	
private:
	
	BOOST_STATIC_ASSERT(( Vec3Type::dimensions() == 3));
	
	struct always_one_weight_fun
	{
		real_type operator()( int i) const { return real_type( 1);}
	};
	
	struct vector_weight_fun
	{
		vector_weight_fun( const std::vector<real_type>& weights) : weights_( weights) {}
		
		real_type operator()( int i) const
		{
			RAMEN_ASSERT( i >= 0 && i < weights_.size());
			return weights_[i];
		}
		
	private:
		
		std::vector<real_type>& weights_;
	};
	
public:
	
	orchard_bouman_t( int max_clusters)
	{
		RAMEN_ASSERT( max_clusters > 0);

		max_clusters_ = max_clusters;
	}
	
	struct cluster_t
	{
		cluster_t() { init();}
		
		bool empty() const { return num_samples == 0 || sum_weights == 0;}
				
		real_type max_eigenvalue() const			{ return eigenvalue;}
		const vector_type& max_eigenvector() const	{ return eigenvector;}

		template<class WeightFun>
		void compute_stats( const std::vector<vector_type>& samples, WeightFun wfun,
								const std::vector<const cluster_t*>& labels)
		{
			init();
			
			int size = samples.size();
			
			// calc mean
			for( int i = 0; i < size; ++i)
			{
				if( labels[i] == this)
				{
					real_type w = wfun( i);
					sum_weights += w;
					mean += ( samples[i] * w);
					++num_samples;
				}
			}
			
			if( sum_weights == 0)
			{
				// TODO: do something here
				return;
			}
			
			mean /= sum_weights;
			
			// calc covariance
			for( int i = 0; i < size; ++i)
			{
				if( labels[i] == this)
				{
					real_type x = samples[i].x - mean.x;
					real_type y = samples[i].y - mean.y;
					real_type z = samples[i].z - mean.z;
					real_type w = wfun( i);
					covariance( 0, 0) += w * x * x;
					covariance( 1, 0) += w * y * x;
					covariance( 1, 1) += w * y * y;
					covariance( 2, 0) += w * z * x;
					covariance( 2, 1) += w * z * y;
					covariance( 2, 2) += w * z * z;
				}
			}
			
			covariance( 0, 1) = covariance( 1, 0);
			covariance( 0, 2) = covariance( 2, 0);
			covariance( 1, 2) = covariance( 2, 1);
			covariance /= sum_weights;
			eigen_decomp();
		}	
		
		template<class WeightFun>		
		void split( cluster_t& other, const std::vector<vector_type>& samples, 
					WeightFun wfun, std::vector<const cluster_t*>& labels)
		{
			RAMEN_ASSERT( !empty());
			
			Imath::Plane3<real_type> split_plane( mean, eigenvector);
						
			int size = samples.size();
			
			for( int i = 0; i < size; ++i)
			{
				if( labels[i] == this)
				{
					if( split_plane.distanceTo( samples[i]) < 0)
					{
						labels[i] = &other;
						num_samples--;
						other.num_samples++;
					}
				}
			}
			
			compute_stats( samples, wfun, labels);
			other.compute_stats( samples, wfun, labels);
		}
		
	private:
	
		void init()
		{
			num_samples = 0;
			sum_weights = 0;
			eigenvalue = 0;
			
			for( int i = 0; i < vector_type::dimensions(); ++i)
			{
				mean[i] = 0;
				eigenvector[i] = 0;
			}

			for( int j = 0; j < 3; ++j)
				for( int i = 0; i < 3; ++i)
					covariance( j, i) = 0;
		}

		void eigen_decomp()
		{
			Eigen::SelfAdjointEigenSolver<matrix_type> esolver( covariance);
			eigenvalue = esolver.eigenvalues()( 2);
			
			for( int i = 0; i < 3; ++i)
				eigenvector[i] = esolver.eigenvectors()( 0, i);
		}
		
	public:

		int num_samples;
        real_type sum_weights;
        vector_type mean;
		matrix_type covariance;
		
		real_type eigenvalue;
		vector_type eigenvector;
	};

	// samples
	const std::vector<const cluster_t*>& labels() const { return labels_;}
	
	// clusters
	const std::list<cluster_t>& clusters() const { return clusters_;}

	typedef typename std::list<cluster_t>::const_iterator const_iterator;
	
	const_iterator begin() const	{ return clusters_.begin();}
	const_iterator end() const		{ return clusters_.end();}

	// algorithm
	void cluster_samples( const std::vector<vector_type>& samples)
	{
		do_cluster_samples( samples, always_one_weight_fun());
	}
	
	void cluster_samples( const std::vector<vector_type>& samples, 
						  const std::vector<real_type>& weights)
	{
		do_cluster_samples( samples, vector_weight_fun( weights));
	}

private:

	typedef typename std::list<cluster_t>::const_iterator const_iterator;
	
	template<class WeightFun>
	void do_cluster_samples( const std::vector<vector_type>& samples, WeightFun wfun)
	{
        if( samples.empty())
            throw std::runtime_error( "No samples to cluster!");
		
		int num_clusters = std::min( max_clusters_, ( int) samples.size());

		labels_.clear();
		clusters_.clear();

		// init first cluster
		clusters_.push_back( cluster_t());
		labels_.insert( labels_.begin(), samples.size(), (const cluster_t*) &clusters_.front());		
		clusters_.front().compute_stats( samples, labels_);
		
		while( clusters_.size() < num_clusters)
		{
			 iterator it = find_biggest_cluster();			 
			 clusters_.push_back( cluster_t());
			 it->split( clusters_.back(), samples, wfun, labels_);
		 }
		
		// erase empty clusters
		clusters_.erase( std::remove_if( clusters_.begin(), clusters_.end(), boost::bind( &orchard_bouman_t::cluster_t::empty, _1)), clusters_.end());		
	}	

	iterator find_biggest_cluster()
	{
		RAMEN_ASSERT( !clusters_.empty());
		
		real_type max_eval = -1;
		
		for( iterator it( clusters_.begin()), ei( clusters_.end()); it != ei; ++it)
		{
			if( it->max_eigenvalue() > max_eval)
			{
				max_eval = it->max_eigenvalue();
				biggest = it;
			}
		}
		
		return biggest;
	}
	
	// params
	int max_clusters_;
	real_type max_variance_;
	
    // samples
	std::vector<const cluster_t*> labels_;
	
	// clusters
	std::list<cluster_t> clusters_;	
};

} // namespace
} // namespace

#endif
