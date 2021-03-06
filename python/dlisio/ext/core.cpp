#include <bitset>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <datetime.h>

#include <dlisio/dlisio.h>
#include <dlisio/types.h>

namespace py = pybind11;
using namespace py::literals;

#include <dlisio/ext/exception.hpp>
#include <dlisio/ext/io.hpp>
#include <dlisio/ext/types.hpp>

using File = dl::basic_file< std::ifstream >;

namespace pybind11 { namespace detail {
template <> struct type_caster< dl::dtime > {
public:
    PYBIND11_TYPE_CASTER(dl::dtime, _("datetime.datetime"));

    static handle cast( dl::dtime src, return_value_policy, handle ) {
        // TODO: add TZ info
        return PyDateTime_FromDateAndTime( src.Y,
                                           src.M,
                                           src.D,
                                           src.H,
                                           src.MN,
                                           src.S,
                                           src.MS );
    }
};
}} // namespace pybind11::detail

namespace {

namespace conv {

int sshort( const char*& xs ) noexcept;
int snorm( const char*& xs ) noexcept;
long slong( const char*& xs ) noexcept;

int ushort( const char*& xs ) noexcept;
int unorm( const char*& xs ) noexcept;
long ulong( const char*& xs ) noexcept;

float fshort( const char*& xs ) noexcept;
float fsingl(  const char*& xs ) noexcept;
double fdoubl( const char*& xs ) noexcept;

float isingl(  const char*& xs ) noexcept;
float vsingl(  const char*& xs ) noexcept;

std::pair< float, float > fsing1( const char*& xs ) noexcept;
std::tuple< float, float, float > fsing2( const char*& xs ) noexcept;
std::complex< float > csingl( const char*& xs ) noexcept;

std::pair< double, double > fdoub1( const char*& xs ) noexcept;
std::tuple< double, double, double > fdoub2( const char*& xs ) noexcept;
std::complex< double > cdoubl( const char*& xs ) noexcept;

long uvari( const char*& xs ) noexcept;

std::string ident( const char*& xs );
std::string ascii( const char*& xs );

dl::dtime dtime( const char*& xs ) noexcept;

long origin( const char*& xs ) noexcept;

std::tuple< long, int, std::string > obname( const char*& xs );
std::tuple< long, int, std::string > obname( const char*& xs, int nmemb );
std::tuple< std::string, long, int, std::string > objref( const char*& xs );

int status( const char*& xs ) noexcept;

int sshort( const char*& xs ) noexcept {
    std::int8_t x;
    xs = dlis_sshort( xs, &x );
    return x;
}

int snorm( const char*& xs ) noexcept {
    std::int16_t x;
    xs = dlis_snorm( xs, &x );
    return x;
}

long slong( const char*& xs ) noexcept {
    std::int32_t x;
    xs = dlis_slong( xs, &x );
    return x;
}

int ushort( const char*& xs ) noexcept {
    std::uint8_t x;
    xs = dlis_ushort( xs, &x );
    return x;
}

int unorm( const char*& xs ) noexcept {
    std::uint16_t x;
    xs = dlis_unorm( xs, &x );
    return x;
}

long ulong( const char*& xs ) noexcept {
    std::uint32_t x;
    xs = dlis_ulong( xs, &x );
    return x;
}

float fshort( const char*& xs ) noexcept {
    float x;
    xs = dlis_fshort( xs, &x );
    return x;
}

float fsingl(  const char*& xs ) noexcept {
    float x;
    xs = dlis_fsingl( xs, &x );
    return x;
}

double fdoubl( const char*& xs ) noexcept {
    double x;
    xs = dlis_fdoubl( xs, &x );
    return x;
}

float isingl(  const char*& xs ) noexcept {
    float x;
    xs = dlis_isingl( xs, &x );
    return x;
}

float vsingl(  const char*& xs ) noexcept {
    float x;
    xs = dlis_vsingl( xs, &x );
    return x;
}

std::pair< float, float > fsing1( const char*& xs ) noexcept {
    float V, A;
    xs = dlis_fsing1( xs, &V, &A );
    return { V, A };
}

std::tuple< float, float, float > fsing2( const char*& xs ) noexcept {
    float V, A, B;
    xs = dlis_fsing2( xs, &V, &A, &B );
    return std::make_tuple( V, A, B );
}

std::complex< float > csingl( const char*& xs ) noexcept {
    float R, I;
    xs = dlis_csingl( xs, &R, &I );
    return { R, I };
}

std::pair< double, double > fdoub1( const char*& xs ) noexcept {
    double V, A;
    xs = dlis_fdoub1( xs, &V, &A );
    return std::make_pair( V, A );
}

std::tuple< double, double, double > fdoub2( const char*& xs ) noexcept {
    double V, A, B;
    xs = dlis_fdoub2( xs, &V, &A, &B );
    return std::make_tuple( V, A, B );
}

std::complex< double > cdoubl( const char*& xs ) noexcept {
    double R, I;
    xs = dlis_cdoubl( xs, &R, &I );
    return { R, I };
}

long uvari( const char*& xs ) noexcept {
    std::int32_t x;
    xs = dlis_uvari( xs, &x );
    return x;
}

std::string ident( const char*& xs ) {
    char str[ 256 ];
    std::int32_t len;

    dlis_ident( xs, &len, nullptr );
    xs = dlis_ident( xs, &len, str );

    return { str, str + len };
}

std::string ascii( const char*& xs ) {
    std::vector< char > str;
    std::int32_t len;

    dlis_ascii( xs, &len, nullptr );
    str.resize( len );
    xs = dlis_ascii( xs, &len, str.data() );

    return { str.begin(), str.end() };
}

dl::dtime dtime( const char*& xs ) noexcept {
    dl::dtime dt;
    xs = dlis_dtime( xs, &dt.Y,
                         &dt.TZ,
                         &dt.M,
                         &dt.D,
                         &dt.H,
                         &dt.MN,
                         &dt.S,
                         &dt.MS );
    dt.Y = dlis_year( dt.Y );
    return dt;
}

long origin( const char*& xs ) noexcept {
    std::int32_t x;
    xs = dlis_origin( xs, &x );
    return x;
}

std::tuple< long, int, std::string > obname( const char*& xs ) {
    char str[ 256 ];
    std::int32_t len;

    std::int32_t orig;
    std::uint8_t copy;

    xs = dlis_obname( xs, &orig, &copy, &len, str );
    return std::make_tuple( orig, copy, std::string( str, str + len ) );
}

std::tuple< long, int, std::string > obname( const char*& xs, int nmemb ) {
    if( nmemb < 4 ) {
        /*
         * safeguard against too-few-bytes to read the integer parts of the obname
         */
        std::string msg = "obname is minimum 5 bytes, nmemb was "
                        + std::to_string( nmemb );
        throw std::length_error( msg );
    }

    char str[ 256 ] = {};
    std::int32_t orig;
    std::uint8_t copy;
    std::int32_t len;
    std::uint8_t name_len;

    const auto* ptr = xs;
    ptr = dlis_origin( ptr, &orig );
    ptr = dlis_ushort( ptr, &copy );
    ptr = dlis_ushort( ptr, &name_len );
    const auto int_len = std::distance( xs, ptr );

    if( int_len + name_len > nmemb ) {
        std::string msg = "expected obname length (= "
                        + std::to_string(int_len + name_len) + ") < nmemb ("
                        + "= " + std::to_string( nmemb ) + ")";
        throw std::length_error( msg );
    }

    // TODO: stronger exception guarantee?
    xs = dlis_ident( ptr - 1, &len, str );
    return std::make_tuple( orig, copy, str );
}

std::tuple< std::string, long, int, std::string > objref( const char*& xs ) {
    char strid[ 256 ];
    char strobj[ 256 ];
    std::int32_t lenid;
    std::int32_t lenobj;

    std::int32_t orig;
    std::uint8_t copy;

    xs = dlis_objref( xs,
                      &lenid, strid,
                      &orig, &copy, &lenobj, strobj );

    return std::make_tuple(
        std::string( strid, strid + lenid ),
        orig, copy, std::string( strobj, strobj + lenobj )
    );
}

int status( const char*& xs ) noexcept {
    std::uint8_t x;
    xs = dlis_status( xs, &x );
    return x;
}

}

py::dict SUL( const char* buffer ) {
    char id[ 61 ] = {};
    int seqnum, major, minor, layout;
    int64_t maxlen;

    auto err = dlis_sul( buffer, &seqnum,
                                 &major,
                                 &minor,
                                 &layout,
                                 &maxlen,
                                 id );


    switch( err ) {
        case DLIS_OK: break;

        // TODO: report more precisely - a lot of stuff can go wrong with the
        // SUL
        if( err == DLIS_UNEXPECTED_VALUE )
            throw py::value_error( "unable to parse storage unit label (SUL)" );

        case DLIS_INCONSISTENT:
            runtime_warning( "storage unit label inconsistent with "
                             "specification - falling back to assuming DLIS v1"
            );
            break;
    }

    std::string version = std::to_string( major )
                        + "."
                        + std::to_string( minor );

    std::string laystr = "record";
    if( layout != DLIS_STRUCTURE_RECORD ) laystr = "unknown";

    return py::dict( "sequence"_a = seqnum,
                     "version"_a = version.c_str(),
                     "layout"_a = laystr.c_str(),
                     "maxlen"_a = maxlen,
                     "id"_a =  id );
}

struct segheader {
    std::uint8_t attrs;
    int len;
    int type;
};


class file {
public:
    explicit file( const std::string& path );

