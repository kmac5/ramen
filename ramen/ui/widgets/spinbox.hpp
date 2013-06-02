// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_WIDGETS_SPINBOX_HPP
#define RAMEN_UI_WIDGETS_SPINBOX_HPP

#include<ramen/ui/widgets/spinbox_fwd.hpp>

#include<QLineEdit>
#include<QAction>

#include<ramen/assert.hpp>

#include<ramen/util/calculator.hpp>

namespace ramen
{
namespace ui
{

class spinbox_t : public QLineEdit
{
    Q_OBJECT

public:

	spinbox_t( QWidget *parent = 0);

	virtual QSize sizeHint() const;
	
	int decimals() const		{ return decimals_;}
	void setDecimals( int n)
	{
		RAMEN_ASSERT( n >= 0);
		decimals_ = n;
	}
		
	double minimum() const		{ return min_;}
	void setMinimum( double m)	{ min_ = m;}
	
	double maximum() const		{ return max_;}
	void setMaximum( double m)	{ max_ = m;}
	
	void setRange( double lo, double hi)
	{
		RAMEN_ASSERT( lo <= hi);
		min_ = lo;
		max_ = hi;
	}

	double singleStep() const		{ return step_;}
	void setSingleStep( double s)	{ step_ = s;}

	bool trackMouse() const		{ return track_mouse_;}
	void setTrackMouse( bool b)	{ track_mouse_ = b;}
	
protected:

	void setLineEditContents( double v);
	
    const util::calculator_t& calc() const { return calc_;}
	
	QChar thousandSeparator() const;
	QChar decimalSeparator() const;

	int abs( int x) const
	{
		if( x < 0)
			return -x;
		
		return x;
	}
	
	QAction *copy_, *paste_;
	
	bool drag_;
	bool first_drag_;
	bool dragged_;
	int push_x_, last_x_;

private:

    static util::calculator_t calc_;
	static QChar thousand_sep_;
	static QChar decimal_sep_;
	
	double min_, max_;
	double step_;
	int decimals_;
	
	bool track_mouse_;	
};

} // ui
} // ramen

#endif
