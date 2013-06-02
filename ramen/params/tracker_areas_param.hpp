// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_TRACKER_AREAS_PARAM_HPP
#define RAMEN_TRACKER_AREAS_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<algorithm>

#include<OpenEXR/ImathBox.h>

#include<QPointer>

#include<ramen/serialization/yaml.hpp>

#include<ramen/ui/widgets/double_spinbox_fwd.hpp>

namespace ramen
{

struct tracker_areas_t
{
	static float min_reference_radius();
	static float min_search_radius();
	
	tracker_areas_t() {}
	tracker_areas_t( int r, int s);
	tracker_areas_t( const Imath::Box2f& ref, const Imath::Box2f& srch);
		
	void set_areas( const Imath::Box2f& r, const Imath::Box2f& s);
	void set_reference_area( const Imath::Box2f& r);
	void set_search_area( const Imath::Box2f& s);

    bool operator==( const tracker_areas_t& other) const;
    bool operator!=( const tracker_areas_t& other) const;
		
	Imath::Box2f reference;
	Imath::Box2f search;
};

void operator>>( const YAML::Node& in, tracker_areas_t& areas);
YAML::Emitter& operator<<( YAML::Emitter& out, const tracker_areas_t& areas);

class RAMEN_API tracker_areas_param_t : public static_param_t
{
    Q_OBJECT

public:

	tracker_areas_param_t();

    void set_value( const tracker_areas_t& areas);
	
protected:

    tracker_areas_param_t( const tracker_areas_param_t& other);
    void operator=( const tracker_areas_param_t& other);

private Q_SLOTS:

    void value_changed( double v);
	
private:

    virtual param_t *do_clone() const { return new tracker_areas_param_t( *this);}

	virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;
	
    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<ui::double_spinbox_t> ref_minx_, ref_miny_, ref_maxx_, ref_maxy_;
    QPointer<ui::double_spinbox_t> search_minx_, search_miny_, search_maxx_, search_maxy_;
};

} // namespace

#endif