    void close() { this->fs.close(); }

    py::dict sul();
    py::tuple mkindex();
    py::bytes raw_record( const dl::bookmark& );
    py::dict eflr( const dl::bookmark& );
    py::object iflr_chunk( const dl::bookmark& mark, const std::vector< std::tuple< int, int > >&, int, int );


private:
    File fs;
};

file::file( const std::string& path ) : fs( path ) {}

py::dict file::sul() {
    auto sulbuffer = this->fs.read< 80 >();
    return SUL( sulbuffer.data() );
}

py::tuple file::mkindex() {
    std::vector< dl::bookmark > bookmarks;
    std::vector< py::dict > explicits;
    py::dict implicit_refs;
    int remaining = 0;

    while( !this->fs.eof() ) {
        auto mark = tag( this->fs, remaining );
        bookmarks.push_back( std::move( mark.second ) );
        remaining = mark.first;

        const auto& last = bookmarks.back();

        if ( last.isencrypted ) continue;

        if( last.isexplicit ) try {
            explicits.push_back( this->eflr( last ) );
        } catch( std::exception& e ) {
            py::print(e.what(), " at ", bookmarks.size() );
        }

        if ( last.isexplicit ) continue;

        const auto name = py::make_tuple(
            static_cast< std::int32_t >( last.name.origin ),
            static_cast< std::uint8_t >( last.name.copy ),
            static_cast< const std::string& >( last.name.id )
        );

        if( !implicit_refs.contains( name ) )
            implicit_refs[ name ] = py::list();

        implicit_refs[ name ].cast< py::list >().append( last );
    }

    return py::make_tuple( bookmarks, explicits, implicit_refs );
}

py::object convert( int reprc, py::buffer b ) {
    const auto* xs = static_cast< const char* >( b.request().ptr );
    switch( reprc ) {
        case DLIS_FSHORT: return py::cast( conv::fshort( xs ) );
        case DLIS_FSINGL: return py::cast( conv::fsingl( xs ) );
        case DLIS_FSING1: return py::cast( conv::fsing1( xs ) );
        case DLIS_FSING2: return py::cast( conv::fsing2( xs ) );
        case DLIS_ISINGL: return py::cast( conv::isingl( xs ) );
        case DLIS_VSINGL: return py::cast( conv::vsingl( xs ) );
        case DLIS_FDOUBL: return py::cast( conv::fdoubl( xs ) );
        case DLIS_FDOUB1: return py::cast( conv::fdoub1( xs ) );
        case DLIS_FDOUB2: return py::cast( conv::fdoub2( xs ) );
        case DLIS_CSINGL: return py::cast( conv::csingl( xs ) );
        case DLIS_CDOUBL: return py::cast( conv::cdoubl( xs ) );
        case DLIS_SSHORT: return py::cast( conv::sshort( xs ) );
        case DLIS_SNORM:  return py::cast( conv:: snorm( xs ) );
        case DLIS_SLONG:  return py::cast( conv:: slong( xs ) );
        case DLIS_USHORT: return py::cast( conv::ushort( xs ) );
        case DLIS_UNORM:  return py::cast( conv:: unorm( xs ) );
        case DLIS_ULONG:  return py::cast( conv:: ulong( xs ) );
        case DLIS_UVARI:  return py::cast( conv:: uvari( xs ) );
        case DLIS_IDENT:  return py::cast( conv:: ident( xs ) );
        case DLIS_ASCII:  return py::cast( conv:: ascii( xs ) );
        case DLIS_DTIME:  return py::cast( conv:: dtime( xs ) );
        case DLIS_STATUS: return py::cast( conv::status( xs ) );
        case DLIS_ORIGIN: return py::cast( conv::origin( xs ) );
        case DLIS_OBNAME: return py::cast( conv::obname( xs ) );
        case DLIS_OBJREF: return py::cast( conv::objref( xs ) );
        case DLIS_UNITS:  return py::cast( conv:: ident( xs ) );

        default:
            throw py::value_error( "unknown representation code "
                                 + std::to_string( reprc ) );
    }
}

py::list getarray( const char*& xs, int count, int reprc ) {
    py::list l;

    for( int i = 0; i < count; ++i ) {
        switch( reprc ) {
            case DLIS_FSHORT: l.append( conv::fshort( xs ) ); break;
            case DLIS_FSINGL: l.append( conv::fsingl( xs ) ); break;
            case DLIS_FSING1: l.append( conv::fsing1( xs ) ); break;
            case DLIS_FSING2: l.append( conv::fsing2( xs ) ); break;
            case DLIS_ISINGL: l.append( conv::isingl( xs ) ); break;
            case DLIS_VSINGL: l.append( conv::vsingl( xs ) ); break;
            case DLIS_FDOUBL: l.append( conv::fdoubl( xs ) ); break;
            case DLIS_FDOUB1: l.append( conv::fdoub1( xs ) ); break;
            case DLIS_FDOUB2: l.append( conv::fdoub2( xs ) ); break;
            case DLIS_CSINGL: l.append( conv::csingl( xs ) ); break;
            case DLIS_CDOUBL: l.append( conv::cdoubl( xs ) ); break;
            case DLIS_SSHORT: l.append( conv::sshort( xs ) ); break;
            case DLIS_SNORM:  l.append( conv:: snorm( xs ) ); break;
            case DLIS_SLONG:  l.append( conv:: slong( xs ) ); break;
            case DLIS_USHORT: l.append( conv::ushort( xs ) ); break;
            case DLIS_UNORM:  l.append( conv:: unorm( xs ) ); break;
            case DLIS_ULONG:  l.append( conv:: ulong( xs ) ); break;
            case DLIS_UVARI:  l.append( conv:: uvari( xs ) ); break;
            case DLIS_IDENT:  l.append( conv:: ident( xs ) ); break;
            case DLIS_ASCII:  l.append( conv:: ascii( xs ) ); break;
            case DLIS_DTIME:  l.append( conv:: dtime( xs ) ); break;
            case DLIS_STATUS: l.append( conv::status( xs ) ); break;
            case DLIS_ORIGIN: l.append( conv::origin( xs ) ); break;
            case DLIS_OBNAME: l.append( conv::obname( xs ) ); break;
            case DLIS_OBJREF: l.append( conv::objref( xs ) ); break;
            case DLIS_UNITS:  l.append( conv:: ident( xs ) ); break;

            default:
                throw py::value_error( "unknown representation code "
                                     + std::to_string( reprc ) );
        }
    }

    return l;
}

void skiparray( const char*& xs, int count, int reprc ) {
    for( int i = 0; i < count; ++i ) {
        switch( reprc ) {
            case DLIS_FSHORT: conv::fshort( xs ); break;
            case DLIS_FSINGL: conv::fsingl( xs ); break;
            case DLIS_FSING1: conv::fsing1( xs ); break;
            case DLIS_FSING2: conv::fsing2( xs ); break;
            case DLIS_ISINGL: conv::isingl( xs ); break;
            case DLIS_VSINGL: conv::vsingl( xs ); break;
            case DLIS_FDOUBL: conv::fdoubl( xs ); break;
            case DLIS_FDOUB1: conv::fdoub1( xs ); break;
            case DLIS_FDOUB2: conv::fdoub2( xs ); break;
            case DLIS_CSINGL: conv::csingl( xs ); break;
            case DLIS_CDOUBL: conv::cdoubl( xs ); break;
            case DLIS_SSHORT: conv::sshort( xs ); break;
            case DLIS_SNORM:  conv:: snorm( xs ); break;
            case DLIS_SLONG:  conv:: slong( xs ); break;
            case DLIS_USHORT: conv::ushort( xs ); break;
            case DLIS_UNORM:  conv:: unorm( xs ); break;
            case DLIS_ULONG:  conv:: ulong( xs ); break;
            case DLIS_UVARI:  conv:: uvari( xs ); break;
            case DLIS_IDENT:  conv:: ident( xs ); break;
            case DLIS_ASCII:  conv:: ascii( xs ); break;
            case DLIS_DTIME:  conv:: dtime( xs ); break;
            case DLIS_STATUS: conv::status( xs ); break;
            case DLIS_ORIGIN: conv::origin( xs ); break;
            case DLIS_OBNAME: conv::obname( xs ); break;
            case DLIS_OBJREF: conv::objref( xs ); break;
            case DLIS_UNITS:  conv:: ident( xs ); break;

            default:
                throw py::value_error( "unknown representation code "
                                     + std::to_string( reprc ) );
        }
    }
}

struct setattr {
    int type, name;
};

setattr set_attributes( const char*& cur ) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, sizeof( std::uint8_t ) );
    cur += sizeof( std::uint8_t );

    int role;
    dlis_component( attr, &role );

    switch( role ) {
        case DLIS_ROLE_RDSET:
        case DLIS_ROLE_RSET:
        case DLIS_ROLE_SET:
            break;

        default:
            throw py::value_error(
                std::string("first item in EFLR not SET, RSET or RDSET, was ")
                + dlis_component_str( role )
                + "(" + std::bitset< sizeof( attr ) >( role ).to_string() + ")"
            );
    }

    setattr flags;
    const auto err = dlis_component_set( attr, role, &flags.type, &flags.name );

    switch( err ) {
        case DLIS_OK:
            break;

        case DLIS_INCONSISTENT:
            user_warning( "SET:type not set, but must be non-null." );
            flags.type = 1;
            break;

        default:
            throw std::runtime_error( "unhandled error in dlis_component_set" );
    }

    return flags;
}

