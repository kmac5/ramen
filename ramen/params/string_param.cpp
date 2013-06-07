// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/string_param.hpp>

#include<ramen/nodes/node.hpp>

#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/line_edit.hpp>
#include<ramen/ui/widgets/text_edit.hpp>

namespace ramen
{

string_param_t::string_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( std::string());
    read_only_ = false;
    multiline_ = false;
}

string_param_t::string_param_t( const std::string& name, bool read_only) : static_param_t( name)
{
    set_default_value( std::string());
    multiline_ = false;
    read_only_ = read_only;

    if( read_only_)
    {
        set_can_undo( false);
        set_persist( false);
        set_include_in_hash( false);
    }
}

string_param_t::string_param_t( const string_param_t& other) : static_param_t( other)
{
    multiline_ = other.multiline_;
    read_only_ = other.read_only_;
    input_ = 0;
    multi_input_ = 0;
}

void string_param_t::set_default_value( const std::string& x) { value().assign( x);}

void string_param_t::set_value( const std::string& x, change_reason reason)
{
    if( read_only_)
    {
        set_default_value( x);
        update_widgets();
    }
    else
    {
        if( can_undo())
            param_set()->add_command( this);

        value().assign( x);
        emit_param_changed( reason);
    }
}

void string_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    hash_gen << get_value<std::string>( *this);
}

void string_param_t::do_read( const serialization::yaml_node_t& node)
{
    std::string val;
    node.get_value<std::string>( "value", val);
    set_value( val);
}

void string_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value"
        << YAML::Value << get_value<std::string>( *this);
}

QWidget *string_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    ui::line_edit_t *tmp = new ui::line_edit_t();
    QSize s = tmp->sizeHint();
    delete tmp;

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    std::string str = get_value<std::string>( *this);
    int height;

    if( multiline())
    {
        multi_input_ = new ui::text_edit_t( top);
        multi_input_->setReadOnly( read_only_);
        multi_input_->move( app().ui()->inspector().left_margin(), 0);
        multi_input_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height() * 7);
        multi_input_->setEnabled( enabled());
        multi_input_->setPlainText( str.c_str());
        connect( multi_input_, SIGNAL( textHasChanged()), this, SLOT( text_changed()));
        height = multi_input_->height();
    }
    else
    {
        input_ = new ui::line_edit_t( top);
        input_->setReadOnly( read_only_);
        input_->move( app().ui()->inspector().left_margin(), 0);
        input_->resize( app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10, s.height());
        input_->setEnabled( enabled());
        input_->setText( str.c_str());
        connect( input_, SIGNAL( editingFinished()), this, SLOT( text_changed()));
        height = s.height();
    }

    top->setMinimumSize( app().ui()->inspector().width(), height);
    top->setMaximumSize( app().ui()->inspector().width(), height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void string_param_t::do_update_widgets()
{
    if( input_)
    {
        input_->blockSignals( true);
        std::string str = get_value<std::string>( *this);
        input_->setText( str.c_str());
        input_->blockSignals( false);
    }
    else
    {
        if( multi_input_)
        {
            multi_input_->blockSignals( true);
            std::string str = get_value<std::string>( *this);
            multi_input_->setPlainText( str.c_str());
            multi_input_->blockSignals( false);
        }
    }
}

void string_param_t::do_enable_widgets( bool e)
{
    if( input_)
        input_->setEnabled( e);
    else
    {
        if( multi_input_)
            multi_input_->setEnabled( e);
    }
}

void string_param_t::text_changed()
{
    std::string str;

    if( multiline())
        str = multi_input_->toPlainText().toStdString();
    else
        str = input_->text().toStdString();

    param_set()->begin_edit();
    set_value( str);
    param_set()->end_edit();
}

} // namespace
