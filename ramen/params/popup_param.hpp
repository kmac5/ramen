// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_POPUP_PARAM_HPP
#define	RAMEN_POPUP_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<string>
#include<vector>

#include<boost/assign/list_of.hpp>

#include<QPointer>

class QComboBox;

namespace ramen
{

class RAMEN_API popup_param_t : public static_param_t
{
    Q_OBJECT

public:

    explicit popup_param_t( const std::string& name);

    void set_default_value( int x);

    void set_value( int x, change_reason reason = user_edited);

    void add_menu_item( const std::string& item);

    const std::vector<std::string>& menu_items() const	{ return menu_items_;}
    std::vector<std::string>& menu_items()				{ return menu_items_;}

protected:

    popup_param_t( const popup_param_t& other);
    void operator=( const popup_param_t& other);

private:

    virtual param_t *do_clone() const { return new popup_param_t( *this);}

    virtual void do_evaluate( float time) {}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    int find_index_for_string( const std::string& s) const;

    std::vector<std::string> menu_items_;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QComboBox> menu_;

private Q_SLOTS:

    void item_picked( int index);
};

} // namespace

#endif
