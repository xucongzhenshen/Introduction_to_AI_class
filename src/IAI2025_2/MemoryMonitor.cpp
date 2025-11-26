#ifdef _WIN32
#include <windows.h>
#include <psapi.h>

long getMemoryUsage()
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
    {
        return pmc.WorkingSetSize / 1024; // 转换为KB
    }
    return 0;
}
#endif

#include "MemoryMonitor.h"

MemoryMonitor::MemoryMonitor() : initial_memory(getMemoryUsage()), peak_memory(0) {}

void MemoryMonitor::reset()
{
    peak_memory = 0;
}

void MemoryMonitor::update()
{
    long current = getMemoryUsage() - initial_memory;
    if (current > peak_memory)
    {
        peak_memory = current;
    }
}