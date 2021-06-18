#include <algorithm>
#include <cstdio>
#include <iostream>
#include <iterator>
#include <functional>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <nvx/serialization.hpp>

#include "BitField.hpp"


using namespace nvx;
using namespace std;





// struct
struct item_type
{
	string key;
	wchar_t ch;
	int x;

	NVX_SERIALIZABLE( &key, &ch, &x );
};

bool char_cmp(item_type const &lhs, item_type const &rhs)
{
	return lhs.ch < rhs.ch;
}

bool key_cmp(item_type const &lhs, item_type const &rhs)
{
	return lhs.key < rhs.key;
}





void read_text(wstring &data)
{
	data.clear();
	wchar_t ch;
	while( wcin.get(ch) )
		data.push_back(ch);
	return;
}

void read_text(wstring &data, vector<item_type> &items)
{
    unordered_map<wchar_t, int> syms;

	data.clear();
	wchar_t ch;
    while( wcin.get(ch) )
    {
        data.push_back(ch);
        ++syms[ch];
    }

	items.clear();
	for(auto b = syms.begin(), e = syms.end(); b != e; ++b)
		items.push_back( { "", b->first, b->second } );
	items.push_back( { "", L'\0', 1 } );

	return;
}

void calculate_keys(vector<item_type> &items)
{
	if(items.size() < 2)
		return;

	sort(
		items.begin(), items.end(),
		[](item_type const &lhs, item_type const &rhs)->bool {
			return lhs.x > rhs.x;
		}
	);

	items[0].key = "0";
	items[1].key = "1";

	for(auto b = items.begin()+2, e = items.end(); b != e; ++b)
	{
		int x = b->x;
		auto min = items.begin();
		int minval = items.front().x + items.front().key.size() * x;
		int tmpval;

		for(auto bb = items.begin(); bb != b; ++bb)
		{
			tmpval = bb->x + bb->key.size() * x;
			if(tmpval < minval)
			{
				minval = tmpval;
				min = bb;
			}
		}

		b->key = min->key + "1";
		min->key.push_back('0');
	}
}

void write(wstring const &data, vector<item_type> &items)
{
	sort( items.begin(), items.end(), char_cmp );

	BitField bits;
	for(auto ch = data.begin(), e = data.end(); ch != e+1; ++ch)
	{
		auto it = lower_bound(
			items.begin(), items.end(),
			item_type { "", (ch == e ? '\0' : *ch), 0 }, char_cmp
		);

		if(it == items.end() or it->ch != ch)
			continue;

		for(auto bb = it->key.begin(), ee = it->key.end(); bb != ee; ++bb)
			bits.pushbit( *bb == '1' ? 1 : 0 );
	}

	archive<ostream> arch(&cout);
	arch << &items;
	arch << &bits;

	return;
}

void decode(vector<item_type> &items)
{
	BitField bits;
	{
		archive<istream> arch(&cin);
		arch >> &items >> &bits;
	}

	sort( items.begin(), items.end(), key_cmp );

	int i = 0;
	string key;
	vector<item_type>::const_iterator it;
	while(i < bits.bitsize())
	{
		key.clear();
		do
		{
			key.push_back( bits[i] ? '1' : '0' );
			++i;
			it = lower_bound(
				items.begin(), items.end(),
				item_type { key, L'\0', 0 }, key_cmp
			);
		}
		while( it->key != key );

		if(it->ch == '\0')
			break;
		wcout.put(it->ch);
	}

	return;
}

void read_codes(vector<item_type> &items)
{
	int n;
	string key;
	wchar_t ch;
	items.clear();
	cin >> n;
	for(int i = 0; i < n; ++i)
	{
		cin >> key;
		wcin >> ch;
		items.push_back( { key, ch, 0 } );
	}
	return;
}





// main
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

	wstring data;
	vector<item_type> items;

	if( argc > 1 && !strcmp(argv[1], "-d") )
	{
		decode(items);
		return 0;
	}

	if( argc > 1 && !strcmp(argv[1], "-c") )
	{
		read_codes(items);
		read_text(data);
	}
	else
	{
		read_text(data, items);
		calculate_keys(items);
	}

/*
 *     if( argc > 1 && !strcmp(argv[1], "-p"))
 *     {
 *         for(auto b = items.begin(), e = items.end(); b != e; ++b)
 *             printf("%s : %lc (%i)\n", b->key.c_str(), b->ch, b->x);
 *         return 0;
 *     }
 */

	write(data, items);

    return 0;
}
