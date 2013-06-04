// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_WIDGETS_TEXT_EDIT_HPP
#define RAMEN_UI_WIDGETS_TEXT_EDIT_HPP

#include<QPlainTextEdit>

namespace ramen
{
namespace ui
{

class text_edit_t : public QPlainTextEdit
{
	Q_OBJECT
	
public:
	
	text_edit_t( QWidget *parent = 0);
	
protected:
	
	virtual void focusOutEvent( QFocusEvent *e);
	
Q_SIGNALS:

	void textHasChanged();
};

} // ui
} // ramen

#endif
