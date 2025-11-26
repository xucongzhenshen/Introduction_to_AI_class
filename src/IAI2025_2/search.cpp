#include "search.h"
#include "inference.h"
#include "MemoryMonitor.h"
#include <iostream>
#include <algorithm>
#include<random>


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

	
	std::vector<Position> lastPositions;
	std::vector<std::vector<Position>> lastDomains;
	csp.record(lastPositions, lastDomains);

	if (!inference::ac3(csp)) {
		csp.recover(lastPositions, lastDomains);
		return std::vector<Queen*>({ NULL }); 
	}

	
	for (Queen* q : csp.variables) {
		if (q->domain.empty()) {
			csp.recover(lastPositions, lastDomains);
			return std::vector<Queen*>({ NULL }); 
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

std::vector<Queen*> search::backtrackingSearch(Csp& csp, MemoryMonitor& memoryMonitor)
{
	return backtrack(std::vector<Queen*>(), csp, memoryMonitor);
}

std::vector<Queen*> search::backtrack(std::vector<Queen*> assignment, Csp& csp, MemoryMonitor& memoryMonitor)
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
	memoryMonitor.update();
	// Check if assignment is complete
	if (assignment.size() == csp.variables.size()) {
		std::cout << "Assignment complete." << std::endl;
		return assignment;
	}

	// Choose an unassigned variable
	Queen* var = selectUnassignedVariable(csp);
	if (var == NULL) {
		return std::vector<Queen*>({ NULL }); // Failure
	}

	for (Position value : orderDomainValues(var, assignment, csp)) {
		memoryMonitor.update();
		std::vector<Position> lastPositions;
		std::vector<std::vector<Position>> lastDomains;
		csp.record(lastPositions, lastDomains);

		if (csp.consistent(value, assignment)) {
			var->assign(value);
			assignment.push_back(var);

			std::vector<Queen*> inferences = makeInference(csp, var, value);

			if (!failed(inferences)) {
				for (Queen* q : inferences) {
					assignment.push_back(q);
				}

				std::vector<Queen*> result = backtrack(assignment, csp, memoryMonitor);
				if (!failed(result)) {
					return result; 
				}
			}

			csp.recover(lastPositions, lastDomains);
		}
		else {
			csp.recover(lastPositions, lastDomains);
		}
		refresh(assignment);
	}
	
	return std::vector<Queen*>({NULL});
}

std::vector<Queen*> search::minConflict(Csp& csp, int maxSteps, MemoryMonitor& memoryMonitor)
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
	for (int i = 0; i < maxSteps; i++) {
		memoryMonitor.update();
		std::vector<Queen*> current = csp.variables;
		if (isSolution(csp, current)) {
			std::cout << "Solution found in " << i << " steps" << std::endl;
			return current;
		}

		Queen* var = chooseConflictVariable(csp);
		if (var == NULL) {
			continue;
		}
		if (var == prev)var = csp.variables[rand() % csp.variables.size()];
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

std::vector<Queen*> search::minConflictWrapper(Csp& csp, MemoryMonitor& memoryMonitor)
{
	csp.randomAssign();
	return robustMinConflict(csp, 200, memoryMonitor);
}

int search::getConflicts(Csp& csp, Position& position)
{
	int conflictCount = 0;

	for (Queen* q : csp.variables) {
		if (q->position == Position::getUnassigned() ||
			q->position.col == position.col) {
			continue;
		}

		if (relation::conflict(position, q->position)) {
			conflictCount++;
		}
	}
	
	return conflictCount;
}

Queen* search::chooseConflictVariable(Csp& csp)
{
	float lambda = 0.7f;

	std::vector<std::pair<Queen*, int>> conflictedVariables;

	for (Queen* q : csp.variables) {
		if (getConflicts(csp, q->position) > 0) {
			conflictedVariables.push_back(std::make_pair(q, getConflicts(csp, q->position)));
		}
	}
	std::sort(conflictedVariables.begin(), conflictedVariables.end(),
		[](const std::pair<Queen*, int>& a, const std::pair<Queen*, int>& b) {
			return a.second > b.second;
		});

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
	std::vector<std::pair<Position, int>> conflictInfo;

	for (Position candidate : var->domain) {
		int conflicts = getConflicts(csp, candidate);
		conflictInfo.push_back({ candidate, conflicts });
	}

	int minConflicts = INT_MAX;
	for (const auto& info : conflictInfo) {
		if (info.second < minConflicts) {
			minConflicts = info.second;
		}
	}

	std::vector<Position> bestPositions;
	for (const auto& info : conflictInfo) {
		if (info.second == minConflicts) {
			bestPositions.push_back(info.first);
		}
	}

	if (bestPositions.empty()) {
		return var->position;
	}

	if (bestPositions.size() == 1) {
		return bestPositions[0];
	}

	return selectBestFromMultiple(csp, var, bestPositions, minConflicts);
}

Position search::selectBestFromMultiple(Csp& csp, Queen* var,
	std::vector<Position>& candidates,
	int minConflicts)
{
	std::vector<std::pair<Position, int>> candidateScores;

	for (Position candidate : candidates) {
		int score = evaluatePositionQuality(csp, var, candidate);
		candidateScores.push_back({ candidate, score });
	}

	// Step 2: Find the highest score
	int maxScore = INT_MIN;
	for (const auto& pair : candidateScores) {
		if (pair.second > maxScore) {
			maxScore = pair.second;
		}
	}

	// Step 3: Find the highest score positions
	std::vector<Position> bestCandidates;
	for (const auto& pair : candidateScores) {
		if (pair.second == maxScore) {
			bestCandidates.push_back(pair.first);
		}
	}

	// Step 4: From the highest score positions, randomly select one
	int randomIndex = rand() % bestCandidates.size();
	return bestCandidates[randomIndex];
}

int search::evaluatePositionQuality(Csp& csp, Queen* var, Position candidate)
{
	int score = 0;

	// Step 1: Evaluate the quality of the candidate position

	// Save the original state
	Position originalPosition = var->position;

	// Assign the candidate value
	var->position = candidate;

	// Step 1: Calculate the number of conflicts for the original and candidate positions
	int originalConflicts = getConflicts(csp, originalPosition);
	int newConflicts = getConflicts(csp, candidate);
	int conflictReduction = originalConflicts - newConflicts;
	score += conflictReduction * 10; // Conflict reduction weight
	// Step 2: Evaluate the impact on other variables' conflicts
	for (Queen* other : csp.variables) {
		if (other == var || other->position == Position::getUnassigned()) {
			continue;
		}

		// Step 2: Evaluate the impact on other variables' conflicts
		int otherOriginalConflicts = getConflicts(csp, other->position);

		// Step 2: Evaluate the impact on other variables' conflicts
		// Note: Step 2 only considers variables that currently have conflicts
		if (otherOriginalConflicts > 0) {
			// Step 2: Evaluate the impact on other variables' conflicts
			if (!relation::conflict(candidate, other->position)) {
				score += 2; // Step 2: Evaluate the impact on other variables' conflicts
			}
		}
	}

	// Restore the original position
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

std::vector<Queen*> search::robustMinConflict(Csp& csp, int maxSteps, MemoryMonitor& memoryMonitor)
{
	double temperature = 25.0; 
	const double coolingRate = 0.97;

	int count = 0;
	Queen* lastVar = NULL;
	for (int step = 0; step < maxSteps; step++) {
		memoryMonitor.update();

		if (isSolution(csp, csp.variables)) {
			std::cout << "Solution found in " << step << " steps" << std::endl;
			return csp.variables;
		}


		Queen* var = chooseConflictVariable(csp);
		if (var == NULL || count >=2) {
			// If the chosen variable is NULL or the same variable has been chosen 2 times in a row, randomly select a variable
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

		// Choose a new position using simulated annealing
		Position newValue = getMinConflictValueWithSimulatedAnnealing(csp, var, temperature);
		if (newValue == var->position) {
			newValue = var->domain[rand() % var->domain.size()];
		}
		var->position = newValue;

		// Cooling
		temperature *= coolingRate;

		// Print current state
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

	// Step 3: Sort candidate positions by their conflict values
	std::sort(candidates.begin(), candidates.end(),
		[](const auto& a, const auto& b) { return a.second < b.second; });

	int bestConflict = candidates[0].second;

	// Step 4: If there is a position with fewer conflicts, choose one of the positions with the least conflicts
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

	// Step 5: Calculate the acceptance probability for simulated annealing
	double acceptProbability = exp(-(bestConflict - currentConflicts) / temperature);
	if ((double)rand() / RAND_MAX < acceptProbability) {
		// Accept worse solution
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
		// Reject worse solution
		// Need to explore some intermediate positions

		if(!candidates.empty()){
			// Choose a position with a medium conflict value
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