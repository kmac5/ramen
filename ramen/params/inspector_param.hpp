// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_INSPECTOR_PARAM_HPP
#define	RAMEN_INSPECTOR_PARAM_HPP

#include<ramen/params/param.hpp>

#include<boost/signals2/connection.hpp>

#include<QPointer>

#include<ramen/ui/widgets/container_widget.hpp>

namespace ramen
{

class RAMEN_API inspector_param_t : public param_t
{
    Q_OBJECT
    
public:

    explicit inspector_param_t( const std::string& id);
	~inspector_param_t();
	
	const parameterised_t *parameterised() const	{ return contents_;}
	parameterised_t *parameterised()				{ return contents_;}
	
	void set_parameterised( parameterised_t *p);
	
protected:

    inspector_param_t( const inspector_param_t& other);
    void operator=( const inspector_param_t& other);
	
private:
	
    virtual param_t *do_clone() const { return new inspector_param_t( *this);}

    virtual QWidget *do_create_widgets();
	
	void parameterised_deleted( parameterised_t *p);
	
	QPointer<ui::container_widget_t> widget_;
	parameterised_t *contents_;
	boost::signals2::connection connection_;
};

} // namespace

#endif

