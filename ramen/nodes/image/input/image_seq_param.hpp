// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SEQ_PARAM_HPP
#define	RAMEN_IMAGE_SEQ_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<boost/signals2/signal.hpp>

#include<QPointer>

#include<ramen/filesystem/path.hpp>

#include<ramen/ui/widgets/line_edit_fwd.hpp>

class QPushButton;

namespace ramen
{

class image_seq_param_t : public static_param_t
{
    Q_OBJECT
    
public:

    image_seq_param_t( const std::string& name, int proxy_level);

    boost::signals2::signal<void ( const::boost::filesystem::path&, int, bool, bool)> file_picked;

	void set_input_text( const std::string& str);
	
protected:

    image_seq_param_t( const image_seq_param_t& other);
    void operator=( const image_seq_param_t& other);

private Q_SLOTS:

    void select_pushed();
	
private:

    virtual param_t *do_clone() const { return new image_seq_param_t( *this);}
	
    virtual QWidget *do_create_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<ui::line_edit_t> input_;
    QPointer<QPushButton> button_;
	int proxy_level_;
	std::string input_text_;
};

} // namespace

#endif
