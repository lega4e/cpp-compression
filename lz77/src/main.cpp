#include <cstring>
#include <iostream>
#include <vector>


using namespace std;






/*********************** GLOBAL OBJETS **********************/
constexpr int const BUFFER_SIZE = 4096;
constexpr int const WINDOW_SIZE = 15;





/******************** ASSISTIVE FUNCTIONS *******************/
/*
 * Возвращает первое вхождение длиннейшей подстроки [sb; E),
 * где E <= se в строку [b, e), в off в устанавливается
 * значение, такое что e-1-off = указателю на первый символ
 * подстроки, соответствующей sb. В len устанавливается длина
 * найденной подстроки. Если подстрока не найдена, то len = 0,
 * а off может содержать мусор. Подстрока может содержать
 * один символ.
 *
 * Сложность алгоритма: O((e - b)(se - sb))
 */
void find_match(
	uint8_t const *b,  uint8_t const *e,
	uint8_t const *sb, uint8_t const *se,
	int &off, int &len
)
{
	len = 0;
	int maxlen = se - sb;
	uint8_t const *match = nullptr;
	for (int clen = 0; b < e; ++b)
	{
		for (clen = 0; sb + clen < se && sb[clen] == b[clen % (e - b)]; ++clen);
		if (clen > len)
			match = b, len = clen;
		if (len == maxlen)
			break;
	}
	
	if (match)
		off = e - match - 1;

	return;
}

void write_ctl(vector<uint8_t> &target, int off, int len)
{
	target.push_back( uint8_t(off) );
	target.push_back((*((uint8_t *)&off + 1) & ((1 << 4) - 1) ) | (len << 4));
	return;
}

void read_ctl(uint8_t const *b, int &off, int &len)
{
	off = *b;
	*((uint8_t *)&off + 1) = *(b + 1) & ((1 << 4) - 1);
	len = *(b + 1) >> 4;
	return;
}





/********************** CORE FUNCTIONS **********************/
void lz77_encode(uint8_t const *b, uint8_t const *e, vector<uint8_t> &target)
{
	uint8_t const *bufbeg = b;
	uint8_t const *bufend = b;

	uint8_t const *winbeg = b;
	uint8_t const *winend = min(b + WINDOW_SIZE, e-1);

	int off = 0;
	int len = 0;

	while (bufend != e)
	{
		find_match(bufbeg, bufend, winbeg, winend, off, len);
		write_ctl(target, off, len);
		target.push_back(*(winbeg + len));

		// shift buffer
		winbeg += len + 1;
		winend  = min(winend + len+1, e-1);
		bufend  = min(bufend + len+1, e);
		bufbeg  = max(bufbeg, bufend - BUFFER_SIZE);
	}

	return;
}

void lz77_decode(uint8_t const *b, uint8_t const *e, vector<uint8_t> &target)
{
	int off, len;
	uint8_t *bufb, *winb;
	while (b < e)
	{
		read_ctl(b, off, len);
		b += 2;

		if (len)
		{
			target.resize(target.size() + len);
			winb = target.end().base() - len;
			bufb = target.end().base() - len - 1 - off;
			for (int i = 0; i < len; ++i)
				winb[i] = bufb[i % (off + 1)];
		}

		target.push_back(*b);
		++b;
	}

	return;
}





// main
int main( int argc, char *argv[] )
{
	vector<uint8_t> src = {
		1, 2, 1, 2,
		1, 2, 1, 2,
		1, 2, 1, 2
	};

	vector<uint8_t> enc;
	vector<uint8_t> dec;

	lz77_encode(src.begin().base(), src.end().base(), enc);
	// lz77_decode(enc.begin().base(), enc.end().base(), dec);

	// for (int i = 0; i < (int)dec.size(); ++i)
	// {
		// cout << (int)dec[i] << ' ';
		// if ((i + 1) % 4 == 0)
			// cout << '\n';
	// }

	cout.write((char const *)enc.data(), enc.size());

	return 0;
}





// end
