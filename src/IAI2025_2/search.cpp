#include "search.h"
#include "inference.h"
#include <iostream>
#include <algorithm>
#include<random>


// 目前采用了最简单的选择方式：按照variables的顺序选择。
Queen* search::selectUnassignedVariable(Csp& csp)
{
	for (Queen* q : csp.variables)
	{
		if (q->position == Position::getUnassigned())
		{
			return q;
		}
	}

	return NULL;
}

// 最简单的顺序：按照variables本身domain的顺序。
std::vector<Position> search::orderDomainValues(Queen* var, std::vector<Queen*>& assignment, Csp& csp)
{
	return var->domain;
}

/*
 * Inferencing can eliminate the domain of variables according to current assignment.
 * This function return inferences which can be added to assignment.
 * Inferences means the variables whose domain size has been eliminated to 1 after inferencing.
*/
std::vector<Queen*> search::makeInference(Csp& csp, Queen* var, Position value)
{
	std::vector<Queen*> result;

	// 记录状态以便在推理失败时恢复
	std::vector<Position> lastPositions;
	std::vector<std::vector<Position>> lastDomains;
	csp.record(lastPositions, lastDomains);

	// 运行AC-3算法
	if (!inference::ac3(csp)) {
		csp.recover(lastPositions, lastDomains);
		return std::vector<Queen*>({ NULL }); // 推理失败
	}

	// 收集域被缩小到1的变量
	for (Queen* q : csp.variables) {
		if (q->domain.empty()) {
			csp.recover(lastPositions, lastDomains);
			return std::vector<Queen*>({ NULL }); // 有空域，失败
		}
		if (q->domain.size() == 1 && q->position == Position::getUnassigned()) {
			q->assign(q->domain[0]);
			result.push_back(q);
		}
	}

	return result;
}
// an inferences which contains only one null pointer indicates failure.
bool search::failed(std::vector<Queen*>& inferences)
{
	if (inferences.size() == 1)
	{
		if (inferences[0] == NULL) return true;
	}

	return false;
}

bool search::isSolution(Csp& csp, std::vector<Queen*>& solution)
{
	bool result = true;

	for (Queen* q : solution)
	{
		if (getConflicts(csp, q->position) > 0) result = false;
	}

	return result;
}

// remove assigned value and inferences from assignment
void search::refresh(std::vector<Queen*>& assignment)
{
	auto it = assignment.begin();
	while (it != assignment.end())
	{
		Queen* current = *it;
		if (current->position == Position::getUnassigned()) it = assignment.erase(it);
		else it++;
	}
}

std::vector<Queen*> search::backtrackingSearch(Csp& csp)
{
	return backtrack(std::vector<Queen*>(), csp);
}

std::vector<Queen*> search::backtrack(std::vector<Queen*> assignment, Csp& csp)
{
	/*
	 * TODO
	 * Algorithm (Reference: Figure 6.5):
	 function BACKTRACK(assignment, csp) returns a solution, or failure
		if assignment is complete then return assignment (use this condition: assignment.size() == csp.variables.size())
		var<-SELECT-UNASSIGNED-VARIABLE(csp)
		for each value in ORDER-DOMAIN-VALUES(var, assignment, csp) do
			record csp state # csp.recode() require two variables, you need to create two local variables to store the state
			if value is consistent with assignment then
				assign value to var	# use var->assign(value)
				add var to assignment
				inferences<-INFERENCE(csp, var, value)	# use makeInference function here
				if inferences != failure then
					add inferences to assignment
					result<-BACKTRACK(assignment, csp)
					if result != failure then
						return result
			recover csp state (csp.recover)
			remove {var = value} and inferences from assignment # use refresh(assignment)
		return failure
	 */
	 // 如果赋值完成，返回解
	if (assignment.size() == csp.variables.size()) {
		return assignment;
	}

	// 选择未赋值变量
	Queen* var = selectUnassignedVariable(csp);
	if (var == NULL) {
		return std::vector<Queen*>({ NULL }); // 失败
	}

	// 按顺序尝试每个可能的值
	for (Position value : orderDomainValues(var, assignment, csp)) {
		// 记录CSP状态
		std::vector<Position> lastPositions;
		std::vector<std::vector<Position>> lastDomains;
		csp.record(lastPositions, lastDomains);

		// 检查值是否与当前赋值一致
		if (csp.consistent(value, assignment)) {
			// 赋值
			var->assign(value);
			assignment.push_back(var);

			// 进行推理
			std::vector<Queen*> inferences = makeInference(csp, var, value);

			// 如果推理成功
			if (!failed(inferences)) {
				// 添加推理结果到赋值
				for (Queen* q : inferences) {
					assignment.push_back(q);
				}

				// 递归搜索
				std::vector<Queen*> result = backtrack(assignment, csp);
				if (!failed(result)) {
					return result; // 找到解
				}
			}

			// 回溯：恢复CSP状态
			csp.recover(lastPositions, lastDomains);
		}
		else {
			// 恢复CSP状态（如果没进入赋值分支）
			csp.recover(lastPositions, lastDomains);
		}

		// 移除当前赋值和推理结果
		refresh(assignment);
	}
	
	return std::vector<Queen*>({NULL});
}

