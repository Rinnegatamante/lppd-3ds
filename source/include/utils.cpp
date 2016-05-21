#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <windows.h>
#include <psapi.h>
#include <sys/time.h>
#include <math.h>
#include <stdint.h>

size_t getTotalSystemMemory(){
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return pmc.PrivateUsage;
}

uint64_t osGetTime(){
	struct timeval tick;
	gettimeofday(&tick, NULL);
	return floor(tick.tv_sec * 1000 + ((float)(tick.tv_usec / 1000)));
}