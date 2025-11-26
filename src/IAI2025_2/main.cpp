// Assignment2.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <ctime>
#include <chrono>
#include <vector>
#include <fstream>
#include <tuple>
#include <iomanip>
#include "Queen.h"
#include "Csp.h"
#include "search.h"
#include "MemoryMonitor.h"

Csp initCsp(int size)
{
	std::vector<Queen*> variables;
	for (int i = 0; i < size; i++)
	{
		Queen* queen = new Queen(i + 1, size);
		variables.push_back(queen);
	}

	return Csp(variables);
}

void backtrackExperiment(int maxPuzzleSize)
{
	std::cout << "Backtrack Search Experiment Begin." << std::endl;
	for (int size = 1; size <= maxPuzzleSize; size++)
	{
		std::cout << "Current Size: " << size << std::endl;
		Csp csp = initCsp(size);
		MemoryMonitor memoryMonitor;
		std::vector<Queen*> solution = search::backtrackingSearch(csp, memoryMonitor);
		search::printSolution(solution);
	}
}
void minConflictExperiment(int puzzleSize, int iteration, int maxStep)
{
	std::cout << "MinConflict Search Experiment Begin." << std::endl;
	for (int it = 0; it < iteration; it++)
	{
		std::cout << "Iteration: " << it << " begin" << std::endl;
		Csp csp = initCsp(puzzleSize);
		csp.randomAssign();

		MemoryMonitor memoryMonitor;
		std::vector<Queen*> solution = search::minConflict(csp, maxStep, memoryMonitor);
		search::printSolution(solution);
	}
}

void searchTest(search::searchFunc searchFunc, std::string prefix = "")
{
	std::cout << "\n=== Search Test Begin ===" << std::endl;

	if (prefix.empty())
	{
		prefix = "search";
	}

	// 创建输出文件
	std::string outputFile = prefix + "_detailed_results.csv";
	std::ofstream ofile(outputFile);
	if (!ofile.is_open())
	{
		std::cerr << "Error: Cannot open output file: " << outputFile << std::endl;
		return;
	}

	// 详细的CSV文件头
	ofile << "Size,TestID,Success,Time(ms),Memory(KB),SolutionFound" << std::endl;

	// 统计信息文件
	std::string summaryFile = prefix + "_summary.csv";
	std::ofstream summaryStream(summaryFile);
	if (!summaryStream.is_open())
	{
		std::cerr << "Error: Cannot open summary file: " << summaryFile << std::endl;
		ofile.close();
		return;
	}

	summaryStream << "Size,SuccessRate(%),AvgTime(ms),MaxTime(ms),AvgMemory(KB),MaxMemory(KB),TotalTests, TotalTime(ms)" << std::endl;

	std::vector<long long> total_durations;
	const int TESTS_PER_SIZE = 50;

	MemoryMonitor memoryMonitor;

	for (int size = 4; size <= 16; size += 4)
	{
		std::cout << "\n--- Testing Size: " << size << " ---" << std::endl;

		int successCount = 0;
		long long totalTime = 0;
		long long maxTime = 0;
		long long totalMemory = 0;
		long long maxMemory = 0;

		std::vector<std::tuple<bool, long long, long long>> results;

		for (int testID = 0; testID < TESTS_PER_SIZE; testID++)
		{
			Csp csp = initCsp(size);

			// 内存监控重置
			memoryMonitor.reset();

			// 计时开始
			auto start = std::chrono::steady_clock::now();

			// 执行搜索
			std::vector<Queen *> solution = searchFunc(csp, memoryMonitor);

			// 计时结束
			auto end = std::chrono::steady_clock::now();

			// 计算时间和内存使用
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			long long memory_used = memoryMonitor.getPeakUsage();
			long long memory_kb = memory_used; // 转换为KB

			bool success = !search::failed(solution);

			// 更新统计
			if (success)
				successCount++;
			totalTime += duration;
			totalMemory += memory_kb;
			maxTime = std::max(maxTime, duration);
			maxMemory = std::max(maxMemory, memory_kb);

			results.push_back(std::make_tuple(success, duration, memory_kb));

			// 输出单个测试结果
			std::string status = success ? "SUCCESS" : "FAILED";
			std::cout << "  Test " << std::setw(2) << testID + 1 << ": " << status
					  << " - Time: " << std::setw(5) << duration << " ms"
					  << ", Memory: " << std::setw(6) << memory_kb << " KB"
					  << std::endl;

			// 写入详细结果
			ofile << size << ","
				  << testID + 1 << ","
				  << (success ? "Yes" : "No") << ","
				  << duration << ","
				  << memory_kb << ","
				  << (success ? "Yes" : "No") << std::endl;
		}

		// 计算平均值
		double avgTime = static_cast<double>(totalTime) / TESTS_PER_SIZE;
		double avgMemory = static_cast<double>(totalMemory) / TESTS_PER_SIZE;
		double successRate = (static_cast<double>(successCount) / TESTS_PER_SIZE) * 100.0;

		// 输出本组统计信息
		std::cout << "✓ Size " << size << " Summary:" << std::endl;
		std::cout << "  Success Rate: " << successCount << "/" << TESTS_PER_SIZE
				  << " (" << std::fixed << std::setprecision(1) << successRate << "%)" << std::endl;
		std::cout << "  Average Time: " << std::fixed << std::setprecision(2) << avgTime << " ms" << std::endl;
		std::cout << "  Maximum Time: " << maxTime << " ms" << std::endl;
		std::cout << "  Average Memory: " << std::fixed << std::setprecision(2) << avgMemory << " KB" << std::endl;
		std::cout << "  Maximum Memory: " << maxMemory << " KB" << std::endl;
		std::cout << "  Total Time for Size " << size << ": " << totalTime << " ms" << std::endl;

		// 写入统计文件
		summaryStream << size << ","
					  << std::fixed << std::setprecision(1) << successRate << ","
					  << std::fixed << std::setprecision(2) << avgTime << ","
					  << maxTime << ","
					  << std::fixed << std::setprecision(2) << avgMemory << ","
					  << maxMemory << ","
					  << TESTS_PER_SIZE << ","
					  << totalTime << std::endl;

		total_durations.push_back(totalTime);
	}

	// 关闭文件
	ofile.close();
	summaryStream.close();

	std::cout << "\n=== Search Test Completed ===" << std::endl;
	std::cout << "Results saved to:" << std::endl;
	std::cout << "  - Detailed results: " << outputFile << std::endl;
	std::cout << "  - Summary statistics: " << summaryFile << std::endl;
}

int main(int argc, const char* argv[])
{
	std::srand((unsigned int)std::time(NULL)); // Generate a random seed

	//backtrackExperiment(16);
	//minConflictExperiment(8, 100, 200);
	//searchTest(search::backtrackingSearch, "data/backtrack");
	searchTest(search::minConflictWrapper, "data/minconflict");

	system("pause");
	return 0;
}
