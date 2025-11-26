#pragma once
#include <vector>
#include <string>
#include <set>
#include <map>
#include "Example.h"

using namespace std;

/*
 * Dataset container
 */
class Dataset
{
public:
	Dataset(string filename);			 // initialize from CSV file
	Dataset(vector<vector<string>> row); // initialize from raw rows

	vector<string> attributes;		   // list of attribute names
	vector<vector<string>> raw_values; // raw rows, each row contains attribute values (last column may be label)
	vector<Example> examples;		   // examples converted to Example objects

	vector<string> get_values(string line);		  // split a CSV line into values
	bool get_label(string value);				  // returns true if label string equals "1"
	Example build_example(vector<string> values); // convert vector<string> to Example
};
