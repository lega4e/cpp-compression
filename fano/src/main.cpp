#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <functional>
#include <fstream>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <nvx/BitField.hpp>
#include <nvx/serialization.hpp>

#pragma GCC diagnostic ignored "-Wparentheses"


using namespace std;





/********************** GLOBAL OBJECTS **********************/
constexpr unsigned int const CONTROL_NUMBER = 3247928473u;

ifstream *fin                  = nullptr;
ofstream *fout                 = nullptr;
basic_ifstream<wchar_t> *wfin  = nullptr;
basic_ofstream<wchar_t> *wfout = nullptr;

string input_file, output_file;





/****************** BIT FIELD SERIALIZATION *****************/
namespace nvx
{
	template<typename Ostream, typename Meta>
	int serialize(
		nvx::archive<Ostream, Meta> &arch,
		nvx::BitField const *obj,
		bool write = true
	)
	{
		return
			serialize(arch, nvx::R<int>(obj->bitsize()), write) +
			serialize(arch, (std::vector<uint8_t> *)obj, write);
	}

	template<typename Ostream, typename Meta>
	int deserialize(
		nvx::archive<Ostream, Meta> &arch,
		nvx::BitField *obj
	)
	{
		int bitsize;
		int res =
			deserialize(arch, &bitsize) +
			deserialize(arch, (std::vector<uint8_t> *)obj);
		obj->bitresize(bitsize);
		return res;
	}
}





/************************ ITEM STRUCT ***********************/
/*
 * Структура, представляющая один символ, с
 * которым сопоставлено число встреч этого
 * символа во всём тексте и код Фано
 */
struct item_t
{
	string  key; // код символа по алгоритму Фано
	wchar_t ch;  // символ
	int     c;   // количество этих символов во всём тексте

	NVX_SERIALIZABLE(&key, &ch, &c);
};

bool char_cmp(item_t const &lhs, item_t const &rhs)
{
	return lhs.ch < rhs.ch;
}

bool key_cmp(item_t const &lhs, item_t const &rhs)
{
	return lhs.key < rhs.key;
}

bool count_cmp(item_t const &lhs, item_t const &rhs)
{
	return lhs.c < rhs.c;
}





/***************** INPUT AND OUTPUT STREAMS *****************/
typedef unique_ptr<istream, function<void(istream *)>>                               in_p;
typedef unique_ptr<ostream, function<void(ostream *)>>                               out_p;
typedef unique_ptr<basic_istream<wchar_t>, function<void(basic_istream<wchar_t> *)>> win_p;
typedef unique_ptr<basic_ostream<wchar_t>, function<void(basic_ostream<wchar_t> *)>> wout_p;

in_p input_stream()
{
	in_p in(&cin, [](istream *p) {
		if (p != &cin)
			delete p;
	});

	if (!input_file.empty())
	{
		in.reset(new ifstream(input_file));
		if (!*in)
		{
			fprintf(stderr, "Error: can't open file '%s'\n", input_file.c_str());
			throw 1;
		}
	}

	return in;
}

out_p output_stream()
{
	out_p out(&cout, [](ostream *p)->void {
		if (p != &cout)
			delete p;
	} );

	if (!output_file.empty())
	{
		out.reset(new ofstream(output_file));
		if (!*out)
		{
			fprintf(stderr, "Error: can't open file '%s'\n", output_file.c_str());
			throw 1;
		}
	}

	return out;
}

win_p winput_stream()
{
	win_p win(&wcin, [](basic_istream<wchar_t> *p) -> void {
		if (p != &wcin)
			delete p;
	});

	if (!input_file.empty())
	{
		win.reset(new basic_ifstream<wchar_t>(input_file));
		if (!*win)
		{
			fprintf(stderr, "Error: can't open file '%s'\n", input_file.c_str());
			throw 1;
		}
	}

	return win;
}

