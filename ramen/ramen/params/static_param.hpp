// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_STATIC_PARAM_HPP
#define	RAMEN_STATIC_PARAM_HPP

#include<ramen/params/param.hpp>

namespace ramen
{

class RAMEN_API static_param_t : public param_t
{
    Q_OBJECT

public:

    explicit static_param_t( const std::string& name);

protected:

    static_param_t( const static_param_t& other);
    void operator=( const static_param_t& other);

    virtual std::auto_ptr<undo::command_t> do_create_command();
};

} // namespace

#endif
