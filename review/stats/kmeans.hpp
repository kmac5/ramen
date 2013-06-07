// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_STATS_KMEANS_HPP
#define RAMEN_STATS_KMEANS_HPP

#include<algorithm>
#include<ctime>
#include<stdexcept>

#include<vector>
#include<list>

#include<boost/bind.hpp>
#include<boost/random.hpp>

#include<OpenEXR/ImathVec.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace stats
{

template<class VecType>
struct kmeans_traits
{
	typedef VecType							vector_type;
	typedef typename vector_type::BaseType	real_type;

	static real_type zero() { return real_type( 0);}
	
	static real_type distance( const vector_type& a, const vector_type& b)
	{
		vector_type x( b - a);
		return x.length2();
	}
};
	
template<class VecType, class Traits = kmeans_traits<VecType> >
class kmeans_t
{
public:

    typedef Traits                          traits_type;	
	typedef VecType							sample_type;
	typedef typename traits_type::real_type	real_type;

    kmeans_t( std::size_t K, int max_runs = 5, int max_iters = 20, unsigned int seed = 0)
	{
        K_ = K;

        max_iters_ = max_iters;
        max_runs_ = max_runs;

        if( !seed)
            random_generator_.seed( static_cast<unsigned int>( std::time( 0)));
        else
            random_generator_.seed( seed);
	}
	
	struct cluster_t
	{
		cluster_t()
		{
			num_samples = 0;
			radius = traits_type::zero();
		}
		
		cluster_t( const sample_type& s)
		{
			num_samples = 0;
			radius = traits_type::zero();
			mean = s;
		}
		
		bool empty() const { return num_samples == 0;}
		
		void add_sample( const sample_type& s)
		{
			if( num_samples)
				mean += s;
			else
				mean = s;
			
			++num_samples;
		}
		
		void update_radius( const sample_type& s)
		{
			real_type d = traits_type::distance( s, mean);
			radius = std::max( radius, d);
		}
		
        int num_samples;
        sample_type mean;
        real_type radius;
	};

	const std::list<cluster_t>& clusters() const { return clusters_;}

	typedef typename std::list<cluster_t>::const_iterator const_iterator;
	
	const_iterator begin() const	{ return clusters_.begin();}
	const_iterator end() const		{ return clusters_.end();}
	
	void cluster_samples( const std::vector<sample_type>& samples)
	{
        if( samples.empty())
            throw std::runtime_error("No samples to cluster!");

        if( K_ > samples.size())
            K_ = samples.size();

		labels_.clear();
		labels_.insert( labels_.begin(), samples.size(), (const cluster_t*) 0);
		
        for( int run = 0; run < max_runs_; ++run)
        {
            clusters_.clear();
			
			// clear labels
			for( int i = 0; i < labels_.size(); ++i)
				labels_[i] = 0;
			
            generate_random_clusters( samples);
			float max_radius = do_kmeans( samples);

            if(( run == 0) || ( max_radius < run_radius_))
            {
                run_radius_ = max_radius;
                std::swap( clusters_, best_run_);
            }
		}

		// put back the best solution		
		std::swap( clusters_, best_run_);
	}
	
private:

    void generate_random_clusters( const std::vector<sample_type>& samples)
    {
        typedef boost::variate_generator<boost::mt19937&, boost::uniform_int<> > random_type;
        random_type random_( random_generator_, boost::uniform_int<>(0, samples.size() - 1));

        for( int i = 0; i < K_; ++i)
        {
            while( 1)
            {
                int indx = random_();

                if( labels_[indx] == 0)
                {
                    clusters_.push_back( cluster_t( samples[indx]));
                    labels_[indx] = &(clusters_.back());
                    break;
                }
            }
        }
    }

    int assign_nearest_cluster( const std::vector<sample_type>& samples)
    {
        int num_swaps = 0;

		for( int i = 0; i < samples.size(); ++i)
        {
			int nearest;
			
			const cluster_t *cl = find_nearest_cluster( samples[i], nearest);
			
			if( labels_[i] != cl)
			{
				labels_[i] = cl;
                ++num_swaps;
            }
        }

        return num_swaps;
    }
	
	const cluster_t *find_nearest_cluster( const sample_type& s, int& nearest_index) const
	{
		const_iterator it( clusters().begin());
		real_type min_dist = traits_type::distance( s, it->mean);
		nearest_index = 0;
		const cluster_t *cluster = &(*it);
		++it;
		
		for( int index = 0; it != clusters().end(); ++it, ++index)
		{
			real_type d = traits_type::distance( s, it->mean);

			if( d < min_dist)
			{
				min_dist = d;
				nearest_index = index;
				cluster = &(*it);
			}
		}
		
		return cluster;
	}
	
	float do_kmeans( const std::vector<sample_type>& samples)
	{
		for( int iters = 0; iters < max_iters_; ++iters)
		{
			int num_swaps = assign_nearest_cluster( samples);
			
			if( num_swaps == 0)
				break;

			for( typename std::list<cluster_t>::iterator it( clusters_.begin()); it != clusters_.end(); ++it)
			{
				it->num_samples = 0;
				it->radius = traits_type::zero();
				
				for( int i = 0; i < samples.size(); ++i)
				{
					if( labels_[i] == &(*it))
						it->add_sample( samples[i]);
				}
				
				if( !it->empty())
					it->mean /= it->num_samples;

				for( int i = 0; i < samples.size(); ++i)
				{
					if( labels_[i] == &(*it))
						it->update_radius( samples[i]);
				}					
			}
			
			clusters_.erase( std::remove_if( clusters_.begin(), clusters_.end(),
											boost::bind( &kmeans_t::cluster_t::empty, _1)), clusters_.end());
			
			RAMEN_ASSERT( !clusters_.empty());
		}

		float max_radius = traits_type::zero();
		
		for( const_iterator it( begin()); it != end(); ++it)
			max_radius = std::max( max_radius, it->radius);
		
		return max_radius;
	}
	
    std::size_t K_;
    int max_iters_;
    int max_runs_;

    // samples
	std::vector<const cluster_t*> labels_;

	// random
	boost::mt19937 random_generator_;
	
	// clusters
	std::list<cluster_t> clusters_, best_run_;
	real_type max_radius_, run_radius_;
};
	
} // namespace
} // namespace

#endif
