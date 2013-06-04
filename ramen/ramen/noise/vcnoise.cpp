// This code comes from the procedural texturing an modelling book.
// Add a proper copyright here.

#include<ramen/noise/vcnoise.hpp>

namespace ramen
{
namespace noise
{

vcnoise_t::vcnoise_t( int seed) { init( seed);}

float vcnoise_t::operator()(float x, float y, float z) const
{
    int ix, iy, iz;
    int i, j, k;
    float fx, fy, fz;
    float dx, dy, dz;
    float sum = 0;

    ix = floor( x);
    fx = x - ix;

    iy = floor( y);
    fy = y - iy;

    iz = floor( z);
    fz = z - iz;

    for (k = -1; k <= 2; k++) {
        dz = k - fz;
        dz = dz*dz;
        for (j = -1; j <= 2; j++) {
            dy = j - fy;
            dy = dy*dy;
            for (i = -1; i <= 2; i++){
                dx = i - fx;
                dx = dx*dx;
                sum += vlattice(ix+i,iy+j,iz+k)
                    * catrom_(dx + dy + dz);
            }
        }
    }
    return sum;
}

float vcnoise_t::vlattice( int ix, int iy, int iz) const
{
    return value_table_[ perm_( ix,iy,iz)];
}

void vcnoise_t::init(int seed)
{
	Imath::Rand48 rng( seed);
    float *table = value_table_;

    for(int i = 0; i < tabsize; i++)
        *table++ = 1. - 2.* rng.nextf();
}

} // noise
} // ramen
