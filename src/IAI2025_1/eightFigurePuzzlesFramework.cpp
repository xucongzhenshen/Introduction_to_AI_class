#include <time.h>
#include <windows.h>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <vector>
#include "eightFigurePuzzles.h"
using namespace std;

//用于记录当前状态是否被访问过
map<int, int> visited;

//深度有限搜索，用于限制深度
#define MAX_DEPTH 20

//广度优先搜索
int* binaryFirstSearch(PUZZLE_NODE initialNode, PUZZLE_NODE objPuzzleNode) {
    //result[0] 1:correct;0:wrong
    //result[1] 步数 steps
    int* result = new int[2]{0, 0};

    /*
		请在该位置完成广度优先搜索函数
	*/
    cout << "初始节点状态：" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << " " << initialNode.puzzle[i * 3 + 0].puzzleId << "  " << initialNode.puzzle[i * 3 + 1].puzzleId << "  " << initialNode.puzzle[i * 3 + 2].puzzleId << endl;
    }
    cout << endl;

    PUZZLE_NODE puzzleNode = initialNode;
    queue<PUZZLE_NODE> puzzleNodeQueue;
    puzzleNode.depth = 0;
    puzzleNodeQueue.push(puzzleNode);
    while (!puzzleNodeQueue.empty())
    {
        PUZZLE_NODE currentPuzzleNode = puzzleNodeQueue.front();
        if (checkObject(currentPuzzleNode, objPuzzleNode))
        {
            for (int i = 0; i < currentPuzzleNode.precedeActionList.size(); i++)
            {
                outputAction(currentPuzzleNode.precedeActionList[i], i + 1);
            }
            cout << "找到正确结果:" << endl;
            for (int i = 0; i < 3; i++)
            {
                cout << " " << currentPuzzleNode.puzzle[i * 3 + 0].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 1].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 2].puzzleId << endl;
            }
            cout << endl;
            result[0] = 1;
            result[1] = currentPuzzleNode.depth;
            return result;
        }
        else
        {
            visited[visitedNum(currentPuzzleNode)] = 1;
            if (currentPuzzleNode.nextActionList.size() == 0)
            {
                currentPuzzleNode = updatePuzzleNodeActionList(currentPuzzleNode);
            }
            puzzleNodeQueue.pop();
            for (int i = 0; i < currentPuzzleNode.nextActionList.size(); i++)
            {
                PUZZLE_NODE nextPuzzleNode = moveToPuzzleNode(currentPuzzleNode.nextActionList[i], currentPuzzleNode);
                if (visited[visitedNum(nextPuzzleNode)] == 1)
                {
                    continue;
                }
                if (!currentPuzzleNode.precedeActionList.empty())
                {
                    for (int actionIndex = 0; actionIndex < currentPuzzleNode.precedeActionList.size(); actionIndex++)
                    {
                        nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.precedeActionList[actionIndex]);
                    }
                }
                nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.nextActionList[i]);
                nextPuzzleNode.depth = currentPuzzleNode.depth + 1;
                puzzleNodeQueue.push(nextPuzzleNode);
            }
        }
    }

    return result;
}

//深度有限搜索
int* depthFirstSearch(PUZZLE_NODE initialNode, PUZZLE_NODE objPuzzleNode) {
    //result[0] 1:correct;0:wrong
    //result[1] 步数 steps
    int* result = new int[2]{0, 0};
    /*
		请在该位置完成深度有限搜索，最大深度限度为25
	*/
    cout << "初始节点状态：" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << " " << initialNode.puzzle[i * 3 + 0].puzzleId << "  " << initialNode.puzzle[i * 3 + 1].puzzleId << "  " << initialNode.puzzle[i * 3 + 2].puzzleId << endl;
    }
    cout << endl;

    PUZZLE_NODE puzzleNode = initialNode;
    stack<PUZZLE_NODE> puzzleNodeStack;
    puzzleNode.depth = 0;
    puzzleNodeStack.push(puzzleNode);
    while (!puzzleNodeStack.empty())
    {
        PUZZLE_NODE currentPuzzleNode = puzzleNodeStack.top();
        if (checkObject(currentPuzzleNode, objPuzzleNode) || currentPuzzleNode.depth >= MAX_DEPTH)
        {
            for (int i = 0; i < currentPuzzleNode.precedeActionList.size(); i++)
            {
                outputAction(currentPuzzleNode.precedeActionList[i], i + 1);
            }
            cout << "找到正确结果:" << endl;
            for (int i = 0; i < 3; i++)
            {
                cout << " " << currentPuzzleNode.puzzle[i * 3 + 0].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 1].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 2].puzzleId << endl;
            }
            result[0] = 1;
            result[1] = currentPuzzleNode.depth;
            return result;
        }
        else
        {
            visited[visitedNum(currentPuzzleNode)] = 1;
            if (currentPuzzleNode.nextActionList.size() == 0)
            {
                currentPuzzleNode = updatePuzzleNodeActionList(currentPuzzleNode);
            }
            puzzleNodeStack.pop();
            for (int i = 0; i < currentPuzzleNode.nextActionList.size(); i++)
            {
                PUZZLE_NODE nextPuzzleNode = moveToPuzzleNode(currentPuzzleNode.nextActionList[i], currentPuzzleNode);
                if (visited[visitedNum(nextPuzzleNode)] == 1)
                {
                    continue;
                }
                if (!currentPuzzleNode.precedeActionList.empty())
                {
                    for (int actionIndex = 0; actionIndex < currentPuzzleNode.precedeActionList.size(); actionIndex++)
                    {
                        nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.precedeActionList[actionIndex]);
                    }
                }
                nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.nextActionList[i]);
                nextPuzzleNode.depth = currentPuzzleNode.depth + 1;
                puzzleNodeStack.push(nextPuzzleNode);
            }
        }
    }

    if (checkObject(initialNode, objPuzzleNode) && initialNode.depth < MAX_DEPTH) {
        result[0] = 1;
    } else {
        result[0] = 0;
    }

    return result;
}

