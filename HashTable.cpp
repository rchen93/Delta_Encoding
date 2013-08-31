#include "HashTable.h"
#include <iostream>
using namespace std;

bool searchList(list<Record>& l, string sequence) 
{
	list<Record>::iterator iter, end;
	iter = l.begin();
	end = l.end();

	while (iter != end)
	{
		if (iter->m_item == sequence)
			return true;
		iter++;
	}

	// Element does not exist in list
	return false;
}

HashTable::HashTable(int buckets)
{
	num_buckets = buckets;
	filled_buckets = 0;

	// Array of size buckets of pointers that point to a list<Record>
	m_pointers = new list<Record>*[buckets];

	// Initialize each pointer in the array as NULL
	for (int i = 0; i < buckets; i++)
		m_pointers[i] = NULL;

}

HashTable::~HashTable()
{
	for (int i = 0; i < num_buckets; i++)
	{
		m_pointers[i]->clear();
		delete m_pointers[i];
	}
	delete m_pointers;
}

int HashTable::hashFunction(string sequence)
{
	// djb2 hashing algorithm
	unsigned long hash = 5381;
	for(int i = 0; i < sequence.size(); i++)
		hash = (hash << 5) + hash + sequence[i];
	return hash % num_buckets;
}


bool HashTable::insert(string sequence, int offset)
{
	// Calculate slot to put item in
	int bucket = hashFunction(sequence);

	// Check the slot is not occupied
	if (m_pointers[bucket] == NULL)
	{
		int endpos = offset+sequence.length() - 1;
		int stringlength = sequence.length();
		Record toBeInserted = Record(sequence, offset, endpos);
		m_pointers[bucket] = new list<Record>;
		m_pointers[bucket]->push_back(toBeInserted);
		filled_buckets++;
		//cerr << "Bucket: " << bucket << endl;
		//cerr << "Item: " << sequence << endl;
		//cerr << "Offset: " << offset << endl;
		return true;
	}

	// If occupied, check if there is a duplicate before adding to linked list
	else
	{
		if (searchList(*m_pointers[bucket], sequence))
			return false;
		else
		{
			int endpos = offset+sequence.length() - 1;
			int stringlength = sequence.length();
			Record toBeInserted = Record(sequence, offset, endpos);
			m_pointers[bucket]->push_back(toBeInserted);
			filled_buckets++;
			//cerr << "Bucket: " << bucket << endl;
			//cerr << "Item: " << sequence << endl;
			//cerr << "Offset: " << offset << endl;
			return true;
		}
	}

}

int HashTable::search(string sequence)
{
	int bucket = hashFunction(sequence);

	if (m_pointers[bucket] == NULL)
		return -1;
	else
	{
		if(searchList(*m_pointers[bucket], sequence))
		{
			//cerr << "Item exists!" << endl;
			return bucket;
		}
		//cerr << "Item does not exist!" << endl;
		return -1;
	}
}

void HashTable::printTable()
{
	cerr << "Total buckets: " << num_buckets << endl;
	cerr << "Number of buckets filled: " << filled_buckets << endl;

	for (int i = 0; i < num_buckets; i++)
	{
		if (m_pointers[i] == NULL)
			continue;
		else
		{
			cerr << "Bucket is: " << i << endl;

			list<Record>::iterator iter, end;
			iter = m_pointers[i]->begin();
			end = m_pointers[i]->end();

			while (iter != end)
			{
				cerr << "Item is: " << iter->m_item << endl;
				cerr << "Offset is: " << iter->m_offset<< endl;
				iter++;
			}
			cerr << endl;
		}
	}
}

void HashTable::getProperties(int bucket, string sequence, int& offset, int& endpos)
{
	list<Record>::iterator iter, end;
	iter = m_pointers[bucket]->begin();
	end = m_pointers[bucket]->end();

	while (iter != end)
	{
		if (iter->m_item == sequence)
			break;
		iter++;
	}

	offset = iter->m_offset;
	endpos = iter->m_endpos;
}