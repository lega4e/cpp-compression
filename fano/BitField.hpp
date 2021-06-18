#ifndef NVX_BIT_FIELD_HPP_3189321
#define NVX_BIT_FIELD_HPP_3189321

#include <cstdlib>
#include <cstring>
#include <vector>





namespace nvx
{
	
	
	
	
	
class BitField: public std::vector<uint8_t>
{
public:
	void zeroize()
	{
		memset(data(), 0, size());
		return;
	}

	int bitsize() const
	{
		return _bitsize;
	}

	void bitresize(int newbitsize)
	{
		_bitsize = newbitsize;
		resize( _bitsize / 8 + (_bitsize % 8 ? 1 : 0) );
		return;
	}

	void set(int n, bool bit)
	{
		if( n/8 + (n%8 ? 1 : 0) >= (int)size() )
			bitresize(n+1);

		if(bit)
			at(n/8) |= 1 << (n%8);
		else
			at(n/8) &= 255u ^ (1 << (n%8));

		return;
	}
	
	bool get(int n) const
	{
		return at(n/8) & (1 << n%8);
	}

	void pushbit(bool bit)
	{
		set(_bitsize, bit);
		return;
	}

	inline bool operator[](int n) const
	{
		return get(n);
	}

	template<class Ostream>
	Ostream &print( Ostream &os, int sep = 4 ) const
	{
		for(int i = 0; i < _bitsize; ++i)
		{
			if(i && i % 4 == 0)
				os << '\'';
			os << (get(i) ? 1 : 0);
		}
		return os;
	}

	NVX_SERIALIZABLE( &_bitsize, &(std::vector<uint8_t> &)*this );

private:
	int _bitsize = 0;
};





}

template<class Ostream>
inline Ostream &operator<<( Ostream &os, nvx::BitField const &toprint )
{
	toprint.print(os);
	return os;
}





#endif
