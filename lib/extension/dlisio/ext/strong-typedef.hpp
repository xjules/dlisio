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

    constexpr static bool nothrow_eq =
        noexcept (std::declval< const T& >() == std::declval< const T& >());

    constexpr static bool nothrow_ne =
        noexcept (std::declval< const T& >() != std::declval< const T& >());

    constexpr static bool nothrow_lt =
        noexcept (std::declval< const T& >() < std::declval< const T& >());

    constexpr static bool nothrow_le =
        noexcept (std::declval< const T& >() <= std::declval< const T& >());

    constexpr static bool nothrow_gt =
        noexcept (std::declval< const T& >() < std::declval< const T& >());

    constexpr static bool nothrow_ge =
        noexcept (std::declval< const T& >() >= std::declval< const T& >());

    bool operator == ( const strong_typedef& rhs ) const noexcept (nothrow_eq)
    {
        return this->value == rhs.value;
    }

    bool operator != ( const strong_typedef& rhs ) const noexcept (nothrow_ne)
    {
        return this->value != rhs.value;
    }

    bool operator < ( const strong_typedef& rhs ) const noexcept (nothrow_lt)
    {
        return this->value < rhs.value;
    }

    bool operator <= ( const strong_typedef& rhs ) const noexcept (nothrow_le)
    {
        return this->value <= rhs.value;
    }

    bool operator > ( const strong_typedef& rhs ) const noexcept (nothrow_gt)
    {
        return this->value > rhs.value;
    }

    bool operator >= ( const strong_typedef& rhs ) const noexcept (nothrow_ge)
    {
        return this->value >= rhs.value;
    }

private:
    T value;
    /*
     * Inherit the noexcept property of the underlying swap operation. Usually
     * swap is noexcept (although for strings it's only conditionally after
     * C++17, and not really at all before)
     */
    friend void swap( strong_typedef& a, strong_typedef& b )
        noexcept (nothrow_swappable)
    {
        swap( static_cast< T& >( a ), static_cast< T& >( b ) );
    }
};

}

#endif // DLISIO_EXT_STRONG_TYPEDEF
