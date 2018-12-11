#ifndef DLISIO_EXT_STRONG_TYPEDEF
#define DLISIO_EXT_STRONG_TYPEDEF

#include <type_traits>
#include <utility>

namespace dl {

using std::swap;

template< typename Tag, typename T >
class strong_typedef {
public:
    using value_type = T;

    constexpr static bool nothrow_copy_constructible =
        std::is_nothrow_copy_constructible< value_type >::value;

    constexpr static bool nothrow_move_constructible =
        std::is_nothrow_move_constructible< value_type >::value;

    constexpr static bool nothrow_swappable =
        noexcept (swap( std::declval< T& >(), std::declval< T& >() ));

    strong_typedef() = default;

    explicit strong_typedef( const T& x )
        noexcept(strong_typedef::nothrow_copy_constructible);

    explicit strong_typedef( T&& x )
        noexcept(strong_typedef::nothrow_move_constructible);

    explicit operator T&() noexcept (true);
    explicit operator const T&() const noexcept (true);

private:
    T value;
    /*
     * Inherit the noexcept property of the underlying swap operation. Usually
     * swap is noexcept (although for strings it's only conditionally after
     * C++17, and not really at all before
     */
    friend void swap( strong_typedef& a, strong_typedef& b )
        noexcept (nothrow_swappable)
    {
        swap( static_cast< T& >( a ), static_cast< T& >( b ) );
    }
};

}

#endif // DLISIO_EXT_STRONG_TYPEDEF
