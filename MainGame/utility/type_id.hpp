#pragma once

namespace util
{
    ///
    /// \brief Class to encapsulate type_id
    /// Its implementation is by iavr (http://codereview.stackexchange.com/users/39083/iavr)
    /// and its code can be found on the following link:
    /// http://codereview.stackexchange.com/questions/48594/unique-type-id-no-rtti
    ///
    class type_id_t
    {
        using sig = type_id_t();

        sig* id;
        type_id_t(sig* id) : id{id} {}

    public:
        template<typename T>
        friend type_id_t type_id();

        bool operator==(type_id_t o) const { return id == o.id; }
        bool operator!=(type_id_t o) const { return id != o.id; }
    };

    ///
    /// \brief This function is guaranteed to return an ID unique to each type
    ///
    template<typename T>
    inline type_id_t type_id() { return &type_id<T>; }

    const type_id_t notype = type_id<void>();
}
