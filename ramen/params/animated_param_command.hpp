// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIMATED_PARAM_COMMAND_HPP
#define	RAMEN_ANIMATED_PARAM_COMMAND_HPP

#include<ramen/params/animated_param_command_fwd.hpp>

#include<ramen/undo/command.hpp>

#include<memory>
#include<vector>

#include<ramen/params/param_set_fwd.hpp>

#include<ramen/params/poly_param_value.hpp>
#include<ramen/anim/float_curve.hpp>

namespace ramen
{

class animated_param_t;

class animated_param_command_t : public undo::command_t
{
public:

    animated_param_command_t( param_set_t& pset, const std::string& id);

    virtual void undo();
    virtual void redo();

private:

    void swap_curves();

    param_set_t& pset_;
    std::string id_;

    ramen::poly_param_value_t previous_value_, new_value_;
    std::vector<anim::float_curve_t> old_;
};

} // namespace

#endif
