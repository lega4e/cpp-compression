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





/************************ FLAG HADING ***********************/
struct Cfg
{
	string ifname, ofname;
	char mode;
};

void print_help(char const *progname)
{
	printf(
		"Usage: %s [option]\n"
		"Options:\n"
		"  -e, --encode   : encode input\n"
		"  -d, --decode   : decode input\n"
		"  -i, --input    : set input file (by default there is stdin)\n"
		"  -o, --output   : set output file (by default there is stdout)\n"
		"  -h, --help     : print this help message and exit\n"
		"  -v, --version  : print version and exit\n",
		progname
	);
}

void print_version()
{
	printf("lz77 v1.0 by nvxden, MIT License\n");
	return;
}

Cfg handle_flags( int argc, char *argv[] )
{
	string ifname, ofname;
	char mode = 'e';

	for (int i = 1; i < argc; ++i)
	{
		if (!strcmp("-e", argv[i]) || !strcmp("--encode", argv[i]))
			mode = 'e';
		else if (!strcmp("-d", argv[i]) || !strcmp("--decode", argv[i]))
			mode = 'd';
		else if (!strcmp("-i", argv[i]) || !strcmp("--input", argv[i]) || argv[i][0] != '-')
		{
			if (argv[i][0] == '-')
			{
				++i;
				if (i >= argc)
				{
					fprintf(stderr, "Error: require input file name for option -i\n");
					exit(1);
				}
			}

			ifname = argv[i];
		}
		else if (!strcmp("-o", argv[i]) || !strcmp("--output", argv[i]))
		{
			++i;
			if (i >= argc)
			{
				fprintf(stderr, "Error: require input file name for option -i\n");
				exit(1);
			}

			ofname = argv[i];
		}
		else if (!strcmp("-h", argv[i]) || !strcmp("--help", argv[i]))
		{
			print_help(argv[0]);
			mode = 'h';
			break;
		}
		else if (!strcmp("-v", argv[i]) || !strcmp("--version", argv[i]))
		{
			print_version();
			mode = 'v';
			break;
		}
		else
		{
			fprintf(stderr, "Error: unknown flag %s\n", argv[i]);
			exit(1);
		}
	}

	return Cfg { move(ifname), move(ofname), mode };
}

FILE *openfile_or_exit(char const *filename, char const *mode)
{
	FILE *file = fopen(filename, mode);
	if (!file)
	{
		fprintf(stderr, "Error: can't open file %s\n", filename);
		exit(1);
	}

	return file;
}






// main
int main( int argc, char *argv[] )
{
	FILE *ifile = stdin;
	FILE *ofile = stdout;

	Cfg cfg = handle_flags(argc, argv);
	if (cfg.mode == 'v' || cfg.mode == 'h')
		return 0;



	// work
	vector<uint8_t> to;

	try
	{
		if (!cfg.ifname.empty())
			ifile = openfile_or_exit(cfg.ifname.c_str(), "r");

		vector<uint8_t> from;
		int ch;

		while ( (ch = getc(ifile)) != EOF )
			from.push_back((uint8_t)ch);

		if (cfg.mode == 'e')
			lz77_encode(from.begin().base(), from.end().base(), to);
		else
			lz77_decode(from.begin().base(), from.end().base(), to);
	}
	catch (char const *err)
	{
		fprintf(stderr, "%s\n", err);
	}

	if (ifile != stdin)
		fclose(ifile);



	// writing result
	if (!cfg.ofname.empty())
		ofile = openfile_or_exit(cfg.ofname.c_str(), "w");

	for (uint8_t ch : to)
		putc(ch, ofile);

	if (ofile != stdout)
		fclose(ofile);



	return 0;
}





// end
