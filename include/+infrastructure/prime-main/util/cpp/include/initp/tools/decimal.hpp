#ifndef INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
#define INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED

#include <initp/tools/cnumber>
#include <initp/tools/cdecimal>

#include <cstdint>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <type_traits>

namespace initp {
namespace tools {

namespace detail {

template<typename T>
class decimal_types {
protected:
    typedef double(*to_double_fp)(T, uint8_t);
    typedef T(*from_double_fp)(double, uint8_t, uint8_t);
    typedef T(*addition_fp)(T, uint8_t, T, uint8_t, uint8_t, uint8_t);
    typedef T(*subtraction_fp)(T, uint8_t, T, uint8_t, uint8_t, uint8_t);
    typedef T(*multiplication_fp)(T, uint8_t, T, uint8_t, uint8_t, uint8_t);
    typedef T(*division_fp)(T, uint8_t, T, uint8_t, uint8_t, uint8_t);
    typedef T(*round_fp)(T, uint8_t, uint8_t, uint8_t);
    typedef T(*integral_part_fp)(T, uint8_t);
    typedef T(*fractional_part_fp)(T, uint8_t);
};

template<typename T, typename Enable = void>
class decimal;

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, uint16_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_uint16_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_uint16; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_uint16_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_uint16_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_uint16_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_uint16_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_uint16; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_uint16; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_uint16; }
};

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, int16_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_int16_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_int16; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_int16_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_int16_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_int16_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_int16_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_int16; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_int16; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_int16; }
};

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, uint32_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_uint32_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_uint32; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_uint32_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_uint32_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_uint32_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_uint32_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_uint32; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_uint32; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_uint32; }
};

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, int32_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_int32_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_int32; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_int32_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_int32_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_int32_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_int32_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_int32; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_int32; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_int32; }
};

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, uint64_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_uint64_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_uint64; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_uint64_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_uint64_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_uint64_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_uint64_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_uint64; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_uint64; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_uint64; }
};

template<typename T>
class decimal<T, typename std::enable_if<std::is_same<T, int64_t>::value>::type>:
    public decimal_types<T> {
protected:
    typename decimal_types<T>::to_double_fp to_double(void) const { return &tools_int64_to_double; }
    typename decimal_types<T>::from_double_fp from_double(void) const { return &tools_double_to_int64; }
    typename decimal_types<T>::addition_fp addition(void) const { return &tools_int64_addition; }
    typename decimal_types<T>::subtraction_fp subtraction(void) const { return &tools_int64_subtraction; }
    typename decimal_types<T>::multiplication_fp multiplication(void) const { return &tools_int64_multiplication; }
    typename decimal_types<T>::division_fp division(void) const { return &tools_int64_division; }
    typename decimal_types<T>::round_fp round(void) const { return &tools_round_int64; }
    typename decimal_types<T>::integral_part_fp integral_part(void) const { return &tools_ipart_of_int64; }
    typename decimal_types<T>::fractional_part_fp fractional_part(void) const { return &tools_fract_of_int64; }
};

}

template<typename T, bool Fixed = true, uint8_t Round = 2>
class decimal: public detail::decimal<T> {

