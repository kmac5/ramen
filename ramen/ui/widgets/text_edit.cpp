// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/ui/widgets/text_edit.hpp>

#include<QAction>
#include<QContextMenuEvent>
#include<QMenu>

namespace ramen
{
namespace ui
{

text_edit_t::text_edit_t( QWidget *parent) : QPlainTextEdit( parent)
{	
	setTabChangesFocus( true);
	document()->setModified( false);
}

void text_edit_t::focusOutEvent( QFocusEvent *e)
{
	if( document()->isModified())
	{
		document()->setModified( false);		
		textHasChanged();
	}

	QPlainTextEdit::focusOutEvent( e);
}

} // ui
} // ramen
