#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include "Example.h"
#include "TreeNode.h"

using namespace std;

namespace util
{
	static string LABEL_ATTRIBUTE = "WILLWAIT";			 // name of the label attribute
	vector<string> get_attributes();					 // return the list of attribute names (size 10)
	map<string, set<string>> get_attributes_options();	 // return possible values for each attribute
	int get_attribute_index(string attribute);			 // return index of an attribute in the attribute list
	vector<int> get_labels(vector<Example> examples);	 // convert example labels to vector<int>
	double evaluate(vector<int> pred, vector<int> gold); // evaluate predictions against gold labels

	// helper printing functions
	void print_attributes(vector<string> &attributes);
	void print_example(Example &example);
	void print_examples(vector<Example> &examples);
	void print_raw_values(vector<vector<string>> raw_values);
	void print_options(map<string, set<string>> &options);
	void print_tree(TreeNode *root, int depth);
}