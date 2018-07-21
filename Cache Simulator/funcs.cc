#include <stdio.h>
#include <unistd.h> //for the getopt function
#include <stdlib.h> //for atoi
#include <iostream>
#include <iomanip>
#include <cmath>
#include "funcs.h"
using namespace std;

bool parseParams(int argc, char *argv[], bool &is_split, int &cache_capacity,
				 int &cache_blocksize, int &cache_associativity, int &Hitpolicy, int &mp)
{
	//needed for the parsing of command line options
	int c;

	bool s_flag, c_flag, b_flag, a_flag;
	bool errflg = false;

	s_flag = c_flag = b_flag = a_flag = errflg = false;

	extern char *optarg;
	extern int optind, optopt;

	while ((c = getopt(argc, argv, "sc:b:a:w:")) != -1)
	{
		switch (c)
		{

		case 's':
			s_flag = true;
			is_split = s_flag;
			break;

		case 'c':
			cache_capacity = atoi(optarg);
			c_flag = true;
			break;
		case 'b':
			cache_blocksize = atoi(optarg);
			b_flag = true;
			break;
		case 'a':
			cache_associativity = atoi(optarg);
			a_flag = true;
			break;
		case 'w':

			switch (*optarg)
			{
			case 'a': //cout<<"write allocate strategy\n";
				mp = 2;
				break;
			case 'b': //cout<<"write back strategy\n";
				Hitpolicy = 0;
				break;
			case 't': //cout<<"write through strategy\n";
				Hitpolicy = 1;
				break;
			case 'n': //cout<<"write no-allocate strategy\n";
				mp = 3;
				break;
			}
			break;
		case ':': /* -c without operand */
			fprintf(stderr,
					"Option -%c requires an operand\n", optopt);
			errflg++;
			break;
		case '?':
			fprintf(stderr, "Unrecognised option: -%c\n", optopt);
			errflg = true;
		}
	}

	//check if we have all the options and have no illegal options
	if (errflg || !c_flag || !b_flag || !a_flag)
	{
		fprintf(stderr, "usage: %s -c<capacity> -b<blocksize> -a<associativity>\n", argv[0]);
		return false;
	}

	return true;
}

void byte::display()
{

	cout << setw(8) << hex << data;
}

int &byte::operator=(unsigned int x)
{
	data = x;
}

//=========================================

void Cache::Display()
{

	if (Hitpolicy == 0)
	{
		cout << "Hit Policy           :writeback\n";
	}
	if (Hitpolicy == 1)
	{
		cout << "Hit Policy           :writethrough\n";
	}
	if (Misspolicy == 2)
	{
		cout << "Miss Policy         : write-allocate\n";
	}
	if (Misspolicy == 3)
	{
		cout << "Miss Policy         : write-no-allocate\n";
	}

	cout << "Cache Capacity       : " << c << endl;
	cout << "Cache BlockSize      : " << b << endl;
	cout << "Cache Associativity  : " << a << endl;
	cout << "No. of index        : " << no_of_index << endl;
	cout << "num of misses        :" << no_of_miss << endl;
	cout << "num of hits             :" << no_of_hits << endl;

	if (is_split)
	{
		cout << "cache is split " << endl;
	}
	else
	{
		cout << "cache is unified " << endl;
	}

	cout << "IndexNO        TAG";

	for (int i = 0; i < b; i++)

	{
		cout << setw(9) << i << "       ";
	}
	cout << "\n";

	for (int i = 0; i < no_of_index; i++)
	{

		cout << setfill('0') << setw(8) << i;
		cout << "     " << setw(10) << tag[i];

		for (int j = 0; j < b; j++)
		{
			cout << "   ";
			L1D[i][j].display();
		}
		cout << "\n";
	}
}
//=========================================

void MainMemory::Display()
{
	cout << "    Address";
	for (int i = 0; i < 8; i++)
	{
		cout << i << "         ";
	}
	cout << endl;
	for (int i = 4242572; i < 4243596;)
	{

		cout << setw(10) << hex << main[i].address;
		for (int j = 0; j < 8; j++)
		{
			cout << "     ";
			main[i].display();
			i++;
		}

		cout << "\n";
	}
}
