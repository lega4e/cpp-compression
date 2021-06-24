/* BEGIN */





void test_find_match()
{
	vector<uint8_t> bytes {
		5, 6, 1, 0, 5
	};

	vector<uint8_t> sub1 = { 5, 6, 1, 0 };
	vector<uint8_t> sub2 = { 6, 1, 0, 5 };

	vector<uint8_t> sub3 = { 5, 6, 1 };
	vector<uint8_t> sub4 = { 6, 1, 0 };
	vector<uint8_t> sub5 = { 1, 0, 5 };

	vector<uint8_t> sub6 = { 5, 6 };
	vector<uint8_t> sub7 = { 6, 1 };
	vector<uint8_t> sub8 = { 1, 0 };
	vector<uint8_t> sub9 = { 0, 5 };

	vector<uint8_t> sub10 = { 8, 5 };


	int off, len;
	find_match(bytes.begin().base(), bytes.end().base(), sub1.begin().base(), sub1.end().base(), off, len);
	cout << "sub1: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub2.begin().base(), sub2.end().base(), off, len);
	cout << "sub2: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub3.begin().base(), sub3.end().base(), off, len);
	cout << "sub3: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub4.begin().base(), sub4.end().base(), off, len);
	cout << "sub4: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub5.begin().base(), sub5.end().base(), off, len);
	cout << "sub5: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub6.begin().base(), sub6.end().base(), off, len);
	cout << "sub6: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub7.begin().base(), sub7.end().base(), off, len);
	cout << "sub7: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub8.begin().base(), sub8.end().base(), off, len);
	cout << "sub8: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub9.begin().base(), sub9.end().base(), off, len);
	cout << "sub9: " << off << "|" << len << endl;

	find_match(bytes.begin().base(), bytes.end().base(), sub10.begin().base(), sub10.end().base(), off, len);
	cout << "sub10: " << off << "|" << len << endl;


	return;
}

void test_ctl()
{
	struct ctl_t { int off, len; };

	vector<ctl_t> ctls = {
		ctl_t { 7,    4  },
		ctl_t { 75,   15 },
		ctl_t { 0,    0  },
		ctl_t { 4095, 12 },
		ctl_t { 1023, 6  }
	};

	vector<uint8_t> bytes;
	for (ctl_t ctl : ctls)
		write_ctl(bytes, ctl.off, ctl.len);

	ctl_t ctl;
	uint8_t *b = bytes.begin().base();
	for (int i = 0; i < (int)ctls.size(); ++i)
	{
		read_ctl(b, ctl.off, ctl.len);
		printf("off: %i, len: %i\n", ctl.off, ctl.len);
		b += 2;
	}

	return;
}





/* END */
