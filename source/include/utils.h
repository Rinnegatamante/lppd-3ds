// Timer struct
struct Timer{
	uint32_t magic;
	bool isPlaying;
	uint64_t tick;
};

size_t getTotalSystemMemory();
uint64_t osGetTime();

#define baseDatas 1887436
extern int allocatedDatas; // Arbitrary default allocated datas (in reality it's probably a bit bigger)
#define maxDatas 67108864