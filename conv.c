#include "conv.h"

unsigned long convert_cycles_to_ns(int cpu_freq, unsigned long long cpu_cycles, int num_iters)
{
	unsigned long long mytime = (cpu_cycles * 1000) / (cpu_freq * num_iters);
	return mytime;	
}

unsigned long convert_cycles_to_us(int cpu_freq, unsigned long long cpu_cycles, int num_iters)
{
	unsigned long long mytime = (cpu_cycles) / (cpu_freq * num_iters);
	return mytime;	
}

unsigned long convert_cycles_to_ms(int cpu_freq, unsigned long long cpu_cycles, int num_iters)
{
	unsigned long long mytime = (cpu_cycles) / (cpu_freq * num_iters * 1000);
	return mytime;	
}

unsigned long convert_cycles_to_s(int cpu_freq, unsigned long long cpu_cycles, int num_iters)
{
	unsigned long long mytime = (cpu_cycles) / (cpu_freq * num_iters * 1000 * 1000);
	return mytime;	
}
