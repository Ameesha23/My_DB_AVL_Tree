/*main.cpp*/

//
// myDB project using AVL trees
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
#include <cassert>

#include "avl.h"
#include "util.h"

using namespace std;


//
// tokenize
//
// Given a string, breaks the string into individual tokens (words) based
// on spaces between the tokens.  Returns the tokens back in a vector.
//
// Example: "select * from students" would be tokenized into a vector
// containing 4 strings:  "select", "*", "from", "students".
//
vector<string> tokenize(string line)
{
  vector<string> tokens;
  stringstream  stream(line);
  string token;

  while (getline(stream, token, ' '))
  {
    tokens.push_back(token);
  }

  return tokens;
}

//
//readMeta
//
//function to read the meta data and return the vector containing
//the column numbers for the indexed columns
//
vector<int> readMeta(string tablename, int &recordSize, int &numColumns, vector<string> &indexNames, vector<string> &colNames)
{
  vector<int> index;
  string   filename = tablename + ".meta";
  ifstream meta(filename, ios::in | ios::binary);

  if (!meta.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return index;
  }

  meta.seekg(0, meta.beg);   //move to the beginning of the file to start getting user input
	
  //take in recordSize and numColumns
  meta >> recordSize;
  meta >> numColumns;
	 
  string data1;
  string data2;
  int columnNumber = 1;
  while(!meta.eof() && colNames.size() < 8)
  {
	  meta >> data1;
	  colNames.push_back(data1);
	  meta >> data2;
	  //if column is indexed, add columnNumber to the vector of indexed column numbers
	  if(data2 == "1")
	  {
		  index.push_back(columnNumber);
		  indexNames.push_back(data1);
	  }
	  columnNumber++;
  }
	
  return index;
}

//
//makeAVLTree
//
//makes an AVL tree based off the index value passed to the function
//
avltree<string, streamoff> makeAVLTree(string tablename, int recordSize, int numColumns, int indexCol)
{
	avltree<string, streamoff> indexedTree;
	string   filename = tablename + ".data";
	ifstream data(filename, ios::in | ios::binary);

	if (!data.good())
	{
		cout << "**Error: couldn't open data file '" << filename << "'." << endl;
		return indexedTree;
	}
	
	 //
	 // Okay, read file record by record, and output each record of values:
	 //
	 data.seekg(0, data.end);  // move to the end to get length of file:
	 streamoff length = data.tellg();

	 streamoff pos = 0;  // first record at offset 0:
	 string    value;
	 int i = 0;
	 //for every pos in the file, add record to tree
	 while (pos < length)
	 {
		data.seekg(pos, data.beg);  // move to start of record:
		streamoff curPos = pos;
		for(int i = 0; i < indexCol; i++)
		{
			data >> value;
		}
		indexedTree.insert(value, curPos);
		pos = i * recordSize;
		i++;
	 }
	cout << "  Tree size: " << indexedTree.size() << endl;
	cout << "  Tree height: " << indexedTree.height() << endl;
	
	
	return indexedTree;

}


//
//buildTrees
//
//function that takes in the index vector as a parameter 
vector<avltree<string, streamoff>*> buildTrees(string tablename, int recordSize, int numColumns, vector<int> index, vector<string> indexNames)
{	
	vector<avltree<string, streamoff>*> indexTrees;
	unsigned int i = 0;
	while(i < index.size())
	{
		int indexCol = index[i];
		cout << "Index column: " << indexNames[i] << endl;
		avltree<string, streamoff> A;
		A = makeAVLTree(tablename, recordSize, numColumns, indexCol);
		if(A.size() == 0)
			return indexTrees;
		avltree<string, streamoff> *B = new avltree<string, streamoff>;
		*B = A;
		indexTrees.push_back(B);
		i++;
		
	}
	return indexTrees;
}


//
//getAll
//
//takes in the query items passed by the user and checks if the column is indexed
//and performs linear or avlsearch based on the column indexing. 
//
bool getAll(string tablename,int recordSize, int numCols, string columnName, string key, vector<string> colNames, vector<int> indexTrees, vector<avltree<string, streamoff>*> indexedTrees)
{
	unsigned int i, k;
	for(i = 0; i < colNames.size(); i++)
	{
		if(colNames[i] == columnName)
			break;
	}
	int counter = 0;
	for(k = 0; k < indexTrees.size(); k++)
	{
		if(i+1 == indexTrees[k])
		{
			//indexed column, do an avl tree search
			streamoff *pos = indexedTrees[k]->search(key);
			if(pos == nullptr)
				return false;
			displayAll(tablename, numCols, recordSize, *pos, colNames);
			counter++;
		}
	}
	if(counter == 0)
	{
		//LinearSearch
		vector <streamoff> posMatch = LinearSearch(tablename, recordSize, numCols, key, i+1);
		vector<string> values;
		if(posMatch.size() == 0)
		{
			return false;
		}
		for(unsigned int i = 0; i < posMatch.size(); i++)
		{
			values = GetRecordAll(tablename, posMatch[i], numCols, colNames);
		}
		
	}
	return true;
}