//计算不正确位置的数码个数
int IncorrectNum(PUZZLE_NODE currentNode, PUZZLE_NODE objNode)
{
    int incorrectNum = 0;
    for (int i = 0; i < 9; i++)
    {
        if (currentNode.puzzle[i].puzzleId != objNode.puzzle[i].puzzleId)
        {
            incorrectNum++;
        }
    }
    return incorrectNum;
}

//启发式搜索1
int *heuristicSearchInformedByIncorrectNum(PUZZLE_NODE initialNode, PUZZLE_NODE objPuzzleNode)
{
    //result[0] 1:correct;0:wrong
    //result[1] 步数 steps
    int* result = new int[2]{0, 0};

    /*
		请在该位置完成启发式搜索，启发式函数使用不正确位置的数码个数
	*/
    cout << "初始节点状态：" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << " " << initialNode.puzzle[i * 3 + 0].puzzleId << "  " << initialNode.puzzle[i * 3 + 1].puzzleId << "  " << initialNode.puzzle[i * 3 + 2].puzzleId << endl;
    }
    cout << endl;

    map<int, int> visited;         // 记录状态和对应的g值
    // 创建比较器实例，传入目标节点和启发式函数
    auto InN = [objPuzzleNode](PUZZLE_NODE a, PUZZLE_NODE b)
    { return IncorrectNum(a, objPuzzleNode) + a.depth > IncorrectNum(b, objPuzzleNode) + b.depth; };

    // 创建优先队列，使用比较器
    priority_queue<PUZZLE_NODE, vector<PUZZLE_NODE>, decltype(InN)> openList(InN);

    PUZZLE_NODE puzzleNode = initialNode;
    puzzleNode.depth = 0;
    openList.push(puzzleNode);

    while (!openList.empty()) {
        PUZZLE_NODE currentPuzzleNode = openList.top();
        openList.pop();

        if (checkObject(currentPuzzleNode, objPuzzleNode)) {
            for (int i = 0; i < currentPuzzleNode.precedeActionList.size(); i++) {
                outputAction(currentPuzzleNode.precedeActionList[i], i + 1);
            }
            cout << "找到正确结果:" << endl;
            for (int i = 0; i < 3; i++) {
                cout << " " << currentPuzzleNode.puzzle[i * 3 + 0].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 1].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 2].puzzleId << endl;
            }
            cout << endl;

            result[0] = 1;
            result[1] = currentPuzzleNode.depth;
            return result;
        } else {
            visited[visitedNum(currentPuzzleNode)] = 1;
            if (currentPuzzleNode.nextActionList.size() == 0) {
                currentPuzzleNode = updatePuzzleNodeActionList(currentPuzzleNode);
            }
            for (int i = 0; i < currentPuzzleNode.nextActionList.size(); i++) {
                PUZZLE_NODE nextPuzzleNode = moveToPuzzleNode(currentPuzzleNode.nextActionList[i], currentPuzzleNode);
                if (visited[visitedNum(nextPuzzleNode)] == 1) {
                    continue;
                }
                if (!currentPuzzleNode.precedeActionList.empty()) {
                    for (int actionIndex = 0; actionIndex < currentPuzzleNode.precedeActionList.size(); actionIndex++) {
                        nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.precedeActionList[actionIndex]);
                    }
                }
                nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.nextActionList[i]);
                nextPuzzleNode.depth = currentPuzzleNode.depth + 1;
                openList.push(nextPuzzleNode);
            }
        }
    }

    if (checkObject(initialNode, objPuzzleNode)) {
        result[0] = 1;
    } else {
        result[0] = 0;
    }

    return result;
}

