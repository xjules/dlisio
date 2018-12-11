#ifndef DLISIO_EXT_TYPES_HPP
#define DLISIO_EXT_TYPES_HPP

#include <complex>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

#include "strong-typedef.hpp"

namespace dl {

enum class representation_code : std::uint8_t {
    fshort = DLIS_FSHORT,
    fsingl = DLIS_FSINGL,
    fsing1 = DLIS_FSING1,
    fsing2 = DLIS_FSING2,
    isingl = DLIS_ISINGL,
    vsingl = DLIS_VSINGL,
    fdoubl = DLIS_FDOUBL,
    fdoub1 = DLIS_FDOUB1,
    fdoub2 = DLIS_FDOUB2,
    csingl = DLIS_CSINGL,
    cdoubl = DLIS_CDOUBL,
    sshort = DLIS_SSHORT,
    snorm  = DLIS_SNORM,
    slong  = DLIS_SLONG,
    ushort = DLIS_USHORT,
    unorm  = DLIS_UNORM,
    ulong  = DLIS_ULONG,
    uvari  = DLIS_UVARI,
    ident  = DLIS_IDENT,
    ascii  = DLIS_ASCII,
    dtime  = DLIS_DTIME,
    origin = DLIS_ORIGIN,
    obname = DLIS_OBNAME,
    objref = DLIS_OBJREF,
    attref = DLIS_ATTREF,
    status = DLIS_STATUS,
    units  = DLIS_UNITS,
};

namespace types {

#define DLIS_REGISTER_TYPEALIAS(name, type) \
    struct name : detail::strong_typedef< name, type > { \
        using detail::strong_typedef< name, type >::strong_typedef; \
    }

DLIS_REGISTER_TYPEALIAS(fshort, float);
DLIS_REGISTER_TYPEALIAS(isingl, float);
DLIS_REGISTER_TYPEALIAS(vsingl, float);
DLIS_REGISTER_TYPEALIAS(uvari,  std::int32_t);
DLIS_REGISTER_TYPEALIAS(origin, std::int32_t);
DLIS_REGISTER_TYPEALIAS(ident,  std::string);
DLIS_REGISTER_TYPEALIAS(ascii,  std::string);
DLIS_REGISTER_TYPEALIAS(units,  std::string);

#undef DLIS_REGISTER_TYPEALIAS

template< typename T, int > struct validated;
template< typename T > struct validated< T, 2 > { T V, A; };
template< typename T > struct validated< T, 3 > { T V, A, B; };

using fsing1 = validated< float, 2 >;
using fsing2 = validated< float, 3 >;
using fdoub1 = validated< double, 2 >;
using fdoub2 = validated< double, 3 >;

struct datetime {
    int Y, TZ, M, D, H, MN, S, MS;
};

struct object_name {
    dl::types::origin origin;
    std::uint8_t copy;
    dl::types::ident id;

    bool operator==( const object_name& rhs ) const {
        return origin == rhs.origin
            && copy == rhs.copy
            && id == rhs.id;
    }

    std::tuple< long, long, std::string > as_tuple() const {
        return std::make_tuple(
            static_cast< const dl::types::origin::value_type& >( this->origin ),
            this->copy,
            static_cast< const dl::types::ident::value_type& >( this->id )
        );
    }
};

}

using types::datetime;
using types::object_name;

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

datetime dtime( const char*& xs ) noexcept;

long origin( const char*& xs ) noexcept;

object_name obname( const char*& xs );
object_name obname( const char*& xs, int nmemb );
std::tuple< std::string, long, int, std::string > objref( const char*& xs );

int status( const char*& xs ) noexcept;

}

#endif //DLISIO_EXT_TYPES_HPP
