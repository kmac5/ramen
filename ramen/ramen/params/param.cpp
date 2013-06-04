// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/param.hpp>

#include<stdexcept>

#include<ramen/params/parameterised.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/util/string.hpp>
#include<ramen/util/flags.hpp>

namespace ramen
{

param_t::param_t() : QObject(), param_set_(0)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit | include_in_hash_bit;
}

param_t::param_t( const std::string& name) : QObject(), param_set_(0), name_(name)
{
    flags_ = persist_bit | can_undo_bit | enabled_bit | include_in_hash_bit;
}

param_t::param_t( const param_t& other) : QObject(), param_set_(0), id_( other.id_), name_( other.name_),
                                            value_( other.value_), flags_( other.flags_), expressions_( other.expressions_)
{
    //RAMEN_ASSERT( 0 && "warning: implement expressions copy!");
}

void param_t::init()    { do_init();}
void param_t::do_init() {}

void param_t::set_param_set( param_set_t *parent)
{
    param_set_ = parent;
    do_set_param_set( parent);
}

void param_t::do_set_param_set( param_set_t *parent) {}

const parameterised_t *param_t::parameterised() const
{
    RAMEN_ASSERT( param_set());
    return param_set()->parent();
}

parameterised_t *param_t::parameterised()
{
    RAMEN_ASSERT( param_set());
    return param_set()->parent();
}

const composition_t *param_t::composition() const
{
    RAMEN_ASSERT( param_set());
    const parameterised_t *p = parameterised();

    if( p)
        return p->composition();
    else
        return 0;
}

composition_t *param_t::composition()
{
    RAMEN_ASSERT( param_set());
    parameterised_t *p = parameterised();

    if( p)
        return p->composition();
    else
        return 0;
}

void param_t::set_id( const std::string& identifier)
{
    RAMEN_ASSERT( util::is_string_valid_identifier( identifier));

    if( !util::is_string_valid_identifier( identifier))
        throw std::runtime_error( "Invalid id for param_t");

    id_ = identifier;
}

bool param_t::enabled() const { return util::test_flag( flags_, enabled_bit);}

void param_t::set_enabled( bool e)
{
    util::set_flag( flags_, enabled_bit, e);
    enable_widgets( enabled());
}

bool param_t::is_static() const      { return util::test_flag( flags_, static_bit);}

void param_t::set_static( bool b)
{
    util::set_flag( flags_, static_bit, b);
}

bool param_t::secret() const		{ return util::test_flag( flags_, secret_bit);}

void param_t::set_secret( bool b)
{
    util::set_flag( flags_, secret_bit, b);
}

bool param_t::persist() const	{ return util::test_flag( flags_, persist_bit);}

void param_t::set_persist( bool b)
{
    util::set_flag( flags_, persist_bit, b);
}

bool param_t::can_undo() const	{ return util::test_flag( flags_, can_undo_bit);}

void param_t::set_can_undo( bool b)
{
    util::set_flag( flags_, can_undo_bit, b);
}

bool param_t::include_in_hash() const { return util::test_flag( flags_, include_in_hash_bit);}

void param_t::set_include_in_hash( bool b)
{
    util::set_flag( flags_, include_in_hash_bit, b);
}

bool param_t::can_have_expressions() const	{ return util::test_flag( flags_, can_have_expressions_bit);}

void param_t::set_can_have_expressions( bool b)
{
    util::set_flag( flags_, can_have_expressions_bit, b);
}

bool param_t::track_mouse() const
{
    if( const parameterised_t *p = parameterised())
    {
        if( !p->track_mouse())
            return false;
    }

    return !( flags_ & dont_track_mouse_bit);
}

void param_t::set_track_mouse( bool b)
{
    util::set_flag( flags_, dont_track_mouse_bit, b);
}

void param_t::emit_param_changed( change_reason reason)
{
    if( reason != silent_edit)
    {
        if( param_set_t *p = param_set())
            p->param_changed( this, reason);
    }
}

void param_t::format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale)
{
    do_format_changed( new_format, aspect, proxy_scale);
}

void param_t::do_format_changed( const Imath::Box2i& new_format, float aspect, const Imath::V2f& proxy_scale) {}

void param_t::add_to_hash( hash::generator_t& hash_gen) const
{
    do_add_to_hash( hash_gen);
}

void param_t::do_add_to_hash( hash::generator_t& hash_gen) const {}

void param_t::create_tracks( anim::track_t *parent)
{
    if( !is_static())
        do_create_tracks( parent);
}

void param_t::do_create_tracks( anim::track_t *parent) {}

void param_t::set_frame( float frame)
{
    evaluate( frame);
    do_set_frame( frame);
    emit_param_changed( time_changed);
}

void param_t::do_set_frame( float frame) {}

void param_t::evaluate( float frame)	{ do_evaluate( frame);}
void param_t::do_evaluate( float frame)	{}

