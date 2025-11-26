#pragma once

class MemoryMonitor
{
private:
    long peak_memory;
    long initial_memory;

public:
    MemoryMonitor();
    void reset();
    void update();
    long getPeakUsage() const { return peak_memory; }
};