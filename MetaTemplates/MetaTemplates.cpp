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
template <auto a, auto b>
struct im
{
	static constexpr auto g = im<b%a, a>::g,
						  y = im<b%a, a>::x - (b/a) * im<b%a, a>::y,
						  x = im<b%a, a>::y;
};
template <auto b>
struct im<0, b>
{
	static constexpr auto g = b, y = 0, x = 1;
};

#include <tuple>
using std::tuple;
consteval std::tuple<int, int, int> ii(auto const a, auto const b)
{
	return a ? tuple(get<0>(ii(b%a, a)),
			         get<2>(ii(b%a, a)) - (b/a) * get<1>(ii(b%a, a)), 
				     get<1>(ii(b%a, a)))
		     : tuple(b, 0, 1);
}
consteval int mm(int const a, int const b)
{
	return get<2>(ii(a, b));
}

int main()
{
	constexpr auto p = 97, q = 86;

	int v; std::tie(std::ignore, std::ignore, v) = ii(p, q); 
	//                     const auto [_, __, v] = ii(p, q);
	constexpr auto x  {im<p, q>::x},
				   y  (mi<p, q>::μ),
				   z = mm(p, q);
	return v - x + y - z;                  	
}