int param_t::num_expressions() const { return expressions_.size();}

const expressions::expression_t& param_t::expression( int indx) const
{
    RAMEN_ASSERT( indx >= 0 && indx < num_expressions());
    return expressions_[indx].second;
}

expressions::expression_t& param_t::expression( int indx)
{
    RAMEN_ASSERT( indx >= 0 && indx < num_expressions());
    return expressions_[indx].second;
}

void param_t::add_expression( const std::string& name)
{
    expressions_.push_back( std::make_pair( boost::flyweight<std::string>( name), expressions::expression_t()));
}

bool param_t::eval_expression( int index, float frame, float& v, ui::param_spinbox_t *widget) const
{
    RAMEN_ASSERT( index < num_expressions());

    //if( expression( index).empty())
    //	return false;

    /*
    try
    {
        expressions::context_t context;
        context.current = 0; // <-- TODO: do something with this.
        context.frame = frame;

        expressions::expression_t::result_type r( expression( index).eval( context));
        v = boost::get<float>( r);
        return true;
    }
    catch( std::exception& e)
    {
    }
    */

    return false;
}

void param_t::read_expressions( const serialization::yaml_node_t& node)
{
    /*
    serialization::optional_yaml_node_t expr( node.get_optional_node( "expressions"));

    if( expr)
    {
        for( serialization::yaml_node_t::const_iterator it( expr.get().begin()); it != expr.get().end(); ++it)
        {
            std::string key;
            it.first() >> key;

            if( expressions::expression_t *e = find_expression( key))
            {
                std::string str;
                it.second() >> str;
                //e->set_string( str);
            }
            else
                node.error_stream() << "Unknown expression " << key << " found in param.";
        }
    }
    */
}

void param_t::write_expressions( serialization::yaml_oarchive_t& out) const
{
    /*
    bool all_empty = true;

    for( int i = 0; i < num_expressions(); ++i)
    {
        if( !expression( i).empty())
        {
            all_empty = false;
            break;
        }
    }

    if( all_empty)
        return;

    out << YAML::Key << "expessions" << YAML::Value;
    out.begin_map();

    for( int i = 0; i < num_expressions(); ++i)
    {
        if( !expressions_[i].second.empty())
        {
            out << YAML::Key << expressions_[i].first
                    << YAML::Value << YAML::DoubleQuoted << expressions_[i].second.string();
        }
    }

    out.end_map();
    */
}

expressions::expression_t *param_t::find_expression( const std::string& name)
{
    for( int i = 0; i < num_expressions(); ++i)
    {
        if( expressions_[i].first == name)
            return &( expressions_[i].second);
    }

    return 0;
}

// undo
std::auto_ptr<undo::command_t> param_t::create_command() { return do_create_command();}

std::auto_ptr<undo::command_t> param_t::do_create_command()
{
    return std::auto_ptr<undo::command_t>();
}

// paths
void param_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
    do_convert_relative_paths( old_base, new_base);
}

void param_t::do_convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base) {}

void param_t::make_paths_absolute()     { do_make_paths_absolute();}
void param_t::do_make_paths_absolute()  {}

void param_t::make_paths_relative()     { do_make_paths_relative();}
void param_t::do_make_paths_relative()  {}

// util
void param_t::apply_function( const boost::function<void ( param_t*)>& f)
{
    f( this);
    do_apply_function( f);
}

void param_t::do_apply_function( const boost::function<void ( param_t*)>& f) {}

// python interop
boost::python::object param_t::to_python( const poly_param_value_t& v) const
{
    PyErr_SetString( PyExc_TypeError, "Can't convert value from C++");
    boost::python::throw_error_already_set();
    return boost::python::object();
}

poly_param_value_t param_t::from_python( const boost::python::object& obj) const
{
    throw std::bad_cast();
    return poly_param_value_t();
}

// serialization
void param_t::read(const serialization::yaml_node_t& in) { do_read( in);}

void param_t::do_read(const serialization::yaml_node_t& in)
{
    RAMEN_ASSERT( 0 && "We should never get here");
}

void param_t::write( serialization::yaml_oarchive_t& out) const
{
    if( !value().is_empty() && persist())
    {
        out.begin_map();
            out << YAML::Key << "id" << YAML::Value << id();
            do_write( out);
        out.end_map();
    }
}

void param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    RAMEN_ASSERT( 0 && "We should never get here");
}

// widgets
QWidget *param_t::create_widgets()
{
    if( !secret())
        return do_create_widgets();

    return 0;
}
QWidget *param_t::do_create_widgets() { return 0;}

void param_t::update_widgets()      { do_update_widgets();}
void param_t::do_update_widgets()   {}

void param_t::enable_widgets( bool e)       { do_enable_widgets( e);}
void param_t::do_enable_widgets( bool e)    {}

param_t *new_clone( const param_t& other) { return other.clone();}

} // namespace