struct attribattr {
    int label;
    int count;
    int reprc;
    int units;
    int value;
    int object = 0;
    int absent = 0;
    int invariant = 0;
};

attribattr attrib_attributes( const char*& cur ) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, sizeof( std::uint8_t ) );

    int role;
    dlis_component( attr, &role );

    attribattr flags;
    switch( role ) {
        case DLIS_ROLE_ABSATR:
            flags.absent= 1;
            break;

        case DLIS_ROLE_OBJECT:
            flags.object = 1;
            break;

        case DLIS_ROLE_INVATR:
            flags.invariant = 1;

        case DLIS_ROLE_ATTRIB:
            break;

        default:
            throw py::value_error(
                std::string("expected ATTRIB, INVATR, or OBJECT, was ")
                + dlis_component_str( role )
                + "(" + std::bitset< sizeof( attr ) >( role ).to_string() + ")"
            );
    }

    /*
     * only consume the component tag if it is not object, because if it is
     * then the next function assumes its there
     */
    if( role != DLIS_ROLE_OBJECT )
        cur += sizeof( std::uint8_t );

    if( flags.object || flags.absent ) return flags;

    const auto err = dlis_component_attrib( attr, role, &flags.label,
                                                        &flags.count,
                                                        &flags.reprc,
                                                        &flags.units,
                                                        &flags.value );

    if( !err ) return flags;

    // all sources for this error should've been checked, so
    // something is REALLY wrong if we end up here
    throw std::runtime_error( "unhandled error in dlis_component_attrib" );
}

