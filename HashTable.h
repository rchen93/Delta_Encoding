#ifndef HASH_H
#define HASH_H

#include <string>
#include <list>

using namespace std;

struct Record
{
public:
	Record(string sequence, int offset, int endpos)
	{ m_item = sequence; m_offset = offset; m_endpos = endpos; }
	string m_item;
	int m_offset;
	int m_endpos;
};

class HashTable
{
public:
	HashTable(int buckets);
	~HashTable();
	bool insert(string sequence, int offset);
	int search(string sequence);
	void printTable();
	void getProperties(int bucket, string sequence, int& offset, int& endpos);
private:
	int hashFunction(string sequence);
	int num_buckets;
	int filled_buckets;
	list<Record>** m_pointers;		//Array of pointers to list<Record>

};

bool searchList(list<Record>& l, string sequence);

#endif