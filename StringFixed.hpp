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

    StringFixed(StringFixed const& src) {
        set(src.data(), src.length());
    }

    StringFixed(StringFixed&&) = default;

    template<std::size_t M>
    StringFixed& operator=(StringFixed<M> const& src) {
        set(src.data(), src.length());
        return *this;
    }

    StringFixed& operator=(StringFixed const& src) {
        set(src.data(), src.length());
        return *this;
    }

    StringFixed& operator=(StringFixed&&) = default;
    ~StringFixed() = default;

    template<typename T>
    void set(T c) {
        static_assert(std::is_fundamental<T>::value, "Primitive type required");
        set(_to_pointer(&c), 1);
    }

    void set(const char* s) {
        set(s, std::strlen(s));
    }

    void set(const char* s, const std::size_t length)
    {
        _length = length + 1;
        std::memcpy(&_buffer, s, _length);

        _buffer[_useful_length()] = '\0';
    }

    template<std::size_t str_length>
    void set(const char (&s)[str_length])
    {
        _length = str_length;
        std::strcpy(_to_pointer(&_buffer), _to_pointer(s));
        _buffer[_useful_length()] = '\0';
    }


	const char& operator[](std::size_t i) {
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

    int compare(const StringFixed& other) const {
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


private:
    CharType _buffer[CAPACITY];
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
