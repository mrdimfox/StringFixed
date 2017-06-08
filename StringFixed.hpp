// MIT License

// Copyright (c) 2017 Dmitriy Lisin

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef STRINGFIXED_HPP_
#define STRINGFIXED_HPP_

#include <cstring>
#include <cstddef>
#include <iterator>
#include <algorithm>
#include <string>
#include <type_traits>

#if __cplusplus > 201103L
#include <experimental/string_view>
#endif


template<std::size_t N>
class StringFixed
{
public:
    constexpr static std::size_t CAPACITY = N;

    using CharType = char;

    typedef CharType                                  value_type;
    typedef CharType&                                 reference;
    typedef const CharType&                           const_reference;
    typedef CharType*                                 pointer;
    typedef const CharType*                           const_pointer;
    typedef CharType*                                 iterator;
    typedef const CharType*                           const_iterator;
    typedef std::reverse_iterator<iterator>           reverse_iterator;
    typedef std::reverse_iterator<const_iterator>     const_reverse_iterator;
    typedef size_t                                    size_type;

    enum Status {
        OK,
        BAD_ALLOC,
        EMPTY
    };

    StringFixed() :
        _length(1)
    {
        _buffer[0] = '\0';
    }

    template<typename T>
    StringFixed(T c) :
        _length(2)
    {
        static_assert(std::is_fundamental<T>::value, "Primitive type required");
        set(c);
    }

    StringFixed(const char* s, std::size_t length) {
        set(s, length);
    }

    template<std::size_t str_length>
    StringFixed(const char (&s)[str_length]) :
        _length(str_length)
    {
        set(s);
    }

    template<std::size_t M>
    StringFixed(StringFixed<M> const& src) {
        set(src);
    }

#if __cplusplus > 201103L
    StringFixed(std::experimental::string_view const& src) {
        set(src.data(), src.length());
    }
#endif

    StringFixed(StringFixed const& src) {
        set(src.data(), src.length());
    }

    StringFixed(StringFixed&&) = default;

    template<std::size_t str_length>
    StringFixed& operator=(const char (&s)[str_length]) {
        set(s);
        return *this;
    }

    template<std::size_t M>
    StringFixed& operator=(StringFixed<M> const& src) {
        set(src.data(), src.length());
        return *this;
    }

#if __cplusplus > 201103L
    StringFixed& operator=(std::experimental::string_view const& src) {
        set(src.data(), src.length());
        return *this;
    }

    operator std::experimental::string_view() {
        return std::experimental::string_view(data(), length());
    }
#endif

    StringFixed& operator=(StringFixed const& src) {
        set(src.data(), src.length());
        return *this;
    }

    StringFixed& operator=(StringFixed&&) = default;
    ~StringFixed() = default;

    template<typename T>
    Status set(T c) {
        static_assert(std::is_fundamental<T>::value, "Primitive type required");
        return set(_to_pointer(&c), 1);
    }

    Status set_cstring(const char* s) {
        return set(s, std::strlen(s));
    }

    template<std::size_t M>
    Status set(StringFixed<M> const& src) {
        return set(src.data(), src.length());
    }

#if __cplusplus > 201103L
    Status set(std::experimental::string_view const& src) {
        return set(src.data(), src.length());
    }
#endif

    Status set(const char* s, std::size_t length)
    {
        auto status = OK;
        if (length > CAPACITY) {
            status = BAD_ALLOC;
            length = CAPACITY;
        }

        _length = length + 1;
        std::memcpy(&_buffer, s, length);
        _buffer[_useful_length()] = '\0';

        return status;
    }

    template<std::size_t str_length>
    Status set(const char (&s)[str_length])
    {
        _length = str_length;

        auto status = OK;
        if (_length - 1 > CAPACITY) {
            status = BAD_ALLOC;
            _length = CAPACITY;
        }

        std::memcpy(_to_pointer(&_buffer), _to_cpointer(s), _length);
        _buffer[_useful_length()] = '\0';

        return status;
    }

    template<std::size_t str_length>
    StringFixed& operator+=(const char (&s)[str_length]) {
        add(s);
        return *this;
    }

    template<std::size_t M>
    StringFixed& operator+=(StringFixed<M> const& src) {
        add(src.data(), src.length());
        return *this;
    }

#if __cplusplus > 201103L
    StringFixed& operator+=(std::experimental::string_view const& src) {
        add(src.data(), src.length());
        return *this;
    }
#endif

    Status add(const char c)
    {
        return add(&c, 1);
    }

    Status add_cstring(const char* s)
    {
        return add(s, std::strlen(s));
    }

    template<std::size_t str_length>
    Status add(const char (&s)[str_length])
    {
        return add(_to_cpointer(s), str_length - 1);
    }

    template<std::size_t M>
    Status add(StringFixed<M> const& src)
    {
        return add(src.begin(), src.length());
    }

#if __cplusplus > 201103L
    Status add(std::experimental::string_view const& src) {
        return add(src.data(), src.length());
    }
#endif

