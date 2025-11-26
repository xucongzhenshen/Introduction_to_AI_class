#pragma once
#include <string>
#include <vector>
#include "Example.h"
#include "TreeNode.h"
#include "Dataset.h"

using namespace std;

class DecisionTree
{
public:
	DecisionTree(Dataset trainset); // initialize decision tree from training dataset

	TreeNode *root; // root of the learned decision tree

	// Entropy and information gain calculations
	double entropy(double p1, double p2);
	double entropy_binary(double p);
	double entropy_remain(string attribute, vector<Example> examples);
	double info_gain(string attribute, vector<Example> examples);
	// choose the attribute with largest information gain
	string importance(vector<string> attributes, vector<Example> examples);
	// Implements Figure 18.5: DECISION-TREE-LEARNING
	TreeNode *learn(vector<Example> examples, vector<string> attributes, vector<Example> parent_examples);
	// classification routines
	int classify_rec(Example &example, TreeNode *root);
	vector<int> classify(vector<vector<string>> test_raw_values);

private:
	TreeNode *plurality_value(vector<Example> examples);								// return a leaf node with plurality label
	vector<Example> get_examples(vector<Example> examples, string attr, string option); // filter examples by attribute value
	int get_positive_count(vector<Example> &examples);									// count positive labels
	bool have_same_class(vector<Example> examples);										// check if all examples share same label
	vector<string> remove_attribute(vector<string> attributes, string attribute);		// remove attribute from list
};
