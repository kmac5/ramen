// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/params/animated_param_command.hpp>

#include<boost/foreach.hpp>

#include<adobe/empty.hpp>

#include<ramen/params/animated_param.hpp>
#include<ramen/nodes/node.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{

animated_param_command_t::animated_param_command_t( param_set_t& pset, const std::string& id) : undo::command_t( "Param Changed"), pset_( pset)
{
    id_ = id;
    set_done( true);
    animated_param_t *p = dynamic_cast<animated_param_t*>( &pset_.find( id_));
    RAMEN_ASSERT( p);

    previous_value_ = p->value();

	for( int i = 0; i < p->num_curves(); ++i)
		old_.push_back( p->curve( i));
}

void animated_param_command_t::swap_curves()
{
    animated_param_t *p = dynamic_cast<animated_param_t*>( &pset_.find( id_));

    for( int i = 0; i < old_.size(); ++i)
		old_[i].swap( p->curve( i));
}

void animated_param_command_t::undo()
{
    param_t& p = pset_.find( id_);
    new_value_ = p.value();
    swap_curves();
    p.value() =  previous_value_;
    p.evaluate( app().document().composition().frame());
    p.update_widgets();
	p.emit_param_changed( param_t::user_edited);
    undo::command_t::undo();
}

void animated_param_command_t::redo()
{
    RAMEN_ASSERT( !new_value_.is_empty() && "Empty value in animated param command");
    
    param_t& p = pset_.find( id_);
    p.value() =  new_value_;
    swap_curves();
    p.evaluate( app().document().composition().frame());
    p.update_widgets();
	p.emit_param_changed( param_t::user_edited);
    undo::command_t::redo();
}

} // namespace
