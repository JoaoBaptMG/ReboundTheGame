#pragma once

namespace util
{
    template <typename T>
    class ContainerEntry final
    {
        T* cont;
        typename T::iterator it;
    public:
        using iterator = typename T::iterator;

        ContainerEntry() : cont(nullptr), it() {}
        ContainerEntry(T& cont, typename T::iterator it) : cont(&cont), it(it) {}
        ~ContainerEntry() { if (cont) cont->erase(it); }

        ContainerEntry(const ContainerEntry &other) = delete;
        ContainerEntry(ContainerEntry&& other) noexcept : ContainerEntry() { swap(*this, other); }

        ContainerEntry &operator=(ContainerEntry other) { swap(*this, other); return *this; }

        friend void swap(ContainerEntry &c1, ContainerEntry &c2) noexcept
        {
            using std::swap;
            swap(c1.cont, c2.cont);
            swap(c1.it, c2.it);
        }
    };
}
