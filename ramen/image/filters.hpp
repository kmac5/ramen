// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_FILTERS_HPP
#define RAMEN_IMAGE_FILTERS_HPP

#include<vector>

namespace ramen
{
namespace image
{

struct filter_t
{
protected:

	filter_t();

	static int accuracy();
};

struct lanczos3_filter_t : public filter_t
{
public:

	lanczos3_filter_t() {}

	int filter_area( float scale) const;

	float operator()( float x) const;

	static void init_table();

private:

	static std::vector<float> ftable;
};

struct mitchell_filter_t : public filter_t
{
public:

	mitchell_filter_t() {}

	int filter_area( float scale) const;

	float operator()( float x) const;

	static void init_table();

private:

	static std::vector<float> ftable;
};

struct catrom_filter_t : public filter_t
{
public:

	catrom_filter_t() {}

	int filter_area( float scale) const;

	float operator()( float x) const;

	static void init_table();

private:

	static float catrom( float x);

	static std::vector<float> ftable;
};

} // image
} // ramen

#endif
