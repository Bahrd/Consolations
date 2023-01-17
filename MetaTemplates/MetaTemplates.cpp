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

template <long w, long M, long k = w, long = 0>
struct mult_inv
{
	enum { res = (k - 1) * M % w				};
	enum { μ = mult_inv<w, M, k - 1, res>::μ	};
};

template <long w, long M, long k>
struct mult_inv<w, M, k, 1>
{
	enum { μ = k };
};

template <long M, long k, long res>
struct mult_inv<1, M, k, res>
{
	enum { μ = 1 };
};

int main()
{	
	return mult_inv<97, 18>::μ;
	//     mult_inv< p,  q>::μ = q⁻¹ mod p, that is, μ⋅q mod p = 1
}