std::vector<Queen*> search::minConflict(Csp& csp, int maxSteps)
{
	/*
	 * TODO
	 * Algorithm (Reference: Figure 6.8):
	 function MIN-CONFLICTS(csp,max steps) returns a solution or failure
		inputs: csp, a constraint satisfaction problem
				max steps, the number of steps allowed before giving up
		current<-an initial complete assignment for csp	
		for i = 1 to max steps do
			if current is a solution for csp then # use isSolution
				print how many steps used here
				return current 
			var <- a randomly chosen conflicted variable from csp.VARIABLES # use chooseConflictVariable
			value <- the value v for var that minimizes CONFLICTS(var, v, current , csp) # use getMinConflictValue
			set var =value in current	# use var->position = value
		return failure
	 */
	int cnt = 0;
	Queen* prev = nullptr;
	Position prev_pos(-1, -1);
	 // 当前赋值已经在wrapper中随机初始化
	for (int i = 0; i < maxSteps; i++) {
		
		// 检查当前是否为解
		std::vector<Queen*> current = csp.variables;
		if (isSolution(csp, current)) {
			std::cout << "Solution found in " << i << " steps" << std::endl;
			return current;
		}

		// 随机选择有冲突的变量
		Queen* var = chooseConflictVariable(csp);
		if (var == NULL) {
			continue; // 没有冲突变量，继续
		}
		if (var == prev)var = csp.variables[rand() % csp.variables.size()];
		// 选择使冲突最小的值
		Position newValue = getMinConflictValue(csp, var);
		while(newValue == var->position){
			newValue = Position(rand() % csp.variables.size(), var->position.col);
		}
		var->assign(newValue);

		/*std::pair<Queen*, Queen*> swapPair = selectSwapVariable(csp);
		if (swapPair.first != NULL && swapPair.second != NULL) {
			Position& pos1 = swapPair.first->position;
			Position& pos2 = swapPair.second->position;
			std::swap(pos1.col, pos2.col);
		}*/
	}

	return std::vector<Queen*>({NULL});
}

std::vector<Queen*> search::minConflictWrapper(Csp& csp)
{
	csp.randomAssign();
	return robustMinConflict(csp, 200);
}

int search::getConflicts(Csp& csp, Position& position)
{
	/*
	* TODO
	* 得到一个position在当前棋盘上的冲突数量
	* 注意：与position在同一列的queen的冲突不应该计算
	* 样例：
	*	0 1 0 0
		1 0 0 0
		0 0 1 0
		0 0 0 1
	* Position{0, 0}的冲突数应该为3，因为它与{0, 1},{2, 2},{3, 3}冲突
	* Position{1, 0}的冲突数量应该为1，因为它与{0, 1}冲突
	*/
	int conflictCount = 0;

	for (Queen* q : csp.variables) {
		// 跳过未赋值的皇后和同一列的皇后（根据题目要求）
		if (q->position == Position::getUnassigned() ||
			q->position.col == position.col) {
			continue;
		}

		// 检查是否冲突（同一行或对角线）
		if (relation::conflict(position, q->position)) {
			conflictCount++;
		}
	}
	
	return conflictCount;
}

