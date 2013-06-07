// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_FACTORY_HPP
#define RAMEN_FLIPBOOK_FACTORY_HPP

#include<vector>
#include<memory>

#include<boost/noncopyable.hpp>
#include<boost/function.hpp>
#include<boost/filesystem/path.hpp>

#include<ramen/flipbook/flipbook.hpp>

namespace ramen
{
namespace flipbook
{

class factory_t : boost::noncopyable
{
public:

    static factory_t& instance();

	typedef boost::function<flipbook_t* ( int, const std::string&, const std::string&)> create_fun_type;
	
    bool register_flipbook( const std::string& id, const create_fun_type& f);

	typedef std::vector<std::pair<std::string,create_fun_type> >::const_iterator const_iterator;
	const std::vector<std::pair<std::string,create_fun_type> >& flipbooks() const { return flipbooks_;}
		
	flipbook_t *create( int frame_rate, const std::string& display_device, const std::string& display_transform) const;
	
	flipbook_t *create( const std::string& id, int frame_rate, const std::string& display_device, 
						const std::string& display_transform) const;

	const boost::filesystem::path& flipbooks_dir() const;
	
private:

	int find_flipbook( const std::string& id) const;
	
    factory_t();
    ~factory_t();
	
	std::vector<std::pair<std::string,create_fun_type> > flipbooks_;
	mutable boost::filesystem::path flipbooks_dir_;
};

} // namespace
} // namespace

#endif
