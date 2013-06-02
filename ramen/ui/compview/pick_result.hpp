// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UI_COMP_VIEW_PICK_RESULT_HPP
#define RAMEN_UI_COMP_VIEW_PICK_RESULT_HPP

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{
namespace ui
{

struct pick_result_t
{
    enum component_t
    {
        no_pick = 0,
        body_picked,
        input_picked,
        output_picked
    };

    pick_result_t() : node( 0), component( no_pick), plug_num( -1) {}

    node_t *node;
    component_t component;
    int plug_num;
};

} // ui
} // ramen

#endif