void object_attributes( const char*& cur ) {
    std::uint8_t attr;
    std::memcpy( &attr, cur, sizeof( std::uint8_t ) );

    cur += sizeof( std::uint8_t );

    int role;
    dlis_component( attr, &role );

    if( role != DLIS_ROLE_OBJECT )
        throw py::value_error( std::string("expected OBJECT, was ")
                             + dlis_component_str( role ) );

    int obname;
    const auto err = dlis_component_object( attr, role, &obname );

    if( err ) user_warning( "OBJECT:name not set, but must be non-null" );
}

struct object_template {
    std::vector< py::dict > attribute;
    std::vector< py::dict > invariant;
};

object_template explicit_template( const char*& cur, const char* end ) {
    object_template cols;

    while( true ) {
        if( cur >= end )
            throw py::value_error( "unexpected end-of-record in template" );

        auto flags = attrib_attributes( cur );

        if( flags.object ) return cols;

        if( flags.absent ) {
            user_warning( "ABSATR in object template - skipping" );
            continue;
        }

        int count = 1;
        int reprc = DLIS_IDENT;
        /* set the global defaults unconditionally */
        py::dict col( "count"_a = count,
                      "reprc"_a = reprc,
                      "value"_a = py::none() );

        if( !flags.label ) {
            user_warning( "ATTRIB:label not set, but must be non-null" );
            flags.label = 1;
        }

                          col["label"] = conv::ident( cur );
        if( flags.count ) col["count"] = count = conv::uvari( cur );
        if( flags.reprc ) col["reprc"] = reprc = conv::ushort( cur );
        if( flags.units ) col["units"] = conv::ident( cur );
        if( flags.value ) col["value"] = getarray( cur, count, reprc );

        if( flags.invariant ) cols.invariant.push_back( std::move( col ) );
        else                  cols.attribute.push_back( std::move( col ) );
    }
}

