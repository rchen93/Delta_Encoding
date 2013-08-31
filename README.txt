DISCLAIMER: My solution to a school assignment.

This program will generate a delta file that shows the differences between two similar files and will apply the delta file to update the earlier version into the newer version.
Some sample test files will be provided.

The delta file consists of two commands:
Copy: Specifies number of characters that should be coped from a particular offset in the earlier version
	  Format: C, followed by number of characters to copy, a comma, and the offset to start copying from
	  in the earlier version of the file, (e.g. C23,0 would copy 23 characters from offset 0 )

Add:  Adds entirely new characters when it cannot be located and copied from the earlier version
	  Format: A, followed by number of characters to add, followed by a colon, and the actual characters to append, (e.g. A2:XY would add 2 characters, XY)

Limitations:
-Required to use hash table or binary search tree
-Only containers from C++ standard library that we can use are: vector, list, stack, queue, string

Data Structure:
-Open Hash Table with Linked Lists
	-Size of hash table determined by number of characters in file and length of N-byte sequences I wanted to read in
	-Lists were of class Record, which contained the string, the offset, and the endingposition of the string   with respect to the old file.
-Hashing Function: DJB2 hashing algorithm

Functions:
void createDelta(istream& oldf, istream& newf, ostream& deltaf)
	  Takes contents of two files, A and A', and produces a delta file containing instructions for converting
	  A into A'.
bool applyDelta(istream& oldf, istream& deltaf, ostream& newf)
	  Takes contents of file A and a delta file, and applies instructions of delta to produce new file A'
	  Returns true if operation succeeds
	  Returns false if delta file is malformed, (e.g. a character other than A or C where a command is expected)

Benchmarks:
											Windows file sizes in bytes
							old   			new 			prof's delta 		my delta    
Small-Mart inventory		105				141				95					95
Weird Al's Dr. Seuss		533				606				69					120
War and Peace			    77285			77333			399					389
Strange files               100764			100764			8746				8573

High-Level Algorithm:

1. Read in entire contents of the old file and the new file into two separate strings.
2. For all consecutive N-character sequences in the old file's string, insert that N-character sequence and the offset F where it was found in the old file's string into a hash table.
3. Once you have filled up your table with all N-byte sequences from the source file, start processing new file's string, starting from offset j=0, until j reaches the end of the string
	a. Look up the N-byte sequence which starts at offset j ([j,j+N-1]) in the new file's string in the hash table
	b. If you find this sequence,
		i. Determine how long the match goes on (match length L)
		ii. Write a Copy instruction to the delta file to copy L bytes from offset F from the source file
		iii. Go back to step 3a, continuing at offset j = j+L in the new file's string
	c. If you don't find the current sequence,
		i. If there has not been a previous add,
			-Write an instruction to the delta file to Add the current character
		ii. If there was a previous add
			-Simply change the number of characters to add and Add the current character
		iii. Go back to step 3a, to find the next N-byte sequence in the hash table