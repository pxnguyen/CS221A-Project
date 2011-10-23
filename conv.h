#ifndef __CONV_H_DEFINED__
#define __CONV_H_DEFINED__

unsigned long convert_cycles_to_ns(int cpu_freq, unsigned long long cpu_cycles, int num_iters) ;
unsigned long convert_cycles_to_us(int cpu_freq, unsigned long long cpu_cycles, int num_iters) ;
unsigned long convert_cycles_to_ms(int cpu_freq, unsigned long long cpu_cycles, int num_iters) ;
unsigned long convert_cycles_to_s(int cpu_freq, unsigned long long cpu_cycles, int num_iters) ;

#endif