    using detail::decimal<T>::from_double;
    using detail::decimal<T>::addition;
    using detail::decimal<T>::subtraction;
    using detail::decimal<T>::multiplication;
    using detail::decimal<T>::division;
    using detail::decimal<T>::round;
    using detail::decimal<T>::integral_part;
    using detail::decimal<T>::fractional_part;

private: // Private types
    typedef decimal<T, Fixed, Round> self_type;

public: // Construction
    decimal(void): mantissa_(0), exponent_(0), is_set_(false) {}
    decimal(T mantissa, uint8_t exponent):
        mantissa_(mantissa), exponent_(exponent), is_set_(true) {}
    decimal(const self_type& v):
        mantissa_(v.mantissa_), exponent_(v.exponent_), is_set_(v.is_set_) {}
    decimal(const self_type&& v):
        mantissa_(v.mantissa_), exponent_(v.exponent_), is_set_(v.is_set_) {}
    virtual ~decimal(void) = default;

public: // Static methods
    static self_type from_double(double value, uint8_t exponent, uint8_t round = Round) {
        self_type result(0, exponent);
        result.mantissa_ = result.from_double()(value, exponent, round);
        return result;
    }
    static uint8_t get_exponent(const std::string& value, uint8_t max) {
        std::string::const_iterator it = std::find(value.cbegin(), value.cend(), '.');
        if (it != value.cend()) std::advance(it, 1);
        size_t count = std::distance(it, value.cend());
        return (count > max) ? max : (uint8_t)count;
    }
    static self_type from_string(const std::string& string, uint8_t exponent, uint8_t round = Round) {
        double value;
        std::stringstream stream;
        stream << string;
        stream >> value;
        if (!exponent) {
            return from_double(value, get_exponent(string, 5), round);
        } else return from_double(value, exponent, round);
    }

public: // Friend operators
    friend self_type operator-(const self_type& v) {
        return self_type(-v.mantissa(), v.exponent());
    }
    friend self_type operator+(const self_type& a, const self_type& b) {
        uint8_t exponent = Fixed ? a.exponent_ : (a.exponent_ >= b.exponent_) ? a.exponent_ : b.exponent_;
        T mantissa = a.addition()(a.mantissa_, a.exponent_, b.mantissa_, b.exponent_, exponent, Round);
        return self_type(mantissa, exponent);
    }
    friend self_type operator-(const self_type& a, const self_type& b) {
        uint8_t exponent = Fixed ? a.exponent_ : (a.exponent_ >= b.exponent_) ? a.exponent_ : b.exponent_;
        T mantissa = a.subtraction()(a.mantissa_, a.exponent_, b.mantissa_, b.exponent_, exponent, Round);
        return self_type(mantissa, exponent);
    }
    friend self_type operator*(const self_type& a, const self_type& b) {
        uint8_t exponent = Fixed ? a.exponent_ : (a.exponent_ >= b.exponent_) ? a.exponent_ : b.exponent_;
        T mantissa = a.multiplication()(a.mantissa_, a.exponent_, b.mantissa_, b.exponent_, exponent, Round);
        return self_type(mantissa, exponent);
    }
    friend self_type operator/(const self_type& a, const self_type& b) {
        uint8_t exponent = Fixed ? a.exponent_ : (a.exponent_ >= b.exponent_) ? a.exponent_ : b.exponent_;
        T mantissa = a.division()(a.mantissa_, a.exponent_, b.mantissa_, b.exponent_, exponent, Round);
        return self_type(mantissa, exponent);
    }
    template<typename B>
    friend typename std::enable_if<std::is_integral<B>::value, self_type>::type operator+(const self_type& a, const B& b) {
        T mantissa = a.addition()(a.mantissa_, a.exponent_, b, a.exponent_, a.exponent_, Round);
        return self_type(mantissa, a.exponent_);
    }
    template<typename B>
    friend typename std::enable_if<std::is_integral<B>::value, self_type>::type operator-(const self_type& a, const B& b) {
        T mantissa = a.subtraction()(a.mantissa_, a.exponent_, b, a.exponent_, a.exponent_, Round);
        return self_type(mantissa, a.exponent_);
    }
    template<typename B>
    friend typename std::enable_if<std::is_integral<B>::value, self_type>::type operator*(const self_type& a, const B& b) {
        T mantissa = a.multiplication()(a.mantissa_, a.exponent_, b, a.exponent_, a.exponent_, Round);
        return self_type(mantissa, a.exponent_);
    }
    template<typename B>
    friend typename std::enable_if<std::is_integral<B>::value, self_type>::type operator/(const self_type& a, const B& b) {
        T mantissa = a.division()(a.mantissa_, a.exponent_, b, a.exponent_, a.exponent_, Round);
        return self_type(mantissa, a.exponent_);
    }
    friend bool operator==(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ == mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa == b.mantissa_);
        } else return (a.mantissa_ == b.mantissa_);
    }
    friend bool operator!=(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ != mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa != b.mantissa_);
        } else return (a.mantissa_ != b.mantissa_);
    }
    friend bool operator<(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ < mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa < b.mantissa_);
        } else return (a.mantissa_ < b.mantissa_);
    }
    friend bool operator>(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ > mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa > b.mantissa_);
        } else return (a.mantissa_ > b.mantissa_);
    }
    friend bool operator<=(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ <= mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa <= b.mantissa_);
        } else return (a.mantissa_ <= b.mantissa_);
    }
    friend bool operator>=(const self_type& a, const self_type& b) {
        if (a.exponent_ > b.exponent_) {
            T mantissa = a.round()(b.mantissa_, b.exponent_, a.exponent_, 0);
            return (a.mantissa_ >= mantissa);
        } else if (a.exponent_ < b.exponent_) {
            T mantissa = a.round()(a.mantissa_, a.exponent_, b.exponent_, 0);
            return (mantissa >= b.mantissa_);
        } else return (a.mantissa_ >= b.mantissa_);
    }
    friend std::ostream& operator<<(std::ostream& stream, const self_type& value) {
        stream << value.to_string();
        return stream;
    }

