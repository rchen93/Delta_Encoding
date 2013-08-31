#include <iostream>
#include <fstream>			// std::ifstream, std::ofstream
#include <string>
#include <cassert>
#include "HashTable.h"

using namespace std;

const int N_CHAR = 16;		// number of characters to read in at a time

bool getNumber(istream& inputf, int& n)
{
	char ch;
	if (!inputf.get(ch)  ||  !isascii(ch)  ||  !isdigit(ch))
		return false;
	inputf.unget();
	inputf >> n;
	return true;
}

bool getCommand(istream& inputf, char& cmd, int& length, int& offset)
{
	if (!inputf.get(cmd)  ||  (cmd == '\n'  &&  !inputf.get(cmd)) )
	{
		cmd = 'x';  // signals end of file
		return true;
	}

	char ch;
	switch (cmd)
	{
	case 'A':
		return getNumber(inputf, length) && inputf.get(ch) && ch == ':';
	case 'C':
		return getNumber(inputf, length) && inputf.get(ch) && ch == ',' && getNumber(inputf, offset);
	}
	return false;
}

bool applyDelta(istream& oldf, istream& deltaf, ostream& newf)
{
	char ch, cmd;
	int length, offset;
	string temp;

	while (oldf.get(ch))
		temp += ch;

	while (getCommand(deltaf, cmd, length, offset) && cmd != 'x')
	{
		if (cmd == 'A')
		{
			for (int i = 0; i < length; i++)
			{
				deltaf.get(ch);
				newf << ch;
			}
		}
		else if (cmd == 'C')
		{
			for (int i = 0; i < length; i++)
			{
				newf << temp[i+offset];
			}
		}
	}
	if (cmd = 'x')
		return true;
	else
	{
		cerr << "This is not a valid delta file!" << endl;
		return false;
	}
}