wout_p woutput_stream()
{
	wout_p wout(&wcout, [](basic_ostream<wchar_t> *p)->void {
		if (p != &wcout)
			delete p;
	} );

	if (!output_file.empty())
	{
		wout.reset(new basic_ofstream<wchar_t>(output_file));
		if (!*wout)
		{
			fprintf(stderr, "Error: can't open file '%s'\n", output_file.c_str());
			throw 1;
		}
	}

	return wout;
}





/******************** ASSISTIVE FUNCTIONS *******************/
void check_control_number(nvx::archive<istream> &arch)
{
	unsigned int contnum;
	arch >> &contnum;
	if (contnum != CONTROL_NUMBER)
		throw "Decode error: fail check control number";
	return;
}

void print_items(vector<item_t> const &items)
{
	int max = -1;
	for (item_t const &item : items)
	{
		if ((int)item.key.size() > max)
			max = (int)item.key.size();
	}

	char buf[32];
	sprintf(buf, "%%-%is", max);
	string
		tabfmt = string("\\t  : ")   + buf + " (%i)\n",
		nlnfmt = string("\\n  : ")   + buf + " (%i)\n",
		gphfmt = string("'%1lc' : ") + buf + " (%i)\n",
		nogfmt = string(" *  : ")    + buf + " (%i)\n";

	for (item_t const &item : items)
	{
		if ( item.ch == '\t' )
			printf(tabfmt.c_str(), item.key.c_str(), item.c);
		else if ( item.ch == '\n' )
			printf(nlnfmt.c_str(), item.key.c_str(), item.c);
		else if ( !iswgraph(item.ch) && item.ch != ' ' )
			printf(nogfmt.c_str(), item.key.c_str(), item.c);
		else
			printf(gphfmt.c_str(), item.ch, item.key.c_str(), item.c);
	}

	return;
}

/*
 * Считать все данные из stdin в строку, одновременно
 * подсчитывая количество встречающихся символов
 */
void read_text(wstring &data, vector<item_t> &items)
{
	unordered_map<wchar_t, int> syms;

	data.clear();
	wchar_t ch;
	auto win = winput_stream();
	while (win->get(ch))
	{
		data.push_back(ch);
		++syms[ch];
	}

	items.clear();
	for (auto b = syms.begin(), e = syms.end(); b != e; ++b)
		items.push_back( { "", b->first, b->second } );

	return;
}

/*
 * Расчитывает код по Фано для каждого символа в тексте
 */
void calculate_keys(vector<item_t> &items)
{
	if (items.size() < 2)
	{
		items.front().key = "0";
		return;
	}

	/*
	 * Отсортировываем по убыванию частоты символа
	 */
	sort(items.rbegin(), items.rend(), count_cmp);

	items[0].key = "0";
	items[1].key = "1";

	int c, minval, tmpval;
	for (auto b = items.begin()+2, e = items.end(); b != e; ++b)
	{
		c = b->c;
		auto min = items.begin();
		minval = items.front().c + items.front().key.size() * c;

		for (auto bb = items.begin(); bb != b; ++bb)
		{
			tmpval = bb->c + bb->key.size() * c;
			if (tmpval < minval)
			{
				minval = tmpval;
				min = bb;
			}
		}

		b->key = min->key + "1";
		min->key.push_back('0');
	}

	return;
}

/*
 * Кодирует данные из строки согласно кодам Фано в битовое поле
 */
void data2bitfield(wstring const &data, vector<item_t> &items, nvx::BitField &bits)
{
	sort( items.begin(), items.end(), char_cmp );

	for (auto ch = data.begin(), e = data.end(); ch != e; ++ch)
	{
		auto it = lower_bound(
			items.begin(), items.end(),
			item_t { "", *ch, 0 }, char_cmp
		);

		if (it == items.end() or it->ch != *ch)
		{
			fwprintf(stderr, L"Error: unknown symbol '%lc' in data2bitfield\n", ch);
			throw "Unknown symbol";
		}

		for (auto bb = it->key.begin(), ee = it->key.end(); bb != ee; ++bb)
			bits.pushbit( *bb == '1' ? 1 : 0 );
	}

	return;
}





