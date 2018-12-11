#ifndef DLISIO_EXT_TYPES_HPP
#define DLISIO_EXT_TYPES_HPP

#include <cstdint>
#include <exception>
#include <string>

#include <boost/variant.hpp>

#include "typeconv.hpp"

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

struct float1 { float V, A; };
struct double1 { float V, A; };

struct float2 { float V, A, B; };
struct double2 { float V, A, B; };

using value_vector = boost::variant<
    std::vector< std::int8_t >,
    std::vector< std::int16_t >,
    std::vector< std::int32_t >,
    std::vector< std::uint8_t >,
    std::vector< std::uint16_t >,
    std::vector< std::uint32_t >,
    std::vector< float >,
    std::vector< double >,
    std::vector< float1 >,
    std::vector< double1 >,
    std::vector< float2 >,
    std::vector< double2 >,
    std::vector< std::complex< float > >,
    std::vector< std::complex< double > >,
    std::vector< std::string >
>;

struct object_attribute {
    std::string         label = "";
    std::int32_t        count = 1;
    representation_code reprc = representation_code::ident;
    std::string         units = "";
    value_vector        value = {};
};

inline representation_code to_reprc( std::uint8_t x ) {
    if (x < DLIS_FSHORT || x > DLIS_UNITS ) {
        const auto msg = "invalid representation code "
                       + std::to_string( x )
                       + ", expected 1 <= reprc <= 27"
                       ;
        throw std::invalid_argument(msg);
    }

    return static_cast< representation_code >( x );
}

class channel {
    std::string name;
    std::uint8_t reprc;
    std::string units;
    std::vector< std::string > properties;
    std::vector< int > dimension;
    std::vector< int > element_limit;
    std::vector< object_name > axis;
};

}

#endif //DLISIO_EXT_TYPES_HPP