void createDelta(istream& oldf, istream& newf, ostream& deltaf)
{
	char ch;
	string old_text, new_text, temp;
	string tempdelta = "";
	int bucket_size, offset;
	int counter = 0;
	int numInsert = 0;
	int copyBytes = 0;
	int record_offset;
	int record_endpos;
	int addCounter = 0;
	int addPos = 1;
	int deleteLength = 0;
	int insertLength = 1;

	// Read old/new text into strings
	while (oldf.get(ch))
		old_text += ch;
	while (newf.get(ch))
		new_text += ch;

	// Determine bucket size for hash table
	if (old_text.length() > new_text.length())
		bucket_size = (old_text.length()/N_CHAR);
	else
		bucket_size = (new_text.length()/N_CHAR);

	HashTable* table = new HashTable(2*N_CHAR*bucket_size);

	// Place strings from the old file of N_CHAR length into hash table
	for (int i = 0; i < old_text.length(); i++)
	{
		temp += old_text[i];
		counter++;
		if (counter == N_CHAR)
		{
			offset = i - counter + 1;
			table->insert(temp, offset);
			temp.erase(0,1);
			numInsert++;
			counter--;							
		}
	}

	//cerr << "Number of insertions: " << numInsert << endl;
	//table->printTable();

	temp = "";
	counter = 0;

	// Processing the new file
	for (int i = 0; i < new_text.length(); i++)
	{
		temp += new_text[i];
		counter++;
		// if N_CHAR sequence has been read in
		if (counter == N_CHAR)
		{
			int bucket = table->search(temp);
			// Copy
			if (bucket != -1)
			{
				table->getProperties(bucket, temp, record_offset, record_endpos);

				int k = 0;
				for (int j = record_offset; j < old_text.length(); j++)
				{
					if (old_text[j] == temp[k])
					{
						k++;
						copyBytes++;
					}
					if (k == temp.length())
						break;
				}

				// N_CHARS matched, continue searching

				for (int m = record_endpos+1; m < old_text.length(); m++)
				{
					if (i < new_text.length() - 1)
					{
						i++;
						if (old_text[m] == new_text[i])
						{
							temp += new_text[i];
							copyBytes++;
						}
						else
						{
							i--;
							break;
						}
					}
				}

				tempdelta += "C";
				tempdelta += to_string(static_cast<long long>(copyBytes));
				tempdelta.push_back(',');
				tempdelta += to_string(static_cast<long long>(record_offset));
				counter = 0;
				temp = "";
				copyBytes = 0;
				addCounter = 0;
				insertLength = 1;
				continue;

			}

			// Add one character
			else
			{
				if (addCounter == 0)
				{
					addCounter++;
					tempdelta += "A";
					tempdelta += to_string(static_cast<long long>(insertLength));
					addPos = tempdelta.length() - 1;
					tempdelta.push_back(':');
					tempdelta += temp[0];
					temp.erase(0,1);
					//cerr << "Temp length: " << temp.length() << endl;
					counter--;	
					continue;
				}

				// if there has been a previous add, just change the number and append to string
				else
				{
					addCounter++;

					// update number of characters to insert
					insertLength++;

					// find number of integers to delete
					for (int i = addPos; i < tempdelta.length() - 1; i++)
					{
						if (!isdigit(tempdelta[i]))
							break;
						deleteLength++;
					}
					//cerr << "Length to be deleted: " << delete_length << endl;

					// erase it
					tempdelta.erase(addPos, deleteLength);

					// insert the new number into deltastring
					string addNumber = to_string(static_cast<long long>(insertLength));
					tempdelta.insert(addPos, addNumber);

					// add the character to the deltastring
					tempdelta += temp[0];
					temp.erase(0,1);

					counter--;	
					deleteLength = 0;
					continue;
				}
			}
		}
	}

	// if you got here, you read in the entire file, but there may be left-over characters in the tempstring
	//cerr << "Remaining string: " << temp << endl;
	while (temp != "")
	{
		int bucket = table->search(temp);
		// Copy
		if (bucket != -1)
		{
			table->getProperties(bucket, temp, record_offset, record_endpos);
			copyBytes = temp.length();
			tempdelta += "C";
			tempdelta += to_string(static_cast<long long>(copyBytes));
			tempdelta.push_back(',');
			tempdelta += to_string(static_cast<long long>(record_offset));
			counter = 0;
			temp = "";
			copyBytes = 0;
			addCounter = 0;
			insertLength = 1;
			break;
		}

		// Add one character
		else
		{
			addCounter++;

			//update number of characters inserted
			insertLength++;

			// find number of integers to delete
			for (int i = addPos; i < tempdelta.length(); i++)
			{
				if (!isdigit(tempdelta[i]))
					break;
				deleteLength++;
			}
			//cerr << "Length to be deleted: " << delete_length << endl;

			// erase it
			tempdelta.erase(addPos, deleteLength);

			// insert the new number into the deltastring
			string addNumber =  to_string(static_cast<long long>(insertLength));
			tempdelta.insert(addPos, addNumber);

			// add the character to the deltastring
			tempdelta += temp[0];
			temp.erase(0,1);

			counter--;
			deleteLength = 0;
			continue;
		}

	}

	deltaf << tempdelta;
}

bool runTest(string oldname, string newname, string deltaname, string newname2)
{
	ifstream oldfile(oldname);
	if (!oldfile)
	{
		cerr << "Cannot open " << oldname << endl;
		return false;
	}
	ifstream newfile(newname);
	if (!newfile)
	{
		cerr << "Cannot open " << newname << endl;
		return false;
	}
	ofstream deltafile(deltaname);
	if (!deltafile)
	{
		cerr << "Cannot create " << deltaname << endl;
		return false;
	}
	createDelta(oldfile, newfile, deltafile);
	deltafile.close();

	oldfile.clear();   // clear the end of file condition
	oldfile.seekg(0);  // reset back to beginning of the file
	ifstream deltafile2(deltaname);
	if (!deltafile2)
	{
		cerr << "Cannot read the " << deltaname << " that was just created!" << endl;
		return false;
	}
	ofstream newfile2(newname2);
	if (!newfile2)
	{
		cerr << "Cannot create " << newname2 << endl;
		return false;
	}
	assert(applyDelta(oldfile, deltafile2, newfile2));
	cout << "You must compare " << newname << " and " << newname2 << endl;
	cout << "If they are not identical, you failed this test." << endl;
	return true;
}

int main()
{
	runTest("Test_Files/warandpeace1.txt", "Test_Files/warandpeace2.txt", "Test_Files/deltaf.txt", "Test_Files/newf.txt");
}