// 计算曼哈顿距离
int ManhattonDis(PUZZLE_NODE currentNode, PUZZLE_NODE objNode)
{
    int manhattonDis = 0;
    for (int i = 0; i < 9; i++)
    {
        int currentID = currentNode.puzzle[i].puzzleId;
        int currentX = currentNode.puzzle[i].xPosition;
        int currentY = currentNode.puzzle[i].yPosition;
        int objX = objNode.puzzle[currentID].xPosition;
        int objY = objNode.puzzle[currentID].yPosition;
        manhattonDis += abs(currentX - objX) + abs(currentY - objY);
    }
    return manhattonDis;
}

//启发式搜索2
int* heuristicSearchInformedByManhattonDis(PUZZLE_NODE initialNode, PUZZLE_NODE objPuzzleNode) {
    //result[0] 1:correct;0:wrong
    //result[1] 步数 steps
    int* result = new int[2]{0, 0};
    /*
		请在该位置完成启发式搜索，启发式函数采用到目标位置的曼哈顿距离
	*/
    cout << "初始节点状态：" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << " " << initialNode.puzzle[i * 3 + 0].puzzleId << "  " << initialNode.puzzle[i * 3 + 1].puzzleId << "  " << initialNode.puzzle[i * 3 + 2].puzzleId << endl;
    }
    cout << endl;

    map<int, int> visited;         // 记录状态和对应的g值
    // 创建比较器实例，传入目标节点和启发式函数
    auto MhD = [objPuzzleNode](PUZZLE_NODE a, PUZZLE_NODE b)
    { return ManhattonDis(a, objPuzzleNode) + a.depth > ManhattonDis(b, objPuzzleNode) + b.depth; };

    // 创建优先队列，使用比较器
    priority_queue<PUZZLE_NODE, vector<PUZZLE_NODE>, decltype(MhD)> openList(MhD);

    PUZZLE_NODE puzzleNode = initialNode;
    puzzleNode.depth = 0;
    openList.push(puzzleNode);
    while (!openList.empty()) {
        PUZZLE_NODE currentPuzzleNode = openList.top();
        openList.pop();

        if (checkObject(currentPuzzleNode, objPuzzleNode)) {
            for (int i = 0; i < currentPuzzleNode.precedeActionList.size(); i++) {
                outputAction(currentPuzzleNode.precedeActionList[i], i + 1);
            }
            cout << "找到正确结果:" << endl;
            for (int i = 0; i < 3; i++) {
                cout << " " << currentPuzzleNode.puzzle[i * 3 + 0].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 1].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 2].puzzleId << endl;
            }
            cout << endl;

            result[0] = 1;
            result[1] = currentPuzzleNode.depth;
            return result;
        } else {
            visited[visitedNum(currentPuzzleNode)] = 1;
            if (currentPuzzleNode.nextActionList.size() == 0)
            {
                currentPuzzleNode = updatePuzzleNodeActionList(currentPuzzleNode);
            }
            for (int i = 0; i < currentPuzzleNode.nextActionList.size(); i++) {
                PUZZLE_NODE nextPuzzleNode = moveToPuzzleNode(currentPuzzleNode.nextActionList[i], currentPuzzleNode);
                if (visited[visitedNum(nextPuzzleNode)] == 1) {
                    continue;
                }
                if (!currentPuzzleNode.precedeActionList.empty()) {
                    for (int actionIndex = 0; actionIndex < currentPuzzleNode.precedeActionList.size(); actionIndex++) {
                        nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.precedeActionList[actionIndex]);
                    }
                }
                nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.nextActionList[i]);
                nextPuzzleNode.depth = currentPuzzleNode.depth + 1;
                openList.push(nextPuzzleNode);
            }
        }
    }

    if (checkObject(initialNode, objPuzzleNode)) {
        result[0] = 1;
    } else {
        result[0] = 0;
    }

    return result;
}

