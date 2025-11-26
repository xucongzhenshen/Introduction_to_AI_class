#include <iostream>
#include <cmath>
#include "DecisionTree.h"
#include "Dataset.h"
#include "util.h"

using namespace std;

int main(int argc, char* argv[])
{
    std::cerr << ">>> IAI2025_3 starts, argc=" << argc << std::endl;
    
    Dataset trainset("./dataset/train.csv");
    // Dataset testset("./dataset/test.csv");
    DecisionTree dt(trainset);
    // print training examples and the learned tree
    util::print_examples(trainset.examples);
    util::print_tree(dt.root, 0);

    // Run classification on the training set (as a quick self-test)
    vector<int> classify_res = dt.classify(trainset.raw_values);
    vector<int> gold_res = util::get_labels(trainset.examples);
    double acc = util::evaluate(classify_res, gold_res);
    std::cout << "Accuracy on training set: " << acc << std::endl;

    // keep console open when run interactively
    system("pause");
}
