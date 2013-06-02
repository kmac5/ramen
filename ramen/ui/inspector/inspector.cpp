// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/inspector/inspector.hpp>

#include<boost/bind.hpp>

#include<QApplication>
#include<QFontMetrics>
#include<QVBoxLayout>
#include<QLabel>
#include<QScrollArea>
#include<QFrame>
#include<QPushButton>
#include<QMessageBox>

#include<ramen/nodes/node.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/panel.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>
#include<ramen/ui/widgets/line_edit.hpp>

#include<ramen/util/string.hpp>

namespace ramen
{
namespace ui
{
namespace
{

class rename_node_command_t : public undo::command_t
{
public:

	rename_node_command_t( node_t *n, const std::string& new_name, ui::line_edit_t *name_edit) : undo::command_t( "Rename Node")
	{
		n_ = n;
		old_name_ = n_->name();
		new_name_ = new_name;
		name_edit_ = name_edit;
	}

    virtual void undo()
	{
		rename( old_name_);
		undo::command_t::undo();
	}

    virtual void redo()
	{
		rename( new_name_);
		undo::command_t::redo();
	}

private:

	void rename( const std::string& name)
	{
	    name_edit_->blockSignals( true);
		app().document().composition().rename_node( n_, name);

		if( n_->is_active())
		{
			name_edit_->setText( n_->name().c_str());
			app().ui()->anim_editor().node_renamed( n_);
		}

		app().ui()->update();
		name_edit_->setModified( false);
	    name_edit_->blockSignals( false);
	}

	node_t *n_;
	std::string new_name_;
	std::string old_name_;

	ui::line_edit_t *name_edit_;
};

} // unnamed

inspector_t::inspector_t() : window_(0), left_margin_( 0), width_( 0)
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0);

    window_ = new QWidget();
    window_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred);
    window_->setMinimumSize( width()+16, 0);
    window_->setMaximumSize( width()+16, QWIDGETSIZE_MAX);
    window_->setWindowTitle( "Inspector");
    window_->setContentsMargins( 0, 0, 0, 0);

    scroll_ = new QScrollArea();
    scroll_->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded);
    scroll_->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    scroll_->setWidgetResizable( true);
    scroll_->setContentsMargins( 0, 0, 0, 0);

    QVBoxLayout *layout2 = new QVBoxLayout();
    layout2->setContentsMargins( 0, 0, 0, 0);

    QWidget *top = new QWidget();
    create_header();
    layout2->addWidget( header_);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout2->addWidget( separator);

    view_ = new container_widget_t();
    layout2->addWidget( view_);
    top->setLayout( layout2);
    scroll_->setWidget( top);

    layout->addWidget( scroll_);
    window_->setLayout( layout);

	current_ = factory_.end();
}

inspector_t::~inspector_t()
{
    name_edit_->deleteLater();
}

int inspector_t::left_margin() const
{
    if( !left_margin_)
    {
        QLabel *tmp = new QLabel();
        tmp->setText( "M");
        QSize s = tmp->sizeHint();
        delete tmp;

        const int max_label_lenght = 16;
        left_margin_ = s.width() * max_label_lenght;
    }

    return left_margin_;
}

int inspector_t::width() const
{
    if( !width_)
    {
        double_spinbox_t *tmp = new double_spinbox_t();
        QSize s = tmp->sizeHint();
        delete tmp;

		width_ = left_margin() + 5 + ( 3 * s.height()) + ( 3 * s.width()) + 30;
    }

    return width_;
}

void inspector_t::create_header()
{
    header_ = new QWidget();

    name_edit_ = new ui::line_edit_t( header_);
	
	help_ = new QPushButton( header_);
	help_->setText( "Help");
	QSize s = help_->sizeHint();

    int hpos = left_margin() / 2;
    int vsize = name_edit_->sizeHint().height();

    QLabel *label = new QLabel( header_);
    label->setText( "Name");
    label->move( 0, 5);
    label->resize( hpos - 5, vsize);
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);

    name_edit_->move( hpos, 5);
    name_edit_->resize( width() - hpos - 15 - s.width() - 5, vsize);
	name_edit_->setEnabled( false);
    connect( name_edit_, SIGNAL( editingFinished()), this, SLOT( rename_node()));
	hpos = hpos + width() - hpos - 15 - s.width();

	help_->move( hpos, 5);
	help_->setEnabled( false);
	connect( help_, SIGNAL( clicked()), this, SLOT( show_help()));
	
    header_->setMinimumSize( width(), vsize + 10);
    header_->setMaximumSize( width(), vsize + 10);
    header_->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void inspector_t::edit_node( node_t *n)
{
    if( app().quitting())
        return;

    if( current_ != factory_.end())
    {
        view_->clear_contents();
        current_ = factory_.end();
    }

    if( n != 0)
    {
        current_ = factory_.create_panel( n);
        view_->set_contents( current_->second->widget());
    }

    update_header_widgets();
}

void inspector_t::update()
{
    if( current_ != factory_.end())
        current_->second->update();
}

void inspector_t::recreate_panel( node_t *n)
{
    edit_node( 0);
	factory_.delete_panel( n);
    edit_node( n);
}

void inspector_t::update_header_widgets()
{
    name_edit_->blockSignals( true);

    node_t *n = app().ui()->active_node();

    if( n)
	{
        name_edit_->setText( n->name().c_str());
		name_edit_->setEnabled( true);
		help_->setEnabled( n->help_string());			
	}
    else
	{
        name_edit_->setText( "");
		name_edit_->setEnabled( false);
		help_->setEnabled( false);
	}

    name_edit_->blockSignals( false);
}

void inspector_t::rename_node()
{
	if( name_edit_->isModified())
	{
	    node_t *n = app().ui()->active_node();
		RAMEN_ASSERT( n);

		std::string new_name = name_edit_->text().toStdString();

        if( util::is_string_valid_identifier( new_name))
		{
			std::auto_ptr<rename_node_command_t> c( new rename_node_command_t( n, new_name, name_edit_));
			c->redo();
			app().document().undo_stack().push_back( c);
		    app().ui()->update();
		}
		else
		{
			name_edit_->blockSignals( true);
			name_edit_->setText( n->name().c_str());
			name_edit_->blockSignals( false);
		}
	}
}

void inspector_t::show_help()
{
	node_t *n = app().ui()->active_node();

	RAMEN_ASSERT( n);
	RAMEN_ASSERT( n->help_string());
	
	QMessageBox msg_box;
	msg_box.setWindowTitle( "Help");
	msg_box.setText( n->help_string());
	msg_box.exec();
}

} // namespace
} // namespace
