#include <type_traits>
#include <iterator>
#include <utility>

namespace util
{
    template <typename T>
    struct is_iterator
    {
        static char test(...);

        template <typename U,
            typename = typename std::iterator_traits<U>::difference_type,
            typename = typename std::iterator_traits<U>::pointer,
            typename = typename std::iterator_traits<U>::reference,
            typename = typename std::iterator_traits<U>::value_type,
            typename = typename std::iterator_traits<U>::iterator_category>
        static long test(U&&);

        constexpr static bool value = std::is_same<decltype(test(std::declval<T>())),long>::value;
    };
}

template <typename T>
auto begin(const std::pair<T,T>& p) -> typename std::enable_if<util::is_iterator<T>, T>::type
{ return p.first; }

template <typename T>
auto end(const std::pair<T,T>& p) -> typename std::enable_if<util::is_iterator<T>, T>::type
{ return p.second; }
