// Sym includes
#include <symengine/expression.h>
#include <symengine/symbol.h>

typedef SymEngine::RCP<const SymEngine::Symbol> Symbol;
typedef SymEngine::RCP<const SymEngine::Basic> Expression;

//
inline std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
    os << symbol->get_name();
    return os;
}

//
inline std::ostream& operator<<(std::ostream& os, const Expression& expression) {
    os << expression->__str__();
    return os;
}

template <typename T>
Expression toExpression(const T& value) {

    if constexpr (std::is_same_v<std::decay_t<T>, Expression>) {
        return value;
    }
    if constexpr (std::is_integral_v<T>) {
        // T ist Integer
        return SymEngine::integer(value);
    }
    else if constexpr (std::is_floating_point_v<T>) {
        
        // T ist float/double
        double intpart;
        if (std::modf(value, &intpart) == 0.0) {
            
            // Ganzzahl
            return SymEngine::integer(static_cast<int>(value));
        }
        return SymEngine::real_double(value); // Approximierte Darstellung
    }

    //
    return SymEngine::real_double(0);
}

// Add
inline Expression operator+(const Expression& a, const Expression& b) {
    return SymEngine::add(a, b);
}

template <typename T>
inline Expression operator+(const T& a, const Expression& b) {
    return SymEngine::add(toExpression(a), b);
}

template <typename T>
inline Expression operator+(const Expression& a, const T& b) {
    return SymEngine::add(a, toExpression(b));
}

// subtract
inline Expression operator-(const Expression& a) {
    return SymEngine::mul(a, SymEngine::integer(-1));
}

inline Expression operator-(const Expression& a, const Expression& b) {
    return SymEngine::sub(a, b);
}

template <typename T>
inline Expression operator-(const T& a, const Expression& b) {
    return SymEngine::sub(toExpression(a), b);
}

template <typename T>
inline Expression operator-(const Expression& a, const T& b) {
    return SymEngine::sub(a, toExpression(b));
}

inline Expression operator*(const Expression& a, const Expression& b) {
    return SymEngine::mul(a, b);
}

template <typename T>
inline Expression operator*(const T& a, const Expression& b) {
    return SymEngine::mul(toExpression(a), b);
}

template <typename T>
inline Expression operator*(const Expression& a, const T& b) {
    return SymEngine::mul(a, toExpression(b));
}

inline Expression operator/(const Expression& a, const Expression& b) {
    return SymEngine::div(a, b);
}

template <typename T>
inline Expression operator/(const T& a, const Expression& b) {
    return SymEngine::div(toExpression(a), b);
}

template <typename T>
inline Expression operator/(const Expression& a, const T& b) {
    return SymEngine::div(a, toExpression(b));
}

// Potenzen
// die potenzfunktion funktioniert noch nicht einwandfrei für expression ** exoression da der Kompiler aufgrund der Mehrdeutigkeit
// durch die template Funktionen unterhalb eine Mehrdeutigkeit erkennt
// das sollte eingentlich durch das enable if not expression gelöst werden, was das Problem allerdings nicht behoben hat
inline Expression pow(const Expression& a, const Expression& b) {
    return SymEngine::pow(a, b);
}

template<typename T>
using enable_if_expression = std::enable_if_t<std::is_same_v<std::decay_t<T>, Expression>>;

template<typename T>
using enable_if_not_expression = std::enable_if_t<!std::is_same_v<std::decay_t<T>, Expression>>;

template <typename T, typename = enable_if_not_expression<T>>
inline Expression pow(const T& a, const Expression& b) {
    return SymEngine::pow(toExpression(a), b);
}

template <typename T, typename = enable_if_not_expression<T>>
inline Expression pow(const Expression& a, const T& b) {
    return SymEngine::pow(a, toExpression(b));
}

#define SYMBOL(prefix) const Symbol prefix = SymEngine::symbol(#prefix)