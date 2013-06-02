// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ui/widgets/line_edit.hpp>

#include<QContextMenuEvent>

namespace ramen
{
namespace ui
{

line_edit_t::line_edit_t( QWidget *parent) : QLineEdit( parent)
{
}

line_edit_t::line_edit_t( const QString& contents, QWidget *parent) : QLineEdit( contents, parent)
{
}

void line_edit_t::contextMenuEvent( QContextMenuEvent *event)
{
	event->ignore();
}

} // ui
} // ramen
