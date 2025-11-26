#pragma once

#include <vector>
#include <iostream>

struct Position
{
	static Position getUnassigned();	
	int row;
	int col;
	bool operator==(const Position& other);
	bool operator!=(const Position& other);
	Position() : row(-1), col(-1) {}
	Position(int row, int col) : row(row), col(col) {}
	Position(const Position& other) : row(other.row), col(other.col) {}
};
// 输出Position的<<运算符
extern std::ostream& operator<<(std::ostream& os, Position position);

// Queen是CSP的variable，domain是variable的取值范围，variable的值
// 说明：每个Queen只能够在一列上移动，也就是说一个queen只会有一个取值
class Queen
{
public:
	int id; // Queen的id从1开始
	Queen(int id, int size);	// id为1的queen表示第一个皇后，size表示棋盘的大小，比如8*8的棋盘，size应为8
	std::vector<Position> domain;	// Queen的domian由id和puzzle的size决定，比如Queen(1, 8)的domian应为{[0-7], 0}
	Position position;	// Queen的位置，即CSP的variable的取值
	// 修改variable的值时，同时修改domain和position
	// 例如：assign({0, 0})使position变为{0, 0}，domain变为[{0, 0}]
	void assign(Position value);	
};
