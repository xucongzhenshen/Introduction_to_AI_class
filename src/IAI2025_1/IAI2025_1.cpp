// 八数码问题

#include <iostream>
#include <vector>

using namespace std;

class Solver{
    private:
        vector<vector<int>> board;
        vector<vector<int>> goal = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 0}
        };
        pair<int, int> zero_pos; // 0的位置
        int g; // 当前步数
        int h; // 估价函数值
        int f; // f = g + h
    public:
        Solver(const vector<vector<int>>& initial_board) : board(initial_board), g(0)
        {
            // 找到0的位置
            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (board[i][j] == 0) {
                        zero_pos = {i, j};
                        return;
                    }
                }
            }
        }
        
        // 打印当前棋盘状态
        void print_board() {
            for (const auto& row : board) {
                for (const auto& val : row) {
                    cout << val << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
        
        // 检查是否达到目标状态
        bool is_goal() {
            return board == goal;
        }
        void rotate(int x1, int y1, int x2, int y2) {
            swap(board[x1][y1], board[x2][y2]);
            zero_pos = {x2, y2};
        }
        bool is_clockwise(int x1, int y1, int x2, int y2) {
            return (x2 == x1 + 1 && y2 == y1) || (x2 == x1 && y2 == y1 - 1);
        }
        //将p-p+2的数及空格移到右下角
        void move_to_bottom_right(int p) {
            // 计算当前0的位置
            int x = zero_pos.first;
            int y = zero_pos.second;
            // 计算目标位置
            int target_x = (p + 2) / 3;
            int target_y = (p + 2) % 3;
            // 将0移动到目标位置
            while (x < target_x) {
                rotate(x, y, x + 1, y);
                ++x;
                print_board();
            }
            while (y < target_y) {
                rotate(x, y, x, y + 1);
                ++y;
                print_board();
            }
            while (x > target_x) {
                rotate(x, y, x - 1, y);
                --x;
                print_board();
            }
            while (y > target_y) {
                rotate(x, y, x, y - 1);
                --y;
                print_board();
            }
        }

};

int main() {
    vector<vector<int>> initial_board = {
        {1, 2, 3},
        {4, 0, 6},
        {7, 5, 8}
    };
    Solver solver(initial_board);
    return 0;
}

        