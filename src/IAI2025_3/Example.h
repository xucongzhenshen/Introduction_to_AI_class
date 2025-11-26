#pragma once
#include <vector>
#include <string>

using namespace std;

class Example
{
public:
	Example();
	Example(vector<string> data, bool label);
	vector<string> data; // feature values of the example
	bool label;			 // label of the example (true/false)
};