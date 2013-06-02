// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#include<ramen/noise/catrom2.hpp>

#include<cmath>

namespace ramen
{
namespace noise
{

float catrom2_t::table_[401];
	
catrom2_t::catrom2_t() { init_table();}

void catrom2_t::init_table()
{
	static bool inited = false;
	
	if( !inited)
	{
		for( int i = 0; i < num_entries(); i++)
		{
            double x = i / ( double) sample_rate();
            x = std::sqrt( x);
			
            if (x < 1)
                table_[i] = 0.5 * (2+x*x*(-5+x*3));
            else
                table_[i] = 0.5 * (4+x*(-8+x*(5-x)));
		}

		inited = true;
	}
}

} // noise
} // ramen
