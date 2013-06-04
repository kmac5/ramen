// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/ui/widgets/spinbox.hpp>

#include<limits>

#include<QLocale>

namespace ramen
{
namespace ui
{

util::calculator_t spinbox_t::calc_;
QChar spinbox_t::thousand_sep_ = 0;
QChar spinbox_t::decimal_sep_ = 0;
	
spinbox_t::spinbox_t( QWidget *parent) : QLineEdit( parent)
{
	decimals_ = 5;
	min_ = -std::numeric_limits<double>::max();
	max_ = -min_;
	step_ = 1;
	track_mouse_ = true;
	
	drag_ = false;
	dragged_ = false;
	first_drag_ = false;

	// create common actions
	copy_ = new QAction( "copy", this);
	paste_ = new QAction( "paste", this);	
}

QSize spinbox_t::sizeHint() const
{
    QSize s = QLineEdit::sizeHint();
    QFontMetrics fm( font());
    s.setWidth( fm.width( QLatin1Char('0')) * 8);
    return s;
}

void spinbox_t::setLineEditContents( double v)
{
	QString str = locale().toString( v, 'f', decimals()).remove( thousandSeparator());

	int index = str.indexOf( decimalSeparator());
	if( index != -1)
	{
		for( int i = str.size() - 1; i > index; --i)
		{
			if( str[i] == '0')
				str[i] = ' ';
			else
				break;
		}

		str = str.trimmed();
		
		if( str[str.size() - 1] == decimalSeparator())
			str.truncate( str.size() - 1);
	}

	setText( str);
	setModified( false);
}

QChar spinbox_t::thousandSeparator() const
{
	if( thousand_sep_ == 0)
	{
		QString str = locale().toString( 1100.0, 'f');
		thousand_sep_ = str[1];			
	}
	
	return thousand_sep_;
}

QChar spinbox_t::decimalSeparator() const
{
	if( decimal_sep_ == 0)
	{
		QString str = locale().toString( 0.1, 'f');
		decimal_sep_ = str[1];
	}
	
	return decimal_sep_;	
}

} // ui
} // ramen
