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

template <auto w, auto M, auto k = w, auto = 0>
struct mult_inv
{
	static constexpr auto res = (k - 1) * M % w;
	static constexpr auto μ = mult_inv<w, M, k - 1, res>::μ;
};

template <auto w, auto M, auto k>
struct mult_inv<w, M, k, 1>
{
	static constexpr auto μ = k;
};

template <auto M, auto k, auto res>
struct mult_inv<1, M, k, res>
{
	static constexpr auto μ = 1;
};


int main()
{	
	return mult_inv<97, 18>::μ;
	//     mult_inv< p,  q>::μ = q⁻¹ mod p, that is, μ⋅q mod p = 1
}