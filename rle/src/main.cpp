#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>


using namespace std;





// functions
/*
 * Последовательность символов преобразуется в последовательность,
 * где символы чередуется с кодирующим байтом; сначала идёт
 * кодирующий байт, потом — символ или несколько символов.
 *
 * Пример:
 *
 * AAAABBCDCD -> 
 *
 * Схематично:
 *   4A2B4'CDCD // ' означает отсутствие повторений
 *
 * В битах (каждый байт отделён пробелом от другого; кодирующие
 * байты расписаны в битах):
 *   1000'0010 A 1000'0000 B 0000'0011 C D C D
 *
 * Описание формата кодирующих байтов:
 *   - Первый бит 0 обозначает, что повторений не будет
 *
 *   - Первый бит 1 обозначает, что повторения будут
 *
 *   - Следующие 7 битов говорят о том, сколько повторяющихся
 *     или сколько не повторяющихся символов будет далее.
 *
 *   - После кодирующего байта, если он сообщает о повторениях,
 *     идёт повторяющийся символ; после кодирующего байта,
 *     сообщающего об отсутствии повторений, идёт обозначенное
 *     в последних 7 битах число символов
 *
 *   - 1000'0000 — обозначает два повторения (0 + 2)
 *     1000'0001 — обозначает три повторения (1 + 2)
 *     1000'0010 — обозначает четыре повторения (2 + 2) и т.д.
 *
 *   - 0000'0000 — обозначает один неповторяющийся символ (0 + 1)
 *     0000'0001 — обозначает два неповторяющийхся символа (1 + 1)
 *     0000'0010 — обозначает три неповторяющийхся символа (2 + 1)
 *     и т.д.
 *
 */
void rle_encode(vector<uint8_t> const &src, vector<uint8_t> &target)
{
	target.clear();
	if (src.empty())
		return;

	uint8_t ps    = src[0]; // previous symbol
	int     count = 1;      // repeat count
	char    stage = '\0';

	for (auto b = src.begin()+1, e = src.end(); b != e+1; ++b)
	{
	switch_again:
		switch (stage)
		{
		case '\0':
			if (b == e)
			{
				stage = 'u';
				goto switch_again;
			}

			++count;
			if (*b == ps)
				stage = 'r';
			else
				stage = 'u';
			break;

		case 'r':
			if (b != e && *b == ps)
			{
				++count;
				break;
			}

			target.push_back( (1 << 7) | uint8_t(count - 2) );
			target.push_back( ps );
			count = 1;
			stage = '\0';
			break;

		case 'u':
			if (b != e && *b != ps)
			{
				++count;
				break;
			}

			if (b != e)
				--count;

			target.push_back(uint8_t(count-1));
			target.resize((int)target.size() + count);
			memcpy( (target.end() - count).base(), (b - count - (b == e ? 0 : 1)).base(), count );
			stage = 'r';
			count = 2;
			break;

		}
		
		if (b != e)
			ps = *b;
	}

	return;
}

void rle_decode(vector<uint8_t> const &src, vector<uint8_t> &target)
{
	uint8_t ctl;
	uint8_t ch;
	int     count;
	for (auto b = src.begin(), e = src.end(); b != e;)
	{
		ctl = *b, ++b;
		count = ctl & ((1 << 7) - 1);
		if (ctl & (1 << 7))
		{
			count += 2;
			if (b == e)
				throw "Decode error";

			ch = *b;
			for (int i = 0; i < count; ++i)
				target.push_back(ch);
			++b;
		}
		else
		{
			count += 1;
			while (count && b != e)
			{
				target.push_back(*b);
				--count, ++b;
			}

			if (count)
				throw "Decode error";
		}
	}

	return;
}

template<typename T>
bool equal(vector<T> const &lhs, vector<T> const &rhs)
{
	if (lhs.size() != rhs.size())
		return false;

	for (int i = 0; i < (int)lhs.size(); ++i)
	{
		if (lhs[i] != rhs[i])
			return false;
	}

	return true;
}





// main
int main( int argc, char *argv[] )
{

	try
	{
		vector<uint8_t> org = {
			'a', 'b', 'c', 'd', 'e',
			'a', 'a', 'a', 'a', 'b',
			'b', 'a', 'b', 'e', 'e', 'e',
			'a', 'a', 'a', 'a', 'a', 'd', 'c'
		};

		vector<uint8_t> enc, dec;
		rle_encode(org, enc);
		rle_decode(enc, dec);

		for (uint8_t b : dec)
			cout << b << " ";
		cout << endl;

		cout << (equal(org, dec) ? "Equal"s : "Not equal"s) << endl;
	}
	catch (char const *err)
	{
		fprintf(stderr, "%s\n", err);
	}



	return 0;
}





// end
