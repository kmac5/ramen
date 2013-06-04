/*
 *  color_cluster.cpp
 *  fugu
 *
 */

#include"color_cluster.hpp"

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include<Accelerate/Accelerate.h>
#endif

namespace fugu
{

namespace stats
{

template<> long xspev( float *symm, int size, float *eigvals, float *eigvecs, float *work)
{
long info;

	sspev_( "V", "U", (__CLPK_integer *) &size, symm, eigvals, eigvecs, (__CLPK_integer *) &size, work, &info);
	return info;
}

template<> long xspev( double *symm, int size, double *eigvals, double *eigvecs, double *work)
{
long info;

	dspev_( "V", "U", (__CLPK_integer *) &size, symm, eigvals, eigvecs, (__CLPK_integer *) &size, work, &info);
	return info;
}

} // namespace
} // namespace