Queen* search::chooseConflictVariable(Csp& csp)
{
	/*
	* TODO
	* 返回一个目前赋值的冲突数大于0的variable
	* 注意：冲突数大于0的variable可能有多个，需要随机选择
	* 样例：
	*	0 1 0 0
		1 0 0 0
		0 0 1 0
		0 0 0 1
	* Queen1-4的冲突数都大于0，随机选择一个作为该函数的返回结果
	*/
	float lambda = 0.7f;

	std::vector<std::pair<Queen*, int>> conflictedVariables;

	// 收集所有有冲突的变量
	for (Queen* q : csp.variables) {
		if (getConflicts(csp, q->position) > 0) {
			conflictedVariables.push_back(std::make_pair(q, getConflicts(csp, q->position)));
		}
	}
	// 按冲突数排序，冲突数多的在前面
	std::sort(conflictedVariables.begin(), conflictedVariables.end(),
		[](const std::pair<Queen*, int>& a, const std::pair<Queen*, int>& b) {
			return a.second > b.second;
		});

	// 如果没有冲突变量，返回NULL
	if (conflictedVariables.empty()) {
		return NULL;
	}
	else {
		if (rand() / (float)RAND_MAX > lambda) {
			return conflictedVariables[rand() % conflictedVariables.size()].first;
		}
		else {
			std::vector<Queen*> topVariables;
			for(auto& pair : conflictedVariables){
				if(pair.second == conflictedVariables[0].second){
					topVariables.push_back(pair.first);
				}
			}
			return topVariables[rand() % topVariables.size()];
		}
	}
}

Position search::getMinConflictValue(Csp& csp, Queen* var)
{
	/*
	* TODO
	* 返回var的domian中，可以使冲突数最小的值
	* 注意：使冲突数最小的值可能有多个，需要随机选择，如果不随机选择问题可能会陷入局部稳定点并且该稳定点不是解
	* 样例：
	*	1 1 0 0
		0 0 0 0
		0 0 1 0
		0 0 0 1
	* Queen1所在的位置的冲突数为3，它的domain为{[0-3], 0}。{1, 0},{2, 0},{3, 0}的冲突数都为1。
	* 需要从中随机选取一个作为返回值。
	*/
	// 收集冲突信息
	std::vector<std::pair<Position, int>> conflictInfo;

	for (Position candidate : var->domain) {
		int conflicts = getConflicts(csp, candidate);
		conflictInfo.push_back({ candidate, conflicts });
	}

	// 找到最小冲突数
	int minConflicts = INT_MAX;
	for (const auto& info : conflictInfo) {
		if (info.second < minConflicts) {
			minConflicts = info.second;
		}
	}

	// 收集所有最小冲突的位置
	std::vector<Position> bestPositions;
	for (const auto& info : conflictInfo) {
		if (info.second == minConflicts) {
			bestPositions.push_back(info.first);
		}
	}

	// 如果没有找到（理论上不应该发生），返回当前位置
	if (bestPositions.empty()) {
		return var->position;
	}

	// 如果只有一个最佳选择，直接返回
	if (bestPositions.size() == 1) {
		return bestPositions[0];
	}

	// 如果有多个最佳选择，使用更智能的选择策略
	return selectBestFromMultiple(csp, var, bestPositions, minConflicts);
}

