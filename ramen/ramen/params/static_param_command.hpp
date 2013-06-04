// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_STATIC_PARAM_COMMAND_HPP
#define RAMEN_STATIC_PARAM_COMMAND_HPP

#include<string>

#include<ramen/undo/command.hpp>

#include<ramen/params/poly_param_value.hpp>

#include<ramen/params/param_set_fwd.hpp>
#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

class static_param_command_t : public undo::command_t
{
public:

    static_param_command_t( param_set_t& pset, const std::string& id);

    virtual void undo();
    virtual void redo();

protected:

    param_set_t& pset_;
    std::string id_;
    poly_param_value_t old_value_, new_value_;
};

} // namespace

#endif
