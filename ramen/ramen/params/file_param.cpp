// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/file_param.hpp>

#include<boost/filesystem/operations.hpp>

#include<QPushButton>
#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>
#include<QLabel>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/line_edit.hpp>

namespace ramen
{

file_param_t::file_param_t( const std::string& name) : static_param_t( name)
{
    is_input_ = true;
    set_default_value( boost::filesystem::path());
    ext_list_string_ = "File (*.*)";
}

file_param_t::file_param_t( const file_param_t& other) : static_param_t( other)
{
    is_input_ = other.is_input_;
    ext_list_string_ = other.ext_list_string_;
    input_ = 0;
    button_ = 0;
}

void file_param_t::set_default_value( const boost::filesystem::path& p) { value().assign( p);}

void file_param_t::set_value( const boost::filesystem::path& p, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    value().assign( p);
    emit_param_changed( reason);
}

bool file_param_t::empty() const
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    return p.empty();
}

bool file_param_t::file_exists() const
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));

    if( !p.empty())
        return boost::filesystem::exists( p);

    return false;
}

std::string file_param_t::extension() const
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    return p.extension().string();
}

void file_param_t::set_extension( const std::string& ext)
{
    RAMEN_ASSERT( !is_input_);
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));
    p.replace_extension( ext);
    value().assign( p);
    update_input_text();
}

void file_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    if( is_input_)
        hash_gen << filesystem::hash_string( get_value<boost::filesystem::path>( *this));
}

boost::python::object file_param_t::to_python( const poly_param_value_t& v) const
{
    std::string str = filesystem::file_string( v.cast<boost::filesystem::path>());
    return boost::python::object( str);
}

poly_param_value_t file_param_t::from_python( const boost::python::object& obj) const
{
    std::string str = boost::python::extract<std::string>( obj);
    boost::filesystem::path p( str);
    return poly_param_value_t( p);
}

void file_param_t::do_read( const serialization::yaml_node_t& node)
{
    std::string val;
    node.get_value<std::string>( "value", val);
    set_value( boost::filesystem::path( val), silent_edit);
}

void file_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value"
            << YAML::Value << YAML::DoubleQuoted
            << filesystem::file_string( get_value<boost::filesystem::path>( *this));
}

void file_param_t::do_update_widgets()
{
    if( input_)
        update_input_text();
}

void file_param_t::do_enable_widgets( bool e)
{
    if( input_)
    {
        input_->setEnabled( e);
        button_->setEnabled( e);
    }
}

void file_param_t::update_input_text( const boost::filesystem::path& p)
{
    if( input_)
    {
        input_->blockSignals( true);
        input_->setText( filesystem::file_cstring( p));
        input_->blockSignals( false);
    }
}

void file_param_t::update_input_text()
{
    update_input_text( get_value<boost::filesystem::path>( *this));
}

void file_param_t::do_convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));

    if( p.is_relative())
    {
        value().assign( filesystem::convert_relative_path( p, old_base, new_base));
        update_widgets();
    }
}

void file_param_t::do_make_paths_absolute()
{
    RAMEN_ASSERT( composition());

    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));

    if( p.is_relative())
    {
        value().assign( composition()->relative_to_absolute( p));
        update_widgets();
    }
}

void file_param_t::do_make_paths_relative()
{
    RAMEN_ASSERT( composition());

    boost::filesystem::path p( get_value<boost::filesystem::path>( *this));

    if( p.is_absolute())
    {
        value().assign( composition()->absolute_to_relative( p));
        update_widgets();
    }
}

QWidget *file_param_t::do_create_widgets()
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

    if( is_input_)
        input_->setReadOnly( true);

    input_->setEnabled( enabled());

    update_input_text();

    connect( input_, SIGNAL( editingFinished()), this, SLOT( text_changed()));

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void file_param_t::select_pushed()
{
    QString fname;
    static bool was_relative = false;
    bool relative  = false;

    if( is_input_)
    {
        QFileDialog dialog( 0, "Select file", QString::null, ext_list_string_.c_str());
        dialog.setOption( QFileDialog::DontUseNativeDialog, true);
        dialog.setFileMode( QFileDialog::ExistingFile);

        QCheckBox *relative_check = new QCheckBox( "Relative");

        if( app().document().has_file())
            relative_check->setChecked( was_relative);
        else
        {
            was_relative = false;
            relative_check->setChecked( false);
            relative_check->setEnabled( false);
        }

        QGridLayout *glayout = (QGridLayout *) dialog.layout();
        glayout->addWidget( relative_check, glayout->rowCount(), 0, 1, glayout->columnCount(), Qt::AlignLeft);

        dialog.show();

        if( dialog.exec())
        {
            QStringList filenames = dialog.selectedFiles();
            fname = filenames[0];
            relative = relative_check->isChecked();
            was_relative = relative;
        }
    }
    else
        fname = QFileDialog::getSaveFileName( 0, "Select file", QString::null, ext_list_string_.c_str(),
                                              0, QFileDialog::DontUseNativeDialog);

    if( !fname.isEmpty())
    {
        boost::filesystem::path p( fname.toStdString());

        if( is_input_ && relative)
        {
            RAMEN_ASSERT( composition());
            p = composition()->absolute_to_relative( p);
        }

        update_input_text( p);

        param_set()->begin_edit();
        set_value( p);
        param_set()->end_edit();
    }
}

void file_param_t::text_changed()
{
    std::string fname( input_->text().toStdString());
    boost::filesystem::path p( fname);
    param_set()->begin_edit();
    set_value( p);
    param_set()->end_edit();
}

} // namespace
