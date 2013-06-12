// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_INPUT_NODE_HPP
#define	RAMEN_IMAGE_INPUT_NODE_HPP

#include<ramen/nodes/image_node.hpp>

#include<boost/optional.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/tuple/tuple.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/nodes/image/input/input_clip.hpp>

#include<ramen/nodes/image/input/image_input_command_fwd.hpp>

namespace ramen
{
namespace image
{

class input_node_t : public image_node_t
{
public:

    static const node_metaclass_t& image_input_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    input_node_t();
    input_node_t( const boost::filesystem::path& path, bool sequence,
						const boost::filesystem::path& from_dir);
	
	// set / get for undo
	boost::tuple<int,int,int,int> get_channels() const;
	void set_channels( const boost::tuple<int,int,int,int>& c);
		
    void set_channels( const std::string& red, const std::string& green,
						const std::string& blue, const std::string& alpha);

	float get_aspect_param_value() const;
	void set_aspect_param_value( float a);

	// add colorspace here
	
protected:

    input_node_t( const input_node_t& other);
    void operator=( const input_node_t&);

private:

	friend class undo::image_input_command_t;
	
    virtual node_t *do_clone() const { return new input_node_t( *this);}

    virtual void do_create_params();
	void create_image_params( const boost::filesystem::path& p = boost::filesystem::path());
	void create_more_params();
	
    void param_changed( param_t *p, param_t::change_reason reason);
	
    virtual void do_set_frame( float t);
	boost::optional<int> map_frame_to_sequence( float t) const;
	
    virtual bool do_is_valid() const;
	
    virtual void do_calc_hash_str( const render::context_t& context);

    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_defined( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

	// serialization	
	virtual void do_read( const serialization::yaml_node_t& node, const std::pair<int,int>& version);	
    virtual void do_write( serialization::yaml_oarchive_t& out) const;
	
	// paths
    virtual void convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base);
	virtual void make_paths_absolute();
	virtual void make_paths_relative();

	// misc
	const std::vector<input_clip_t>& clips() const	{ return clips_;}
	std::vector<input_clip_t>& clips()				{ return clips_;}
	
	int index_for_proxy_level( int proxy_level) const;
	void create_reader( int proxy_level);
	void create_reader( int proxy_level, const boost::filesystem::path& from_dir);
	
	void file_picked( const boost::filesystem::path& p, int level, bool sequence, bool relative);
	
	static const int num_proxy_levels;
	
    Imath::Box2i real_defined_;
	std::vector<input_clip_t> clips_;
	std::vector<boost::shared_ptr<movieio::reader_t> > readers_;
};

} // namespace
} // namespace

#endif
