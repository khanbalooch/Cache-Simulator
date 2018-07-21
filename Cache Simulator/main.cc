#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <cmath>
#include "funcs.h"

#define CACHE_READ 0
#define CACHE_WRITE 1

using namespace std;
////////////////////////////////////calculating memory address for writing on eviction ////////////////////////////////////////////////////
int addresscal(Cache &cache, int ind, int tag)
{
	int offsetbits = log2(cache.b);
	int indexbits = log2(cache.c / cache.b);
	int sum[3] = {0, 0, 0};
	bitset<32> index = ind;
	bitset<32> tage = tag;
	int j = 0;
	for (int i = 0; i < 32; i++)
	{
		if (i < offsetbits + indexbits)
		{
			sum[1] += index[i] * pow(2, i + offsetbits);
		}
		else
		{
			sum[2] += tage[j] * pow(2, (i + offsetbits + indexbits));
			j++;
		}
	}
	return sum[1] + sum[2];
}
//////////////////////////////////////calculting tag bit and offset///////////////////////////////////////////////////////////////////////
void cachebits(bitset<32> address, Cache &cache, int &tag, int &offset, int &index)
{

	int offsetbits = log2(cache.b);
	int indexbits = log2(cache.c / cache.b);
	int sum[3] = {0, 0, 0};

	for (int i = 0; i < 32; i++)
	{
		if (i < offsetbits)
		{
			int x = address[i];
			sum[0] += address[i] * pow(2, i);
		}
		else if (i < offsetbits + indexbits)
		{
			int x = address[i];
			sum[1] += address[i] * pow(2, i - offsetbits);
		}
		else
		{
			int x = address[i];
			sum[2] += address[i] * pow(2, i - offsetbits - indexbits);
		}
	}
	offset = sum[0];
	index = sum[1];
	tag = sum[2];
}

//////////////////////////////////function from reading cache,memory///////////////////////////////////////////////////////////////////
unsigned int read(int address, int tag, int index, int offset, Cache &cache, MainMemory &Main)
{
	unsigned int value;
	int mindex = (address / cache.b) * cache.b;

	if (cache.is_valid[index]) //checking validity
	{
		if (cache.tag[index] == tag) //comparing tag
		{
			cout << "found in cache\n";
			cache.no_of_hits++;
			value = cache.L1D[index][offset].data;
		}
		else //tag donot match
		{
			cache.no_of_miss++;
			cout << "NOT found in cache\n";
		}
	}
	else //not valid
	{
		cache.no_of_miss++;
		cout << "NOT found in cache\n";
		value = Main.main[address].data;

		cache.is_valid[index] = 1;
		cache.tag[index] = tag;

		for (int i = 0; i < cache.b; i++)
		{
			cache.L1D[index][i] = Main.main[mindex];
			mindex++;
		}
	}

	return value;
}
////////////////////////////function for writng in memory with cache ///////////////////////////////////////////////////////////////////
/////////////////////////////                                       ////////////////////////////////////////////////////////////////////