py::dict deepcopy( const py::dict& d ) {
    return py::reinterpret_steal< py::dict >( PyDict_Copy( d.ptr() ) );
}

template< typename T >
std::vector< T > deepcopy( const std::vector< T >& xs ) {
    std::vector< T > ys;
    ys.reserve( xs.size() );

    for( const auto& x : xs )
        ys.push_back( deepcopy( x ) );

    return ys;
}

py::dict eflr( const char* cur, const char* end ) {
    if( std::distance( cur, end ) == 0 )
        throw py::value_error( "eflr must be non-empty" );

    py::dict record;
    auto set = set_attributes( cur );
    if( cur >= end ) {
        throw py::value_error( "unexpected end-of-record "
                                 "after SET component" );
    }

    if( set.type ) record["type"] = conv::ident( cur );
    if( set.name ) record["name"] = conv::ident( cur );

    auto tmpl = explicit_template( cur, end );

    if( cur >= end )
        throw py::value_error( "unexpected end-of-record after template" );

    py::dict objects;
    while( true ) {
        if( cur == end ) break;

        object_attributes( cur );

        /*
         * just assume obname. objects have to specify it, and if it is unset
         * then UserWarning has already been emitted
         */
        auto name = conv::obname( cur );

        /*
         * each object is a row in a table of attributes. If the object is cut
         * short (terminated by a new object), use the defaults from the template
         */
        auto row = deepcopy( tmpl.attribute );

        for( auto& cell : row ) {
            if( cur == end ) break;

            auto flags = attrib_attributes( cur );

            if( flags.object ) break;

            if( flags.absent ) {
                cell["value"] = py::none();
                continue;
            }

            if( flags.label ) {
                user_warning( "ATTRIB:label set, but must be null - was "
                            + conv::ident( cur ) );
            }

            if( flags.count ) cell["count"] = conv::uvari( cur );
            if( flags.reprc ) cell["reprc"] = conv::ushort( cur );
            if( flags.units ) cell["units"] = conv::ident( cur );
            if( flags.value ) {
                /*
                 * count/repr can both be set by this label, and by the
                 * template, so the simplest thing is to just look them up in
                 * the dict
                 */
                auto count = cell["count"].cast< int >();
                auto reprc = cell["reprc"].cast< int >();
                cell["value"] = getarray( cur, count, reprc );
            }
        }

        /* patch invariant-attributes onto the record */
        row.insert( row.end(), tmpl.invariant.begin(), tmpl.invariant.end() );
        const auto pyname = py::cast( name );
        objects[pyname] = row;
    }

    record["template-attribute"] = tmpl.attribute;
    record["template-invariant"] = tmpl.invariant;
    record["objects"] = objects;
    return record;
}