Position search::selectBestFromMultiple(Csp& csp, Queen* var,
	std::vector<Position>& candidates,
	int minConflicts)
{
	// 策略1: 优先选择能减少最多"潜在冲突"的位置
	std::vector<std::pair<Position, int>> candidateScores;

	for (Position candidate : candidates) {
		int score = evaluatePositionQuality(csp, var, candidate);
		candidateScores.push_back({ candidate, score });
	}

	// 找到最高分
	int maxScore = INT_MIN;
	for (const auto& pair : candidateScores) {
		if (pair.second > maxScore) {
			maxScore = pair.second;
		}
	}

	// 收集所有最高分的候选
	std::vector<Position> bestCandidates;
	for (const auto& pair : candidateScores) {
		if (pair.second == maxScore) {
			bestCandidates.push_back(pair.first);
		}
	}

	// 在最高分候选中随机选择
	int randomIndex = rand() % bestCandidates.size();
	return bestCandidates[randomIndex];
}

int search::evaluatePositionQuality(Csp& csp, Queen* var, Position candidate)
{
	int score = 0;

	// 计算移动到这个位置后，对其他皇后的影响

	// 临时保存当前状态
	Position originalPosition = var->position;

	// 模拟移动
	var->position = candidate;

	// 评估1: 计算这次移动能解决多少现有冲突
	int originalConflicts = getConflicts(csp, originalPosition);
	int newConflicts = getConflicts(csp, candidate);
	int conflictReduction = originalConflicts - newConflicts;
	score += conflictReduction * 10; // 冲突减少的权重

	// 评估2: 检查这个位置是否会让其他皇后有更多选择
	for (Queen* other : csp.variables) {
		if (other == var || other->position == Position::getUnassigned()) {
			continue;
		}

		// 检查其他皇后在这个新布局下的冲突数变化
		int otherOriginalConflicts = getConflicts(csp, other->position);

		// 简单评估：如果其他皇后的冲突减少，这个位置更好
		// 注意：这里我们只做粗略估计，不实际移动其他皇后
		if (otherOriginalConflicts > 0) {
			// 检查这个新位置是否与高冲突皇后在同一行/对角线
			if (!relation::conflict(candidate, other->position)) {
				score += 2; // 不与其他高冲突皇后冲突，加分
			}
		}
	}

	// 恢复原始位置
	var->position = originalPosition;

	return score;
}

std::pair<Queen*, Queen*> search::selectSwapVariable(Csp& csp) {
	auto SovConNum = [&csp](Queen* q1, Queen* q2) {
		
		Position pos1 = q1->position;
		Position pos2 = q2->position;
		std::swap(pos1.col, pos2.col);
		int after = getConflicts(csp, q1->position) + getConflicts(csp, q2->position);
		std::swap(pos1.col, pos2.col);
		int prior = getConflicts(csp, pos1) + getConflicts(csp, pos2);
		return after - prior;
		};
	auto cmp = [&SovConNum](std::pair<Queen*, Queen*> p1, std::pair<Queen*, Queen*> p2) {
		return SovConNum(p1.first, p1.second) < SovConNum(p2.first, p2.second);
		};
	std::priority_queue<std::pair<Queen*, Queen*>, std::vector<std::pair<Queen*, Queen*> >, decltype(cmp)> swapCandidates(cmp);

	for (size_t i = 0; i < csp.variables.size(); i++) {
		for (size_t j = i + 1; j < csp.variables.size(); j++) {
			Queen* q1 = csp.variables[i];
			Queen* q2 = csp.variables[j];
			if (q1->position.col != q2->position.col) {
				swapCandidates.push(std::make_pair(q1, q2));
			}
		}
	}
	if (swapCandidates.empty()) {
		return std::make_pair<Queen*, Queen*>(NULL, NULL);
	}

	float alpha = 0.7f;
	if( rand() / (float)RAND_MAX > alpha && swapCandidates.size() >1){
		std::vector<std::pair<Queen*, Queen*> > candidates;
		while (swapCandidates.size() >1) {
			candidates.push_back(swapCandidates.top());
			swapCandidates.pop();
		}
		int randomIndex = rand() % candidates.size();
		return candidates[randomIndex];
	}
	return swapCandidates.top();
}