//广度优先搜索
int* binaryFirstSearchDemo(PUZZLE_NODE initialNode, PUZZLE_NODE objPuzzleNode) {
    //result[0] 1:correct;0:wrong
    //result[1] 步数 steps
    int* result = new int[2]{0, 0};

    cout << "初始节点状态：" << endl;
    for (int i = 0; i < 3; i++) {
        cout << " " << initialNode.puzzle[i * 3 + 0].puzzleId << "  " << initialNode.puzzle[i * 3 + 1].puzzleId << "  " << initialNode.puzzle[i * 3 + 2].puzzleId << endl;
    }
    cout << endl;
    /*
		请在该位置完成广度优先搜索函数
	*/
    PUZZLE_NODE puzzleNode = initialNode;
    queue<PUZZLE_NODE> puzzleNodeQueue;
    puzzleNode.depth = 0;
    int depth = 0;
    puzzleNodeQueue.push(puzzleNode);
    while (puzzleNodeQueue.size()) {
        PUZZLE_NODE currentPuzzleNode = puzzleNodeQueue.front();
        if (checkObject(currentPuzzleNode, objPuzzleNode)) {
            for (int i = 0; i < currentPuzzleNode.precedeActionList.size(); i++) {
                outputAction(currentPuzzleNode.precedeActionList[i], i + 1);
            }
            cout << "找到正确结果:" << endl;
            for (int i = 0; i < 3; i++) {
                cout << " " << currentPuzzleNode.puzzle[i * 3 + 0].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 1].puzzleId << "  " << currentPuzzleNode.puzzle[i * 3 + 2].puzzleId << endl;
            }
            cout << endl;

            result[0] = 1;
            result[1] = currentPuzzleNode.depth;
            return result;
        } else {
            visited[visitedNum(currentPuzzleNode)] = 1;
            if (currentPuzzleNode.nextActionList.size() == 0) {
                currentPuzzleNode = updatePuzzleNodeActionList(currentPuzzleNode);
            }
            puzzleNodeQueue.pop();
            for (int i = 0; i < currentPuzzleNode.nextActionList.size(); i++) {
                PUZZLE_NODE nextPuzzleNode = moveToPuzzleNode(currentPuzzleNode.nextActionList[i], currentPuzzleNode);
                if (!currentPuzzleNode.precedeActionList.empty()) {
                    for (int actionIndex = 0; actionIndex < currentPuzzleNode.precedeActionList.size(); actionIndex++) {
                        nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.precedeActionList[actionIndex]);
                    }
                }
                nextPuzzleNode.precedeActionList.push_back(currentPuzzleNode.nextActionList[i]);
                if (visited[visitedNum(nextPuzzleNode)] == 1) {
                    continue;
                }
                nextPuzzleNode.depth = currentPuzzleNode.depth + 1;
                puzzleNodeQueue.push(nextPuzzleNode);
            }
        }
    }
    return result;
}

int main() {
    PUZZLE_NODE objPuzzleNode;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            objPuzzleNode.puzzle[i * 3 + j].puzzleId = i * 3 + j;
            objPuzzleNode.puzzle[i * 3 + j].xPosition = i;
            objPuzzleNode.puzzle[i * 3 + j].yPosition = j;
        }
    }
    objPuzzleNode = updatePuzzleNodeActionList(objPuzzleNode);

    int setup = 0;
    while (true) {
        visited.clear();

        cout << "请选择搜索方式(-1:退出; 0:演示;1:广度优先搜索;2:深度优先搜索;3:启发式搜索(错误数);4:启发式搜索(曼哈顿距离)):" << endl;
        cin >> setup;
        if(setup == -1) return 0;
        int backwardSteps;
        cout << "请输入回退步数(5~20):" << endl;
        cin >> backwardSteps;
        while (backwardSteps < 5 || backwardSteps > 20) {
            cout << "回退步数必须在5到20之间，请重新输入:" << endl;
            cin >> backwardSteps;
        }

        PUZZLE_NODE initialNode = initialPuzzleNode(backwardSteps);

        int* result;
        if (setup == 1) {
            result = binaryFirstSearch(initialNode, objPuzzleNode);
        } else if (setup == 2) {
            result = depthFirstSearch(initialNode, objPuzzleNode);
        } else if (setup == 3) {
            result = heuristicSearchInformedByIncorrectNum(initialNode, objPuzzleNode);
        } else if (setup == 4) {
            result = heuristicSearchInformedByManhattonDis(initialNode, objPuzzleNode);
        } else if (setup == 0) {
            cout << "请选择演示方式" << endl;
            result = binaryFirstSearchDemo(initialNode, objPuzzleNode);
        } else {
            cout << "无效的搜索方式选择" << endl;
            return 0;
        }

        if (result[0] == 1) {
            cout << "The result is correct, steps are " << result[1] << endl;
        } 
        else {
            cout << "The result is wrong" << endl;
        }
        delete [] result;
    }
    return 0;
}