std::vector< char > catrecord( File& fp, int remaining ) {

    std::vector< char > cat;
    cat.reserve( 8192 );

    while( true ) {

        while( remaining > 0 ) {

            auto seg = segment_header( fp );
            remaining -= seg.len;

            int explicit_formatting = 0;
            int has_predecessor = 0;
            int has_successor = 0;
            int is_encrypted = 0;
            int has_encryption_packet = 0;
            int has_checksum = 0;
            int has_trailing_length = 0;
            int has_padding = 0;
            dlis_segment_attributes( seg.attrs, &explicit_formatting,
                                                &has_predecessor,
                                                &has_successor,
                                                &is_encrypted,
                                                &has_encryption_packet,
                                                &has_checksum,
                                                &has_trailing_length,
                                                &has_padding );


            seg.len -= 4; // size of LRSH
            const auto prevsize = cat.size();
            cat.resize( prevsize + seg.len );
            fp.read( cat.data() + prevsize, seg.len );

            if( has_trailing_length ) cat.erase( cat.end() - 2, cat.end() );
            if( has_checksum )        cat.erase( cat.end() - 2, cat.end() );
            if( has_padding ) {
                std::uint8_t padbytes = 0;
                dlis_ushort( cat.data() + cat.size() - 1, &padbytes );
                cat.erase( cat.end() - padbytes, cat.end() );
            }

            if( !has_successor ) return cat;
        }

        remaining = visible_length( fp ) - 4;
    }
}

