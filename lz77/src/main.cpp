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
		for (clen = 0; sb + clen < se && b + clen < e && sb[clen] == b[clen]; ++clen);
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

void read_ctl(uint8_t *b, int &off, int &len)
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

	auto shift_buffer = [&]()->void {
		winbeg += len + 1;
		winend  = min(winend + len+1, e-1);
		bufend  = min(bufend + len+1, e);
		bufbeg  = max(bufbeg, bufend - BUFFER_SIZE);
	};

	while (bufend != e)
	{
		find_match(bufbeg, bufend, winbeg, winend, off, len);
		write_ctl(target, off, len);
		target.push_back(*(winbeg + len));
		shift_buffer();
	}

	return;
}





// main
int main( int argc, char *argv[] )
{
	vector<uint8_t> src = {
		1, 2, 1, 3, 1, 2, 1,
		1, 2, 1, 3, 1, 2, 1,
		3, 1, 2, 1, 1, 1
	};

	vector<uint8_t> target;

	lz77_encode(src.begin().base(), src.end().base(), target);
	cout.write((char const *)target.data(), target.size());

	return 0;
}





// end
