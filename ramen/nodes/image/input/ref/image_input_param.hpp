// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_INPUT_PARAM_HPP
#define	RAMEN_IMAGE_INPUT_PARAM_HPP

#include<ramen/config.hpp>

#include<ramen/params/static_param.hpp>

#include<vector>

#include<boost/tuple/tuple.hpp>
#include<boost/tuple/tuple_io.hpp>

#include<QPointer>
#include<QComboBox>

#include<ramen/filesystem/path_sequence.hpp>

#include<ramen/ui/widgets/line_edit_fwd.hpp>

namespace ramen
{

class image_input_param_t : public static_param_t
{
    Q_OBJECT

public:

    explicit image_input_param_t( const std::string& name);
    image_input_param_t( const std::string& name, const boost::filesystem::path& p, bool sequence, float& aspect);

    boost::filesystem::path path_for_frame( float f) const;
    boost::tuple<int,int,int,int> channels() const;

    void set_channels( const std::string& red, const std::string& green,
						const std::string& blue, const std::string& alpha);

    std::vector<std::string> channel_list() const;

	int start_frame() const { return start_frame_;}
	int end_frame() const	{ return end_frame_;}
	
    // paths
    virtual void convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base);
	virtual void make_paths_absolute();
	virtual void make_paths_relative();

protected:

    image_input_param_t( const image_input_param_t& other);
    void operator=( const image_input_param_t& other);

private Q_SLOTS:

    void select_pushed();
    void channel_picked( int index);

private:

    virtual param_t *do_clone() const;

	virtual void do_add_to_hash( hash_generator_t& hash_gen) const;

	virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();

    void update_info();

    void update_popup( QComboBox *popup, const std::vector<std::string>& ch_list, int value);

    filesystem::path_sequence_t sequence_for_path( const boost::filesystem::path& p) const;

    QPointer<ui::line_edit_t> input_, info_;
    QPointer<QComboBox> red_combo_, green_combo_, blue_combo_, alpha_combo_;
	
	int start_frame_, end_frame_;
};

} // namespace

#endif