py::bytes file::raw_record( const dl::bookmark& m ) {
    this->fs.seek( m.tell );

    auto cat = catrecord( this->fs, m.residual );
    return py::bytes( cat.data(), cat.size() );
}

py::dict file::eflr( const dl::bookmark& mark ) {
    if( mark.isencrypted ) return py::none();
    this->fs.seek( mark.tell );

    auto cat = catrecord( this->fs, mark.residual );
    return ::eflr( cat.data(), cat.data() + cat.size() );
}

py::object file::iflr_chunk( const dl::bookmark& mark,
                             const std::vector< std::tuple< int, int > >& pre,
                             int elems,
                             int dtype ) {

    if( mark.isencrypted ) return py::none();

    this->fs.seek( mark.tell );

    auto cat = catrecord( this->fs, mark.residual );
    const char* ptr = cat.data();

    conv::obname( ptr );
    auto frameno = conv::uvari( ptr );

    const auto is_constant_size = [](int reprc) {
        switch( reprc ) {
            case DLIS_FSHORT:
            case DLIS_FSINGL:
            case DLIS_FSING1:
            case DLIS_FSING2:
            case DLIS_ISINGL:
            case DLIS_VSINGL:
            case DLIS_FDOUBL:
            case DLIS_FDOUB1:
            case DLIS_FDOUB2:
            case DLIS_CSINGL:
            case DLIS_CDOUBL:
            case DLIS_SSHORT:
            case DLIS_SNORM :
            case DLIS_SLONG :
            case DLIS_USHORT:
            case DLIS_UNORM :
            case DLIS_ULONG :
            case DLIS_DTIME :
                return true;
            default:
                return false;
        }
    };

    const auto reprsize = [](int reprc) {
        switch( reprc ) {
            case DLIS_USHORT:
            case DLIS_SSHORT:
                return 1;

            case DLIS_FSHORT:
            case DLIS_SNORM :
            case DLIS_UNORM :
                return 2;

            case DLIS_FSINGL:
            case DLIS_ISINGL:
            case DLIS_VSINGL:
            case DLIS_SLONG :
            case DLIS_ULONG :
                return 4;

            case DLIS_FDOUBL:
            case DLIS_FSING1:
            case DLIS_CSINGL:
            case DLIS_DTIME :
                return 8;

            case DLIS_FSING2:
                return 12;

            case DLIS_FDOUB1:
            case DLIS_CDOUBL:
                return 16;

            case DLIS_FDOUB2:
                return 24;

            default:
                throw std::runtime_error( "unknown reprc" );
        }
    };

    bool constant_size = true;
    int size = 0;
    for( const auto& pair : pre ) {
        const auto count = std::get< 1 >( pair );
        const auto reprc = std::get< 1 >( pair );
        constant_size = constant_size && is_constant_size( reprc );

        size += count * reprsize( reprc );
    }

    if ( !constant_size ) {
        for( auto& pair : pre ) {
            auto count = std::get< 0 >( pair );
            auto reprc = std::get< 1 >( pair );

            skiparray( ptr, count, reprc );
        }
    }
    else {
        ptr += size;
    }

    return getarray( ptr, elems, dtype );
}

}