unsigned int write(int address, int tag, int index, int offset, Cache &cache, MainMemory &Main, int data0, int hp, int mp)
{
	unsigned int value;
	int mindex = (address / cache.b) * cache.b;

	////////////////////////////////////////////////////////wb-wa///////////////////////////////////////////////////////////////////////////
	if (hp == 0 && mp == 2)
	{
		if (cache.is_valid[index]) //check cache validity
		{
			if (cache.tag[index] == tag) //tag comparison
			{
				cache.no_of_hits++;
				cout << "found in cache\n";
				cache.L1D[index][offset] = data0;
				cache.dirty[index] = 1;
			}
			else //tag donot match
			{
				cache.no_of_miss++;
				if (cache.dirty[index] == 1)
				{
					int adr = addresscal(cache, index, cache.tag[index]);
					for (int i = 0; i < cache.b; i++)
					{
						Main.main[adr].data = cache.L1D[index][i].data;
						mindex++;
						adr++;
					}
				}
				cout << "NOT found in cache\n";
				cache.is_valid[index] = 1;
				cache.tag[index] = tag;
				cache.dirty[index] = 1;
				for (int i = 0; i < cache.b; i++)
				{
					cache.L1D[index][i].data = Main.main[mindex].data;
					mindex++;
				}
				cache.L1D[index][offset].data = data0;
			}
		}
		else //not valid
		{
			cache.no_of_miss++;
			cout << "NOT found in cache\n";
			cache.is_valid[index] = 1;
			cache.tag[index] = tag;
			cache.dirty[index] = 1;
			for (int i = 0; i < cache.b; i++)
			{
				cache.L1D[index][i].data = Main.main[mindex].data;
				mindex++;
			}
			cache.L1D[index][offset].data = data0;
		}
	}

	/////////////////////////////////////////////////////////wt-wa/////////////////////////////////////////////////////////////////////////////

	if (hp == 1 && mp == 2)
	{
		if (cache.is_valid[index]) //check cache validity
		{
			if (cache.tag[index] == tag) //tag comparison
			{
				cache.no_of_hits++;
				cout << "found in cache\n";
				cache.L1D[index][offset] = data0;
				Main.main[address].data = data0;
			}
			else //tag donot match
			{
				if (cache.dirty[index] == 1)
				{
					int adr = addresscal(cache, index, cache.tag[index]);
					for (int i = 0; i < cache.b; i++)
					{
						Main.main[adr].data = cache.L1D[index][i].data;
						mindex++;
						adr++;
					}
				}
				cache.no_of_miss++;
				cout << "NOT found in cache\n";
				cache.is_valid[index] = 1;
				cache.tag[index] = tag;
				cache.dirty[index] = 1;
				for (int i = 0; i < cache.b; i++)
				{
					cache.L1D[index][i].data = Main.main[mindex].data;
					mindex++;
				}
				cache.L1D[index][offset].data = data0;
			}
		}
		else //not valid
		{
			cache.no_of_miss++;
			cout << "NOT found in cache\n";
			cache.is_valid[index] = 1;
			cache.tag[index] = tag;
			cache.dirty[index] = 1;
			{
				for (int i = 0; i < cache.b; i++)
					cache.L1D[index][i].data = Main.main[mindex].data;
				mindex++;
			}
			cache.L1D[index][offset].data = data0;
		}
	}
	//////////////////////////////////////////////wb-wn////////////////////////////////////////////////////////////////////////////////////////

	if (hp == 0 && mp == 3)
	{
		if (cache.is_valid[index]) //check cache validity
		{
			if (cache.tag[index] == tag) //tag comparison
			{
				cache.no_of_hits++;
				cout << "found in cache\n";
				cache.L1D[index][offset] = data0;
				cache.dirty[index] = 1;
			}
			else //tag donot match
			{
				cache.no_of_miss++;
				Main.main[address].data = data0;
			}
		}
		else //Not valid
		{
			cache.no_of_miss++;
			Main.main[address].data = data0;
		}
	}
	/////////////////////////////////////////////////////////wt-wn/////////////////////////////////////////////////////////////////////////////

	if (hp == 1 && mp == 3)
	{
		if (cache.is_valid[index]) //check cache validity
		{
			if (cache.tag[index] == tag) //tag comparison
			{
				cache.no_of_hits++;
				cout << "found in cache\n";
				cache.L1D[index][offset] = data0;
				Main.main[address].data = data0;
			}
			else //tag donot match
			{
				cache.no_of_miss++;
				Main.main[address].data = data0;
			}
		}
		else //Not valid
		{
			cache.no_of_miss++;
			Main.main[address].data = data0;
		}
	}
	//////////////////////////////////////                            //////////////////////////////////////////////////////////////////////
	return value;
}

int main(int argc, char *argv[])
{

	enum
	{
		wb,
		wt,
		wa,
		wn
	};

	int Hitpolicy = wb, Misspolicy = wa;

	int cache_capacity = 0;
	int cache_blocksize = 0;
	int cache_associativity = 0;
	int tag, offset, index;
	bool is_split = false;

	int i;
	int read_write;
	int address_in_hex;
	unsigned int data;
	bitset<32> address_in_bin;

	if (!parseParams(argc, argv, is_split, cache_capacity, cache_blocksize, cache_associativity, Hitpolicy, Misspolicy))
	{
		exit(2);
	}
	Cache cache(cache_blocksize, cache_capacity, Hitpolicy, Misspolicy);
	MainMemory Main;
	while (!feof(stdin))
	{
		//read in whether to read or write to the cache
		cin >> dec >> read_write;
		// check again if we have reached the end
		// as this flag is set only after a 'cin'
		if (feof(stdin))
			break;

		cin >> hex >> address_in_hex;
		address_in_bin = address_in_hex;
		cout << dec << address_in_hex;
		cachebits(address_in_bin, cache, tag, offset, index);

		//if it is a cache write the we have to read the data
		if (read_write == CACHE_WRITE)
		{
			cin >> hex >> data;
			write(address_in_hex, tag, index, offset, cache, Main, data, Hitpolicy, Misspolicy);
		}
		else
		{
			read(address_in_hex, tag, index, offset, cache, Main);
		}
	}
	cache.Display(); //cache displaying
	Main.Display();  //displaying memory
}
