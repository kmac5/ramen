// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_INPUT_NODE_HPP
#define	RAMEN_IMAGE_INPUT_NODE_HPP

#include<ramen/nodes/image/image_node.hpp>

#include<boost/optional.hpp>

#include<ramen/imageio/factory.hpp>

namespace ramen
{

class image_input_node_t : public image_node_t
{
public:

    static const node_metaclass_t& image_input_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    image_input_node_t();
    image_input_node_t( const boost::filesystem::path& path, bool sequence);

    void param_changed( param_t *p);

    imageio::reader_t *reader() const { return reader_.get();}

    void set_channels( const std::string& red, const std::string& green,
			const std::string& blue, const std::string& alpha);

protected:

    image_input_node_t( const image_input_node_t& other);
    void operator=( const image_input_node_t&);

private:

    virtual node_t *do_clone() const { return new image_input_node_t( *this);}

    virtual void do_create_params();

	void create_more_params( float aspect = 1.0f);
	
    virtual void do_set_frame( float t);

    virtual bool do_is_valid() const;
    virtual void do_calc_hash_str( const render::context_t& context);

    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_defined( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

    Imath::Box2i real_defined_;
    float time_;
    boost::filesystem::path fname_;
    std::auto_ptr<imageio::reader_t> reader_;
};

} // namespace

#endif