public: // Member operators
    template<typename V, bool F, uint8_t R>
    explicit operator decimal<V, F, R>() const {
        return decimal<V, F, R>((V)mantissa_, exponent_);
    }
    explicit operator double() const {
        return this->to_double();
    }
    operator bool() const {
        return this->is_set_;
    }
    bool operator!() const {
        return !this->is_set_;
    }
    void operator+=(const self_type& v) {
        uint8_t exponent = Fixed ? exponent_ : (exponent_ >= v.exponent_) ? exponent_ : v.exponent_;
        mantissa_ = addition()(mantissa_, exponent_, v.mantissa_, v.exponent_, exponent, Round);
        exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void operator-=(const self_type& v) {
        uint8_t exponent = Fixed ? exponent_ : (exponent_ >= v.exponent_) ? exponent_ : v.exponent_;
        mantissa_ = subtraction()(mantissa_, exponent_, v.mantissa_, v.exponent_, exponent, Round);
        exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void operator*=(const self_type& v) {
        uint8_t exponent = Fixed ? exponent_ : (exponent_ >= v.exponent_) ? exponent_ : v.exponent_;
        mantissa_ = multiplication()(mantissa_, exponent_, v.mantissa_, v.exponent_, exponent, Round);
        exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void operator/=(const self_type& v) {
        uint8_t exponent = Fixed ? exponent_ : (exponent_ >= v.exponent_) ? exponent_ : v.exponent_;
        mantissa_ = division()(mantissa_, exponent_, v.mantissa_, v.exponent_, exponent, Round);
        exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void operator=(const self_type& v) {
        if (Fixed) {
            if (exponent_ != v.exponent_) {
                mantissa_ = round()(v.mantissa_, v.exponent_, exponent_, Round);
            } else mantissa_ = v.mantissa_;
        } else {
            mantissa_ = v.mantissa_;
            exponent_ = v.exponent_;
        }
        if (!is_set_) is_set_ = true;
    }
    void operator=(float value) {
        this->mantissa_ = this->from_double()((double)value, this->exponent_, Round);
        if (!is_set_) is_set_ = true;
    }
    void operator=(double value) {
        this->mantissa_ = this->from_double()(value, this->exponent_, Round);
        if (!is_set_) is_set_ = true;
    }

public: // Public methods
    template<typename V, bool F, uint8_t R>
    void set(const decimal<V, F, R>& value) {
        this->mantissa_ = (T)value.mantissa();
        this->exponent_ = value.exponent();
        if (!is_set_) is_set_ = true;
    }
    template<typename V>
    void set(V mantissa, uint8_t exponent) {
        this->mantissa_ = (T)mantissa;
        this->exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void set(double value, uint8_t exponent, uint8_t round = Round) {
        this->mantissa_ = this->from_double()(value, exponent, round);
        this->exponent_ = exponent;
        if (!is_set_) is_set_ = true;
    }
    void parse_string(const std::string& string, uint8_t exponent, uint8_t round = Round) {
        double value;
        std::stringstream stream;
        stream << string;
        stream >> value;
        if (!exponent) exponent = get_exponent(string, 5);
        this->set(value, exponent, round);
    }
    void round(uint8_t exponent, uint8_t type = Round) {
        if (exponent_ != exponent) {
            mantissa_ = round()(mantissa_, exponent_, exponent, type);
            exponent_ = exponent;
        }
        if (!is_set_) is_set_ = true;
    }
    void clear(void) {
        this->mantissa_ = (T)0;
        this->exponent_ = 0U;
        this->is_set_ = false;
    }
    double to_double(void) const {
        return detail::decimal<T>::to_double()(mantissa_, exponent_);
    }
    bool to_bool(void) const {
        return (mantissa_ > 0) ? true : false;
    }
    template<typename V, bool F, uint8_t R>
    bool diff(const decimal<V, F, R>& value, double max) {
        self_type a(*this);
        decimal<V, F, R> b(value);
        if (this->exponent_ >= value.exponent_) {
            b.round(this->exponent_);
        } else {
            a.round(value.exponent_);
        }
        self_type c = self_type::from_double(max, this->exponent_);
        if (a >= b) {
            return a - b >= c;
        } else {
            return b - a >= c;
        }
    }
    std::string to_string(void) const {
        std::stringstream stream;
        if (this->is_set_) {
            T integral = integral_part()(mantissa_, exponent_);
            if (!integral && mantissa_ < 0)
                stream << "-";
            stream << integral;
            if (exponent_ > 0) {
                stream << "." << std::setw(exponent_) << std::setfill('0');
                stream << fractional_part()(mantissa_, exponent_);
            }
        } else {
            stream << "null";
        }
        return stream.str();
    }

public: // Public properties
    T mantissa(void) const {
        return this->mantissa_;
    }
    uint8_t exponent(void) const {
        return this->exponent_;
    }
    bool is_set(void) const {
        return this->is_set_;
    }

private: // Private fields
    T mantissa_;
    uint8_t exponent_;
    bool is_set_;
};

}}

typedef initp::tools::decimal<int16_t> dec16_t;
typedef initp::tools::decimal<int32_t> dec32_t;
typedef initp::tools::decimal<int64_t> dec64_t;

typedef initp::tools::decimal<uint16_t> udec16_t;
typedef initp::tools::decimal<uint32_t> udec32_t;
typedef initp::tools::decimal<uint64_t> udec64_t;

#endif // INITPLUS_TOOLS_DECIMAL_HPP_INCLUDED
