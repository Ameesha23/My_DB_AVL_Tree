/*util.h*/

//
// Utility functions for myDB project
//
// Ameesha Saxena
// U. of Illinois, Chicago
// CS 251: Fall 2019
// Project #04
//

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

void EchoData(string tablename, int recordSize, int numColumns);

vector<string> GetRecordAll(string tablename, streamoff pos, int numColumns, vector<string> colNames);

vector<string> GetRecordOne(string tablename, streamoff pos, int numColumns, vector<string> colNames, int columnNumber);

vector<streamoff> LinearSearch(string tablename, int recordSize, int numColumns, string matchValue, int matchColumn);

void displayAll(string tablename, int numCols, int recordSize, streamoff pos, vector<string> colNames);

bool checkTokens( vector<string> colNames, vector<string> tokens);
