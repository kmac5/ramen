// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_OCIO_MANAGER_HPP
#define	RAMEN_OCIO_MANAGER_HPP

#include<ramen/ocio/manager_fwd.hpp>

#include<vector>
#include<string>

#include<boost/filesystem/path.hpp>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include<ramen/ocio/context.hpp>

namespace ramen
{
namespace ocio
{

class manager_t : boost::noncopyable
{
public:

	manager_t();
	~manager_t();

	OCIO::ConstConfigRcPtr config() const;

	const std::vector<std::string>& displays() const	{ return displays_;}
	const std::string& default_display() const			{ return displays()[default_display_index()];}
	int default_display_index() const					{ return default_display_index_;}

	void get_views( const std::string& display, std::vector<std::string>& views, int& default_index) const;
	std::string default_view( const std::string& display) const;

	OCIO::ConstContextRcPtr get_context( const context_t *ctx = 0) const;

private:

	void init();

	bool init_from_file( const boost::filesystem::path& p);

	void get_displays();

	std::string default_display_;
	int default_display_index_;
    std::vector<std::string> displays_;
};

} // ocio
} // ramen

#endif
