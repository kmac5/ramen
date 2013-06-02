// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_WIDGETS_LINE_EDIT_HPP
#define RAMEN_UI_WIDGETS_LINE_EDIT_HPP

#include<ramen/ui/widgets/line_edit_fwd.hpp>

#include<QLineEdit>

namespace ramen
{
namespace ui
{

class line_edit_t : public QLineEdit
{
    Q_OBJECT

public:

	line_edit_t( QWidget *parent = 0);
	line_edit_t( const QString& contents, QWidget *parent = 0);
	
protected:
	
	virtual void contextMenuEvent( QContextMenuEvent *event);
};

} // ui
} // ramen

#endif