    Status add(const char* s, std::size_t length)
    {
        if (_length + length - 1 > CAPACITY) {
            return BAD_ALLOC;
        }

        std::memcpy(end(), s, length);
        _length += length;
        _buffer[_useful_length()] = '\0';

        return OK;
    }

    Status pop_back() {
        if (_useful_length() > 0) {
            _length--;
            _buffer[_useful_length()] = '\0';

            return OK;
        }
        else {
            return EMPTY;
        }
    }

    const_reference back() {
        return *rbegin();
    }

    const_reference operator[](std::size_t i) {
        return _buffer[i];
    }

    const_pointer data() const {
        return _to_pointer(&_buffer);
    }

    pointer data() {
        return _to_pointer(&_buffer);
    }

    std::size_t length() const {
        return _length - 1;
    }

    iterator begin() {
        return _to_pointer(&_buffer);
    }

    const_iterator begin() const {
        return _to_pointer(&_buffer);
    }

    iterator end() {
        return &_buffer[_useful_length()];
    }

    const_iterator end() const {
        return &_buffer[_useful_length()];
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    bool full() const {
        return _useful_length() == CAPACITY;
    }

    bool empty() const {
        return _useful_length() == 0;
    }

    template<std::size_t M>
    int compare(const StringFixed<M>& other) const {
        return _compare(this->begin(), this->end(), other.begin(), other.end());
    }

    int compare_cstring(const char* other) const {
        return _compare(this->begin(), this->end(), other, other + std::strlen(other));
    }

    template<std::size_t str_length>
    int compare(const char (&other)[str_length]) const {
        return _compare(this->begin(), this->end(), other, other + str_length - 1);
    }

    int compare(const char* other, std::size_t length) const {
        return _compare(this->begin(), this->end(), other, other + length);
    }

    void clear() {
        _length = 1;
        _buffer[0] = '0';
    }

private:
    CharType _buffer[CAPACITY + 1];
    std::size_t _length;

    inline std::size_t _useful_length() const {
        return _length - 1;
    }

    template<typename T>
    inline pointer _to_pointer(T ptr) {
        return reinterpret_cast<pointer>(ptr);
    }

    template<typename T>
    inline const_pointer _to_pointer(T ptr) const {
        return reinterpret_cast<const_pointer>(ptr);
    }

    template<typename T>
    inline const_pointer _to_cpointer(T ptr) const {
        return reinterpret_cast<const_pointer>(ptr);
    }

    int _compare(const_pointer begin_iter1, const_pointer end_inter1,
                 const_pointer begin_iter2, const_pointer end_inter2) const
    {
        auto it1 = begin_iter1;
        auto it2 = begin_iter2;

        while ((it1 != end_inter1) && (it2 != end_inter2)) {
            ++it1;
            ++it2;
        }

        if ((it1 == end_inter1) && (it2 == end_inter2)) {
            return 0;
        }
        else if (it2 == end_inter2) {
            return -1;
        }
        else {
            return 1;
        }
    }
};

template<std::size_t N>
bool operator ==(const StringFixed<N>& lhs, const StringFixed<N>& rhs)
{
    return (lhs.length() == rhs.length()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<std::size_t N, std::size_t M>
bool operator ==(const StringFixed<N>& lhs, const StringFixed<M>& rhs)
{
    return (lhs.length() == rhs.length()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<std::size_t N, std::size_t M>
bool operator ==(const StringFixed<N>& lhs, const char (&rhs)[M])
{
    return (lhs.length() == std::strlen(rhs)) && std::equal(lhs.begin(), lhs.end(), rhs);
}

template<std::size_t N, std::size_t M>
bool operator ==(const char (&lhs)[M], const StringFixed<N>& rhs)
{
    return (rhs.length() == std::strlen(lhs)) && std::equal(rhs.begin(), rhs.end(), lhs);
}

template<std::size_t N>
bool operator !=(const StringFixed<N>& lhs, const StringFixed<N>& rhs)
{
    return !(lhs == rhs);
}

template<std::size_t N, std::size_t M>
bool operator !=(const StringFixed<N>& lhs, const StringFixed<M>& rhs)
{
    return !(lhs == rhs);
}

template<std::size_t N, std::size_t M>
bool operator !=(const StringFixed<N>& lhs, const char (&rhs)[M])
{
    return !(lhs == rhs);
}

template<std::size_t N, std::size_t M>
bool operator !=(const char (&lhs)[M], const StringFixed<N>& rhs)
{
    return !(lhs == rhs);
}

#if __cplusplus > 201103L
template<std::size_t N>
bool operator ==(const StringFixed<N>& lhs, const std::experimental::string_view& rhs)
{
    return (lhs.length() == rhs.length()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<std::size_t N>
bool operator ==(const std::experimental::string_view& lhs, const StringFixed<N>& rhs)
{
    return (lhs.length() == rhs.length()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<std::size_t N>
bool operator !=(const StringFixed<N>& lhs, const std::experimental::string_view& rhs)
{
    return !(lhs == rhs);
}

template<std::size_t N>
bool operator !=(const std::experimental::string_view& lhs, const StringFixed<N>& rhs)
{
    return !(lhs == rhs);
}
#endif


#endif /* STRINGFIXED_HPP_ */
