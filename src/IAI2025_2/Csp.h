#pragma once

#include <vector>
#include <queue>
#include "Queen.h"
#include "relation.h"


class Csp {
public:
	Csp();
	// 根据variables初始化CSP，初始化arcs和constrains
	Csp(std::vector<Queen*> variables);
	~Csp();

	std::vector<Queen*> variables;
	// variables之间的弧集合，弧被表示为一对pair
	std::queue<std::pair<Queen*, Queen*>> arcs;
	// 约束关系
	relation::relationFunc constraints;

	// 获取current Queen除expection Queen之外的邻居Queen，用于ac-3算法的使用
	std::vector<Queen*> getNeighboursExcept(Queen& current, Queen& exception);
	// 判断value是否与assignment冲突
	bool consistent(Position& value, std::vector<Queen*>& assignment);
	// 记录variables状态，当inference时variables的domian和position被修改，backtrackingSearch算法中，记录当前状态
	void record(std::vector<Position>& lastPositions, std::vector<std::vector<Position>>& lastDomains);
	// 恢复variables状态到递归失败或者失败之前的状态
	void recover(std::vector<Position>& lastPositions, std::vector<std::vector<Position>>& lastDomains);
	// 随机给variables赋值
	void randomAssign();
};
