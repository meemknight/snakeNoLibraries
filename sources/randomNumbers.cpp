#include <randomNumbers.h>
#include <types.h>

#include <Windows.h>

uint64_t seed = 0;
void setRandomSeed()
{
	LARGE_INTEGER time1 = {};
	QueryPerformanceCounter(&time1);
	seed = time1.LowPart;
}

//https://en.wikipedia.org/wiki/Lehmer_random_number_generator
int getRandomNumber(int min, int max)
{

	uint64_t product = seed * 48271;
	uint32_t x = (product & 0x7fffffff) + (product >> 31);
	x = (x & 0x7fffffff) + (x >> 31);
	seed = x;

	uint32_t retVal = seed;
	return retVal % (max-min) + min;
}