//
//displayOne
//
//display the record values corresponding to the column number passed by the user
//
//
void displayOne(string tablename, int numCols, int recordSize, streamoff pos, vector<string> colNames, int columnNumber)
{
  string   filename = tablename + ".data";
  ifstream data(filename, ios::in | ios::binary);

  if (!data.good())
  {
    cout << "**Error: couldn't open data file '" << filename << "'." << endl;
    return;
  }

  data.seekg(pos, data.beg);
  string value;
  for (int i = 0; i < columnNumber; i++)
  {
	  data >> value;	  //cout << value << " ";
  }
  cout << colNames[columnNumber - 1] << ": " << value << endl;
  //cout << endl;
}


//
//getOne
//
//gets the record searched by the user
bool getOne(string tablename,int recordSize, int numCols,string displayColName, string columnName, string key, vector<string> colNames, vector<int> indexTrees, vector<avltree<string, streamoff>*> indexedTrees)
{
	int columnNumber;
	for(unsigned int i = 0; i < colNames.size(); i++)
	{
		if(colNames[i] == displayColName)
		{
			columnNumber = i+1;
			break;
		}
	}
	unsigned int i, k;
	for(i = 0; i < colNames.size(); i++)
	{
		if(colNames[i] == columnName)
			break;
	}
	int counter = 0;
	for(k = 0; k < indexTrees.size(); k++)
	{
		if(i+1 == indexTrees[k])
		{
			//indexed column, do an avl tree search
			streamoff *pos = indexedTrees[k]->search(key);
			if(pos == nullptr)
				return false;
			displayOne(tablename, numCols, recordSize, *pos, colNames, columnNumber);
			counter++;
		}
	}
	if(counter == 0)
	{
		//LinearSearch
		vector <streamoff> posMatch = LinearSearch(tablename, recordSize, numCols, key, i+1);
		vector<string> values;
		if(posMatch.size() == 0)
		{
			return false;
		}
		for(unsigned int i = 0; i < posMatch.size(); i++)
		{
			values = GetRecordOne(tablename, posMatch[i], numCols, colNames, columnNumber);
			//cout << "here " << values[0] << values[1] << values[2] << values[3] << values[4] << endl;
			/*for(unsigned int k = 0; k < colNames.size(); k++)
			{
				//cout << colNames[k] << endl ;
				cout << ": " << values[k] << endl;
			}*/
		}
		
	}
	return true;
}


int main()
{
  string tablename; // = "students";

  cout << "Welcome to myDB, please enter tablename> ";
  getline(cin, tablename);
	
  cout << "Reading meta-data..." << endl;
  //
  // TODO:
  //
  int recordSize;
  int numColumns;
  vector<int> index, colNums;
  vector<string> indexNames, colNames;
  index = readMeta(tablename, recordSize, numColumns, indexNames, colNames);
	
  if(index.size() == 0)
  {
	  return 0;
  }  

  cout << "Building index tree(s)..." << endl;
  //
  // TODO:
  //
  vector<avltree<string, streamoff>*> indexedTrees = buildTrees(tablename, recordSize, numColumns, index, indexNames);
  if(indexedTrees.size() == 0)
	  return 0;
	
  //
  // Main loop to input and execute queries from the user:
  //
  string query;
  
  cout << endl;
  cout << "Enter query> ";
  getline(cin, query);

  while (query != "exit")
  {
    vector<string> tokens = tokenize(query);
	  
	if(checkTokens(colNames, tokens))
	{
		//
		// TODO:
		//
		/*for(unsigned int i = 0; i < tokens.size(); i++)
		{
			cout << tokens[i] << endl;
		}*/
		if(tokens[0] == "select")
		{
			if(tokens[1] == "*")
			{
				if(!getAll(tablename, recordSize, numColumns, tokens[5], tokens[7], colNames, index, indexedTrees))
					cout << "Not found..." << endl;
			}
			else
			{
				//displayOne
				if(!getOne(tablename, recordSize, numColumns, tokens[1], tokens[5], tokens[7], colNames, index, indexedTrees))
					cout << "Not found..." << endl;

			}
		}
		cout << endl;
	}
    cout << "Enter query> ";
    getline(cin, query);
  }

  //
  // done:
  //
  return 0;
}
