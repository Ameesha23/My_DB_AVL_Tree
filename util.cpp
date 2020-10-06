/*util.cpp*/

//
// Utility functions for myDB project
//
// Ameesha Saxena
// U. of Illinois, Chicago
// CS 251: Fall 2019
// Project #04
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "util.h"

using namespace std;


//
// EchoData
//
// Reads the contents of a table's .data file, and outputs the
// values for each record (one record per line).  Pass the table
// name, the record size, and the # of columns per record.
//
// Example: EchoData("students", 82, 5) would output the contents
// of "students.data".
//
void EchoData(string tablename, int recordSize, int numColumns)
{
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return;
  }

  //
  // Okay, read file record by record, and output each record of values:
  //
  data.seekg(0, data.end);  // move to the end to get length of file:
  streamoff length = data.tellg();

  streamoff pos = 0;  // first record at offset 0:
  string    value;

  while (pos < length)
  {
    data.seekg(pos, data.beg);  // move to start of record:

    for (int i = 0; i < numColumns; ++i)  // read values, one per column:
    {
      data >> value;
      cout << value << " ";
    }

    cout << endl;
    pos += recordSize;  // move offset to start of next record:
  }
}


//
// GetRecord
//
// Reads a record of data values and returns these values in a vector.
// Pass the table name, the file position (a stream offset), and the # 
// of columns per record.
//
// Example: GetRecord(�students�, 0, 5) would read the first student
// record in �students.data�.
// 
vector<string> GetRecordAll(string tablename, streamoff pos, int numColumns, vector<string> colNames)
{
  vector<string>  values;
  //
  // open the file, make sure it opened, seekg to the given position,
  // loop and input values using >>, store into vector, return vector
  //
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  //if file cannot be opened, return error
  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return values;
  }

  data.seekg(pos, data.beg);
  string value;
  for (int i = 0; i < numColumns; i++)
  {
	  data >> value;
	  values.push_back(value);
	  cout << colNames[i] << ": " << value << endl;
  }
  if(values.size() == 0)
	  cout << "Not found..." << endl;
  return values;
}

//
//GetRecordOne
//
//returns a vector of strings as well as checks if the column is indexed, and searches and displays accordingly.
//
//
vector<string> GetRecordOne(string tablename, streamoff pos, int numColumns, vector<string> colNames, int columnNumber)
{
  vector<string>  values;
  //
  // open the file, make sure it opened, seekg to the given position,
  // loop and input values using >>, store into vector, return vector
  //
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return values;
  }

  data.seekg(pos, data.beg);
  string value;
  for (int i = 0; i < columnNumber; i++)
  {
	  data >> value;
	  values.push_back(value);
  }
  if(values.size() == 0)
	  cout << "Not found..." << endl;
  cout << colNames[columnNumber-1] << ": " << value << endl;
  return values;
}

//
//displayAll
//
//takes in tablename, and the position of the said record, and displays all the columns for the values
//
//
void displayAll(string tablename, int numCols, int recordSize, streamoff pos, vector<string> colNames)
{
  //
  // open the file, make sure it opened, seekg to the given position,
  // loop and input values using >>, store into vector, return vector
  //
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return;
  }

  data.seekg(pos, data.beg);
  string value;
  for (int i = 0; i < numCols; i++)
  {
	  data >> value;
	  cout << colNames[i] << ": " << value << endl;
	  //cout << value << " ";
  }
  //cout << endl;
}


//
// LinearSearch
//
// Searches the contents of a .data file record by record; the first 
// three parameters are the tablename, record size, and # of columns.
// The 4th parameter specifies the value to search for: matchValue.
// The 5th parameter is the record column to match against --- pass
// 1 for the first column, 2 for the 2nd column, and so on.  All matches
// are exact matches.
//
// Example: LinearSearch(�students�, 82, 5, "kim", 3) would search the 
// "students.data" file for all records whose 3rd column --- lastname ---
// matches "kim".  There are 2 matches (records 3 and 6), so their file 
// positions of 164 and 410 would be returned in the vector.
// 
vector<streamoff> LinearSearch(string tablename, int recordSize, int numColumns, string matchValue, int matchColumn)
{
  vector<streamoff>  matches;

  //
  // open the file, make sure it opened, loop through record by 
  // record looking for matches.  For each match found, add file
  // position to vector.
  //
  //check if the file is good
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return matches;
  }

  //
  // Okay, read file record by record, and output each record of values:
  //
  data.seekg(0, data.end);  // move to the end to get length of file:
  streamoff length = data.tellg();

  streamoff pos = 0;  // first record at offset 0:
  string    value;

  while (pos < length)
  {
    data.seekg(pos, data.beg);  // move to start of record:

    for (int i = 1; i < matchColumn; ++i)  // read values, one per column:
    {
      data >> value;
    }
	data >> value;
	if(value == matchValue)
	{
		matches.push_back(pos);
	}
    pos += recordSize;  // move offset to start of next record:
  }
	 
  return matches;
}


//
//checkTokens
//
//takes in the query entered by the user and checks it for correctness
//
//
bool checkTokens(vector<string> colNames, vector<string> tokens)
{
	
	//case: incorrect select query
	if(tokens[0] == "select" && tokens.size() < 8)
	{
		cout << "Invalid select query, ignored..." << endl << endl;
		return false;
	}
	
	//if select,
	if(tokens[0] == "select")
	{
		//case 1: select column does not exist
		bool counter = false;
		for(unsigned int i = 0; i < colNames.size(); i++)
		{
			if(tokens[1] == colNames[i] || tokens[1] == "*")
			{
				counter = true;
				break;
			}
		}
		if(!counter)
		{
			cout << "Invalid select column, ignored..." << endl << endl;
			return false;
		}
		
		//case 2: where column doesn't exist
		counter = false;
		for(unsigned int i = 0; i < colNames.size(); i++)
		{
			if(tokens[5] == colNames[i])
				counter = true;
		}
		if(!counter)
		{
			cout << "Invalid where column, ignored..." << endl << endl;
			return false;
		}
	}
	else   //if not select
	{
		cout << "Unknown query, ignored..." << endl << endl;
		return false;
	}
	
	//case: unknown table name
		string   filename1 = tokens[3] + ".data";
	    string   filename2 = tokens[3] + ".meta";
	    ifstream data1(filename1, ios::in | ios::binary);
		ifstream data2(filename2, ios::in | ios::binary);
		

	    if (!(data1.good() && data2.good()))
	    {
			cout << "Invalid table name, ignored..."<< endl << endl;
			return false;
	    }
	return true;
}





