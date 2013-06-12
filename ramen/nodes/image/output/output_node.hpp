// Copyright (c) 2010 Esteban Tovagliari.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_OUTPUT_NODE_HPP
#define	RAMEN_IMAGE_OUTPUT_NODE_HPP

#include<ramen/nodes/image/base_output_node.hpp>

namespace ramen
{
namespace image
{

class output_node_t : public base_output_node_t
{
public:

    static const node_metaclass_t& output_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    output_node_t();
	
protected:

    output_node_t( const output_node_t& other);
    void operator=( const output_node_t&);

private:

    node_t *do_clone() const { return new output_node_t( *this);}

    virtual void do_create_params();

    void param_changed( param_t *p, param_t::change_reason reason);

    std::string extension_for_format( int format) const;
    void set_format_for_extension( const std::string& ext);

    virtual void do_calc_defined( const render::context_t& context);
	
    virtual void write( const render::context_t& context);
	
	virtual int priority() const;	
};

} // image
} // ramen

#endif
