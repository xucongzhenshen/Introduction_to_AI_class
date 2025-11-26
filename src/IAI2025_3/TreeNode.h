#pragma once
#include <string>
#include <set>
#include <map>

using namespace std;

class TreeNode
{
public:
    TreeNode(bool value);       // construct a leaf node; attribute set to label attribute, value stores the label
    TreeNode(string attribute); // construct an internal node with given attribute
    string attribute;
    bool value;
    map<string, TreeNode *> options; // map of child pointers keyed by attribute value
};
