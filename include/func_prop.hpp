#pragma once

#include <functional>

namespace _impl{
	template<typename D, template <typename> typename Prop>
	D& call_dest(Prop<D>& a, const D& b, std::function<D(const D&, const D&)> fun) {
		*static_cast<D*>(&a) = fun(*static_cast<D*>(&a), b);
		return *static_cast<D*>(&a);
	}
}

//Adds overloads for addition to D when D publicly extends Addable<D>
template<typename D>
class Addable{
public:
	D& operator+=(const D& b) { return _impl::call_dest<D>(*this, b, D::Add); }
	friend D operator+(const D& a, const D& b) { return D::Add(a, b); }
};

template<typename D>
class Subtractable {
public:
	D& operator-=(const D& b) { return _impl::call_dest<D>(*this, b, D::Subtract); }
	friend D operator-(const D& a, const D& b) { return D::Subtract(a, b); }
};

template<typename D>
class Multipliable {
public:
	D& operator*=(const D& b) { return _impl::call_dest<D>(*this, b, D::Multiply); }
	friend D operator*(const D& a, const D& b) { return D::Multiply(a, b); }
};

template<typename D>
class Divisable {
public:
	D& operator/=(const D& b) { return _impl::call_dest<D>(*this, b, D::Divide); }
	friend D operator/(const D& a, const D& b) { return D::Divide(a, b); }
};

template<typename D, typename B>
class Comparable {
	friend B operator==(const D& a, const D& b) { return D::Compare(a, b); }

};

class Int : public Addable<Int> {
	int val;
public:
	Int(int v) :val(v) {};
	static Int Add(const Int& a, const Int& b) {
		return a + b;
	}
};

template<typename Base>
class Subtractable {
public:
	Base& operator-=(const Base& b) {
		return *this = Base::Subtract(a, b);
	}

	friend Base operator-(const Base& a, const Base& b) {
		return Base::Subtract(a, b);
	}
};

void derp() {
	Int a = 5;
	Int b = 6;
	Int c = a + b;
	b += a;
}
