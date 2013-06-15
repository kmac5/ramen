// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_CDL_NODE_HPP
#define RAMEN_IMAGE_CDL_NODE_HPP

#include<ramen/nodes/image/pointop_node.hpp>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include<ramen/filesystem/path.hpp>

namespace ramen
{
namespace image
{

class cdl_node_t : public pointop_node_t
{
public:

    static const node_metaclass_t& cdl_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    cdl_node_t();

	void read_from_file( const boost::filesystem::path& p);
	void write_to_file( const boost::filesystem::path& p) const;
	
protected:

    cdl_node_t( const cdl_node_t& other);
    void operator=( const cdl_node_t&);

private:

    node_t *do_clone() const { return new cdl_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const image::const_image_view_t& src,
                             const image::image_view_t& dst,
                             const render::context_t& context);
	
	OCIO::CDLTransformRcPtr cdl_transform() const;
};

} // image
} // ramen

#endif
