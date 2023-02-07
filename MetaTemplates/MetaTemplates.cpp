// Template computing (like in the old days) compile-time version
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
	int x; std::tie(std::ignore, std::ignore, x) = rtii(a, b);
	return (a + x) % a;
}

// A modern compile-time only version
template <auto a, auto b>
struct im
{
private:
	template <auto _a, auto _b>
	struct _im
	{
		using _im_ = _im<_b% _a, _a>;
		static constexpr auto g = _im_::g,
							  y = _im_::x - (_b / _a) * _im_::y,
							  x = _im_::y;
	};
	template <auto _b>
	struct _im<0, _b>
	{
		static constexpr auto g = _b, y = 0, x = 1;
	};
public:
	static constexpr auto x = (a + _im<a, b>::x) % a;
	static_assert(_im<a, b>::g == 1);
};

// Contemporary, compile- and run-time implementation
consteval stpl3i imi(auto a, auto b)
{
	if (a)
	{
		auto _i_ = imi(b % a, a);
		return tuple(get<0>(_i_),
					 get<2>(_i_) - (b / a) * get<1>(_i_),
					 get<1>(_i_));
	}
	return tuple(b, 0, 1);
}
consteval auto mim(auto a, auto b)
{
	return (a + get<2>(imi(a, b))) % a;
}

int main()
{
	const auto p{197}, q(85);
	static_assert(gcd<p, q>::value == 1, "gcd(p, q) != 1...");
	
	constexpr auto x = im<p, q>::x, y = mi<p, q>::μ, z = mim(p, q);
	static_assert(x * q % p == 1 && y == x && z == y, "Something's fishy...");

	auto [_, __, v] {imi(p, q)};
	assert(v == rtmi(p, q));
	
	return v - x + y - z;
}