/********************* ENCODE AND DECODE ********************/
/*
 * Считывает данные из stdin, на их основе расчитывает
 * ключи и записывает закодированные данные в stdout
 */
void encode()
{
	wstring        data;
	vector<item_t> items;
	nvx::BitField  bits;

	read_text(data, items);
	calculate_keys(items);
	data2bitfield(data, items, bits);

	auto out = output_stream();
	nvx::archive(out.get()) <<
		nvx::R<unsigned int>(CONTROL_NUMBER) <<
		&items << &bits;

	return;
}

/*
 * Считывает закодированные данные из stdin, декодирует
 * и записывает получившийся результат в stdout
 */
void decode()
{
	vector<item_t> items;
	nvx::BitField  bits;

	{
		auto in = input_stream();
		nvx::archive arch(in.get());
		check_control_number(arch);
		arch >> &items >> &bits;
	}

	sort( items.begin(), items.end(), key_cmp );

	auto wout = woutput_stream();
	int bitn = 0;
	string key;
	vector<item_t>::const_iterator it;
	while (bitn < bits.bitsize())
	{
		key.clear();
		do
		{
			if (bitn >= bits.bitsize())
				throw "Decode error";

			key.push_back( bits[bitn] ? '1' : '0' );
			++bitn;
			it = lower_bound(
				items.begin(), items.end(),
				item_t { key, L'\0', 0 }, key_cmp
			);
		}
		while ( it->key != key );
		wout->put(it->ch);
	}

	return;
}





// main
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	char action = 0;

	// parse options
	for (int i = 1; i < argc; ++i)
	{
		if ( !strcmp(argv[i], "-e") )
			action = 'e';
		else if ( !strcmp(argv[i], "-d") )
			action = 'd';
		else if ( !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") )
			action = 'h';
		else if ( !strcmp(argv[i], "--keys-encoded") )
			action = 'E';
		else if ( !strcmp(argv[i], "--keys-origin") )
			action = 'O';
		else if ( !strcmp(argv[i], "-i") )
		{
			if ( ++i >= argc )
			{
				fprintf(stderr, "Error: required argument for '-i'\n");
				return 1;
			}

			input_file = argv[i];
		}
		else if ( !strcmp(argv[i], "-o") )
		{
			if ( ++i >= argc )
			{
				fprintf(stderr, "Error: required argument for '-o'\n");
				return 1;
			}

			output_file = argv[i];
		}
		else
		{
			cerr << "Unknown option: '" << argv[i] << "'; try -h for usage" << endl;
			return 1;
		}
	}


	// work
	try
	{
		switch (action)
		{
		case 'e':
			encode();
			break;

		case 'd':
			decode();
			break;

		case 'E':
		  {
			vector<item_t> items;
			auto in = input_stream();
			nvx::archive arch(in.get());
			check_control_number(arch);
			arch >> &items;
			sort(items.rbegin(), items.rend(), count_cmp);
			print_items(items);
			break;
		  }

		case 'D':
		  {
			vector<item_t> items;
			wstring        data;
			read_text(data, items);
			calculate_keys(items);
			sort(items.rbegin(), items.rend(), count_cmp);
			print_items(items);
			break;
		  }

		case 'h': case 0:
			(action == 0 ? cerr : cout) <<
				"Usage: " << argv[0] << " [options]\n"
				"Options:\n"
				"  -h, --help     : show this\n" <<
				"  -e             : encode data\n"
				"  -d             : decode data\n"
				"  -i <file>      : set input file\n"
				"  -o <file>      : set output file\n"
				"  --keys-encoded : read encoded data and print keys\n"
				"  --keys-origin  : read original data, calculate keys and print it" <<
			endl;
			break;

		}
	}
	catch (char const *err)
	{
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	catch (int errcode)
	{
		return errcode;
	}


	return 0;
}





// END
