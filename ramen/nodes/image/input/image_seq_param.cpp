// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/input/image_seq_param.hpp>

#include<boost/filesystem/operations.hpp>

#include<QPushButton>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/line_edit.hpp>

namespace ramen
{

image_seq_param_t::image_seq_param_t( const std::string& name, int proxy_level) : static_param_t( name)
{
	proxy_level_ = proxy_level;
}

image_seq_param_t::image_seq_param_t( const image_seq_param_t& other) : static_param_t( other)
{
	proxy_level_ = other.proxy_level_;
	input_text_ = other.input_text_;
    input_ = 0;
    button_ = 0;
}

void image_seq_param_t::set_input_text( const std::string& str)
{
	input_text_ = str;
	
	if( input_)
		input_->setText( QString::fromStdString( input_text_));
}

QWidget *image_seq_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    input_ = new ui::line_edit_t( top);
    button_ = new QPushButton( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
	label->setToolTip( id().c_str());
	
    button_->move( app().ui()->inspector().width() - s.height() - 10, 0);
    button_->resize( s.height(), s.height());
    button_->setText( "...");
    button_->setEnabled( enabled());
    connect( button_, SIGNAL( clicked()), this, SLOT( select_pushed()));

    input_->move( app().ui()->inspector().left_margin(), 0);
    input_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin()
                    - button_->width() - 10, s.height());

	input_->setReadOnly( true);
    input_->setEnabled( enabled());
	input_->setText( QString::fromStdString( input_text_));

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void image_seq_param_t::do_enable_widgets( bool e)
{
    if( input_)
    {
        input_->setEnabled( e);
        button_->setEnabled( e);
    }
}

void image_seq_param_t::select_pushed()
{
	boost::filesystem::path p;
	bool sequence, relative;
	
	if( app().ui()->image_sequence_file_selector( p, sequence, relative))
		file_picked( p, proxy_level_, sequence, relative);
}

} // namespace
