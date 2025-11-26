#pragma once
#include <vector>
#include "MemoryMonitor.h"
#include "Csp.h"

namespace search
{
	Queen *selectUnassignedVariable(Csp &csp);
	
	std::vector<Position> orderDomainValues(Queen *var, std::vector<Queen *> &assignment, Csp &csp);
	
	std::vector<Queen *> makeInference(Csp &csp, Queen *var, Position value);
	
	bool failed(std::vector<Queen *> &inferences);
	
	bool isSolution(Csp &csp, std::vector<Queen *> &solution);
	
	void refresh(std::vector<Queen *> &assignment);

	typedef std::vector<Queen *> (*searchFunc)(Csp &csp, MemoryMonitor& memoryMonitor);


	std::vector<Queen *> backtrackingSearch(Csp &csp, MemoryMonitor& memoryMonitor);

	std::vector<Queen *> backtrack(std::vector<Queen *> assignment, Csp &csp, MemoryMonitor& memoryMonitor);

	std::vector<Queen *> minConflict(Csp &csp, int maxSteps, MemoryMonitor& memoryMonitor);
	std::vector<Queen *> minConflictWrapper(Csp &csp, MemoryMonitor& memoryMonitor);

	int getConflicts(Csp &csp, Position &position);
	
	Queen *chooseConflictVariable(Csp &csp);
	
	Position getMinConflictValue(Csp &csp, Queen *var);

	std::vector<Queen *> robustMinConflict(Csp &csp, int maxSteps, MemoryMonitor& memoryMonitor);
	Position getMinConflictValueWithSimulatedAnnealing(Csp &csp, Queen *var, double temperature);
	Position selectBestFromMultiple(Csp &csp, Queen *var,
									std::vector<Position> &candidates,
									int minConflicts);
	int calculateTotalConflicts(Csp &csp);
	int evaluatePositionQuality(Csp &csp, Queen *var, Position candidate);
	std::pair<Queen *, Queen *> selectSwapVariable(Csp &csp);

	//print the solution
	void printSolution(std::vector<Queen *> &solution);
}