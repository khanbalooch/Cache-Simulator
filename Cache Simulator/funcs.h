#include <iostream>
#include <wait.h>
#include <cstdlib>
#include <unistd.h>
using namespace std;

bool parseParams(int argc, char *argv[], bool &is_split, int &cache_capacity,
				 int &cache_blocksize, int &cache_associativity, int &Hitpolicy, int &mp);

class byte
{
  public:
	unsigned int data;
	unsigned int address;

	int &operator=(unsigned int x);
	void display();
};
//8888888888888888888888888888888888888888888
class Cache
{

  public:
	int Hitpolicy;
	int Misspolicy;
	int no_of_hits;
	int no_of_miss;
	int no_of_index;
	int is_split;
	int b;
	int c;
	int a;

	unsigned int *index;
	unsigned int *tag;
	unsigned int *dirty;
	int *is_valid;
	byte **L1D;
	byte **L1I;

	void Display();

	Cache(int blocksize, int cachesize, int hp, int mp)
	{
		no_of_miss = no_of_hits = 0;
		Hitpolicy = hp;
		Misspolicy = mp;
		b = blocksize;
		c = cachesize;
		no_of_index = cachesize / blocksize;
		a = 1;
		index = new unsigned int[no_of_index];
		tag = new unsigned int[no_of_index];
		dirty = new unsigned int[no_of_index];
		L1D = new byte *[no_of_index];
		is_valid = new int[no_of_index];
		for (int i = 0; i < no_of_index; i++)
		{

			L1D[i] = new byte[b];
		}

		unsigned int adr = 0;
		for (int i = 0; i < no_of_index; i++)
		{
			tag[i] += 0;
			is_valid[i] = 0;
			dirty[i] = 0;

			for (int j = 0; j < b; j++)
			{

				L1D[i][j] = 0;
				L1D[i][j].address = adr;
				adr += 1;
			}
		}
	}
};

class MainMemory
{

  public:
	byte *main;
	int size;
	int block;
	int no_of_index;
	void Display();

	MainMemory()
	{

		size = 16777216;
		block = 4;

		main = new byte[size];
		cout << "Initializing Main Memory\n";
		for (int i = 0; i < size; i++)
		{
			main[i] = i;
			main[i].address = i;
		}
		cout << "Memory Initilized\n";
	}
};
