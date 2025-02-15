﻿// Template computing (like in the old days) compile-time version
// https://en.cppreference.com/w/cpp/language/operators#Array_subscript_operator
// "or C++17's std::gcd"
template <int k, int l>
struct gcd
{
	enum divisor { value = gcd<l, k% l>::value };
};
template <int k>
struct gcd<k, 0>
{
	enum divisor { value = k };
};

/* Evaluates mult_inv<p, q>::μ = q⁻¹ mod p,
   that is, μ⋅q mod p = 1 (a brute force/naïve approach) */
template <auto w, auto M, auto k = w, auto = 0>
struct mi
{
	static constexpr auto res = (k - 1) * M % w,
		μ = mi<w, M, k - 1, res>::μ;
};
template <auto w, auto M, auto k>
struct mi<w, M, k, 1>
{
	static constexpr auto μ = k;
};
template <auto M, auto k, auto res>
struct mi<1, M, k, res>
{
	static constexpr auto μ = 1;
};

/* A bit smarter version: https://stackoverflow.com/a/9758173/17524824
def egcd(a, b):
	if a == 0:
		return (b, 0, 1)
	else:
		g, y, x = egcd(b % a, a)
		return (g, x - (b // a) * y, y)

... and a bunch of its more/less generic carbon copies (a.k.a. gcc's) */
#include <tuple>
#include <cassert>

using std::tuple;
template <typename...T> using stpl = std::tuple<T...>;
using stpl3i = stpl<int, int, int>;

// A run-time only instantiation
stpl3i rtii(auto a, auto b)
{
	if (a)
	{
		auto [g, y, x] = rtii(b % a, a);
		return tuple(g, x - (b / a) * y, y);
	}
	return tuple(b, 0, 1);
}
auto rtmi(auto a, auto b)
{
	decltype(a) x; std::tie(std::ignore, std::ignore, x) = rtii(a, b);
	return (a + x) % a;
}

// A modern compile-time only version
template <auto a, auto b>
struct im
{
private:
	template <auto _a, auto _b>
	struct _
	{
		using __ = _<_b % _a, _a>;
		static constexpr auto g = __::g,
			y = __::x - (_b / _a) * __::y,
			x = __::y;
	};
	template <auto _b>
	struct _<0, _b>
	{
		static constexpr auto g = _b, y = 0, x = 1;
	};
public:
	static constexpr auto x = (a + _<a, b>::x) % a;
	static_assert(_<a, b>::g == 1);
};

//Contemporary, compile- and run-time implementation
static consteval stpl3i imi(auto a, auto b)
{
	if (a)
	{
		auto _ = imi(b % a, a);
		return tuple(get<0>(imi(b % a, a)),
					 get<2>(imi(b % a, a)) - (b / a) * get<1>(imi(b % a, a)),
				     get<1>(imi(b % a, a)));
	}
	else return tuple(b, 0, 1);
}
consteval auto mim(auto a, auto b)
{
	return (a + get<2>(imi(a, b))) % a;
}

int main()
{
	const auto p{ 197 }, q(85);
	static_assert(gcd<p, q>::value == 1, "gcd(p, q) != 1...");

	constexpr auto x = im<p, q>::x,
	               y = mi<p, q>::μ;
	assert(x == rtmi(p, q));	

	constexpr auto a = not(x * q % p != 1 or y != x), // A smart compiler has its say here...
				   b = x * q % p == 1 and y == x;     // A bit of Boolean algebra. Namely, the De Morgan's laws. 
	static_assert(a && b, "Something's fishy...");
	
	constexpr auto v = get<2>(imi(p, q)),
		           z = mim(p, q);
	static_assert(v && z, "Something's suspicious...");

	return y - x + z - v;
}
