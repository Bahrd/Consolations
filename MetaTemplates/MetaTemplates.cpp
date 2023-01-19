template <int k, int l>
struct gcd
{
	enum { value = gcd<l, k% l>::value };
};
template <int k>
struct gcd<k, 0>
{
	enum { value = k };
};

/* Evaluates mult_inv<p, q>::μ = q⁻¹ mod p,
   that is, μ⋅q mod p = 1 (a brute force/naïve approach) */
template <auto w, auto M, auto k = w, auto = 0>
struct mi
{
	static constexpr auto res = (k - 1) * M % w;
	static constexpr auto μ = mi<w, M, k - 1, res>::μ;
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
*/

// Ordinary (run-time) carbon-copy implementation
#include <tuple>
#include <cassert>
using std::tuple;
std::tuple<int, int, int> rtii(auto const a, auto const b)
{
	if (a)
	{
		auto [g, y, x] = rtii(b % a, a);
		return tuple(g, x - (b / a) * y, y);
	}
	else
	{
		return tuple(b, 0, 1);
	}
}
auto rtmi(auto const a, auto const b)
{
	int x; std::tie(std::ignore, std::ignore, x) = rtii(a, b);
	return (x + a) % a;
}

// Template computing (like in the old days) compile-time version
template <int a, int b>
struct _im
{
private:
	static constexpr int _g = _im<b% a, a>::g,
						 _x = _im<b% a, a>::x, 
		                 _y = _im<b% a, a>::y;
public:
	static constexpr int g = _g;
	static constexpr int y = _x - (b / a) * _y;
	static constexpr int x = _y;
};
template <int b>
struct _im<0, b>
{
	static constexpr int g = b, y = 0, x = 1;
};
template <int a, int b>
struct im
{
	static_assert(_im<a, b>::g == 1);
	static constexpr int x = (a + _im<a, b>::x) % a;
};

// Contemporary compile-time version
consteval std::tuple<int, int, int> imi(int const a, int const b)
{
	return a ? tuple(get<0>(imi(b % a, a)),
			         get<2>(imi(b%a, a)) - (b/a) * get<1>(imi(b%a, a)), 
				     get<1>(imi(b%a, a)))
		     : tuple(b, 0, 1);
}
consteval int mm(int const a, int const b)
{
	return (a + get<2>(imi(a, b))) % a;
}

int main()
{
	constexpr auto p{197}, q(85);
	static_assert(gcd<p, q>::value == 1, "gcd(p, q) != 1...");
	
	constexpr auto x = im<p, q>::x, y = mi<p, q>::μ, z = mm(p, q);
	static_assert(x * q % p == 1 && y == x && z == y, "Something's fishy...");

	auto [_, __, v] = imi(p, q);
	assert(v == rtmi(p, q));
	
	return v - x + y - z;
}