void search::printSolution(std::vector<Queen*>& solution)
{
	if (search::failed(solution))
	{
		std::cout << "No Valid Solution!" << std::endl;
		return;
	}

	int size = (int)solution.size();
	std::vector<std::vector<bool>> grid(size, std::vector<bool>(size, false));

	for (Queen* queen : solution)
	{
		grid[queen->position.row][queen->position.col] = true;
	}

	for (int row = 0; row < size; row++)
	{
		for (int col = 0; col < size; col++)
		{
			std::cout << grid[row][col] << " ";
		}
		std::cout << "\n";
	}
}

std::vector<Queen*> search::robustMinConflict(Csp& csp, int maxSteps)
{
	double temperature = 25.0; // 初始温度
	const double coolingRate = 0.97;

	int count = 0;
	Queen* lastVar = NULL;
	for (int step = 0; step < maxSteps; step++) {
		if (isSolution(csp, csp.variables)) {
			std::cout << "Solution found in " << step << " steps" << std::endl;
			return csp.variables;
		}


		Queen* var = chooseConflictVariable(csp);
		if (var == NULL || count >=2) {
			// 如果没有冲突变量但还不是解，或5次是同一个皇后，随机扰动一个皇后
			int randomIndex = rand() % csp.variables.size();
			var = csp.variables[randomIndex];
		}
		if(var == lastVar){
			count++;
		}
		else{
			count = 0;
			lastVar = var;
		}

		// 使用模拟退火策略选择新位置
		Position newValue = getMinConflictValueWithSimulatedAnnealing(csp, var, temperature);
		if (newValue == var->position) {
			newValue = var->domain[rand() % var->domain.size()];
		}
		var->position = newValue;

		// 降温
		temperature *= coolingRate;

		// 输出进度
		if (step % 20 == 0) {
			std::cout << "Step " << step << ", temperature: " << temperature
				<< ", conflicts: " << calculateTotalConflicts(csp) << std::endl;
		}
	}

	return std::vector<Queen*>({ NULL });
}

Position search::getMinConflictValueWithSimulatedAnnealing(Csp& csp, Queen* var, double temperature)
{
	int currentConflicts = getConflicts(csp, var->position);
	std::vector<std::pair<Position, int>> candidates;

	for (Position pos : var->domain) {
		int conflicts = getConflicts(csp, pos);
		candidates.push_back({ pos, conflicts });
	}

	// 按冲突数排序
	std::sort(candidates.begin(), candidates.end(),
		[](const auto& a, const auto& b) { return a.second < b.second; });

	int bestConflict = candidates[0].second;

	// 如果最佳选择比当前位置好，直接选择
	if (bestConflict < currentConflicts) {
		std::vector<Position> bestCandidates;
		for (const auto& pair : candidates) {
			if (pair.second == bestConflict) {
				bestCandidates.push_back(pair.first);
			}
		}
		int randomIndex = rand() % bestCandidates.size();
		return bestCandidates[randomIndex];
	}

	// 模拟退火：以一定概率接受劣质移动
	double acceptProbability = exp(-(bestConflict - currentConflicts) / temperature);
	if ((double)rand() / RAND_MAX < acceptProbability) {
		// 接受劣质移动
		std::vector<Position> bestCandidates;
		for (const auto& pair : candidates) {
			if (pair.second == bestConflict) {
				bestCandidates.push_back(pair.first);
			}
		}
		int randomIndex = rand() % bestCandidates.size();
		return bestCandidates[randomIndex];
	}
	else {
		// 拒绝移动，保持当前位置（但这样会陷入停滞）
		// 所以要强制一些移动

		if(!candidates.empty()){
			// 选择冲突数不是最差的几个位置之一
			int midIndex = candidates.size() / 3 + 1;
			int startIndex = rand() % midIndex;
			int selectedIndex = startIndex + (rand() % (candidates.size() - startIndex));
			return candidates[selectedIndex].first;
		}
		else {
			return var->position;
		}
		
	}
}

int search::calculateTotalConflicts(Csp& csp)
{
	int totalConflicts = 0;
	for (Queen* q : csp.variables) {
		totalConflicts += getConflicts(csp, q->position);
	}
	return totalConflicts;
}