PYBIND11_MODULE(core, m) {
    PyDateTime_IMPORT;

    py::register_exception_translator( []( std::exception_ptr p ) {
        try {
            if( p ) std::rethrow_exception( p );
        } catch( const io_error& e ) {
            PyErr_SetString( PyExc_IOError, e.what() );
        } catch( const eof_error& e ) {
            PyErr_SetString( PyExc_EOFError, e.what() );
        }
    });

    py::class_< dl::bookmark >( m, "bookmark" )
        .def_readwrite( "encrypted", &dl::bookmark::isencrypted )
        .def_readwrite( "explicit",  &dl::bookmark::isexplicit )
        .def( "__repr__", []( const dl::bookmark& m ) {
            auto pos = " pos=" + std::to_string( m.tell );
            auto enc = std::string(" encrypted=") +
                     (m.isencrypted ? "True": "False");
            auto exp = std::string(" explicit=") +
                     (m.isexplicit ? "True": "False");
            return "<dlisio.core.bookmark" + pos + enc + exp + ">";
        })
    ;

    m.def( "sul", []( const std::string& b ) {
        if( b.size() < 80 ) {
            throw py::value_error(
                    "SUL object too small: expected 80, was "
                    + std::to_string( b.size() ) );
        }
        return SUL( b.data() );
    } );

    m.def( "eflr", []( py::buffer b ) {
        const auto info = b.request();
        const auto len = info.size * info.itemsize;
        const auto ptr = static_cast< char* >( info.ptr );
        return eflr( ptr, ptr + len );
    } );

    m.def( "conv", convert );

    py::class_< file >( m, "file" )
        .def( py::init< const std::string& >() )
        .def( "close", &file::close )

        .def( "sul",        &file::sul )
        .def( "mkindex",    &file::mkindex )
        .def( "raw_record", &file::raw_record )
        .def( "eflr",       &file::eflr )
        .def( "iflr",       &file::iflr_chunk )
        ;
}
