// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_PROPORTIONAL_PARAM_HPP
#define	RAMEN_PROPORTIONAL_PARAM_HPP

#include<ramen/params/numeric_param.hpp>

#include<OpenEXR/ImathVec.h>

#include<QPointer>
#include<QToolButton>

namespace ramen
{

class RAMEN_API proportional_param_t : public numeric_param_t
{
    Q_OBJECT

public:

    explicit proportional_param_t( const std::string& name);

    bool proportional() const	    { return flags() & proportional_bit;}
    void set_proportional( bool p);

    bool proportional_checked() const;

private Q_SLOTS:

	void proportional_toggle( bool state);
		
protected:

    proportional_param_t( const proportional_param_t& other);
    void operator=( const proportional_param_t& other);

    void create_proportional_button( QWidget *parent, int height);

    QPointer<QToolButton> prop_button_;

    static Imath::V3f proportional_factor;
};

} // namespace

#endif
