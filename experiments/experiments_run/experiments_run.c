#include <stdint.h>
#include <stdio.h>
#include "experiments.h"
#include "target_configuration.h"

// Select parameters
#define PHASE 1
#define BENCHMARKS_TO_RUN 0b111110
#define PRINT_CONFS_CSV 0
struct bench_sizes bench_sizes[] = {
	{.noop = 10, .prime = 5000, .prime_lposc = 200, .mat_mul_iters = 10, .mat_mul_iters_lposc = 1},
	//{.noop = 20, .prime = 5000, .prime_lposc = 200, .mat_mul_iters = 10, .mat_mul_iters_lposc = 1},
	//{.noop = 10, .prime = 10000, .prime_lposc = 200, .mat_mul_iters = 10, .mat_mul_iters_lposc = 1},
	//{.noop = 10, .prime = 5000, .prime_lposc = 200, .mat_mul_iters = 100, .mat_mul_iters_lposc = 1},
	//{.noop = 10, .prime = 25000, .prime_lposc = 200, .mat_mul_iters = 1000, .mat_mul_iters_lposc = 1}
};
//////////////////////////////////////

#define LINE_SIZE 70
extern struct config configs[];
extern const uint nb_expes;
extern float TIME_RATE;

int main() {
	uint expe_num = watchdog_hw->scratch[1]%nb_expes;
	char buffer[LINE_SIZE];
	iteration_init(PHASE, configs[expe_num].vreg_output);
	
	uint clk_src_freq = switch_configuration_from_parameter(&configs[expe_num]);
	
	if(!PRINT_CONFS_CSV) {
		bool clock_source_lposc = configs[expe_num].clock_source == LPOSC ? true : false;
		for (int b_num = 0; b_num < sizeof(bench_sizes)/sizeof(bench_sizes[0]); b_num++) {
			uint8_t results = execute_benchmarks(clock_source_lposc, BENCHMARKS_TO_RUN, bench_sizes[b_num]);
			sprintf(buffer, "%d,%d,%d,%d,%b\n",expe_num,configs[expe_num].clock_source,clk_src_freq,configs[expe_num].vreg_output,results);
		}
	}
	else {
		if(watchdog_hw->scratch[1] >= nb_expes)
			return 0;
		char buffer_csv[5000];
		buffer_csv[0] = '\0'; // Reboot may not clear the ram  
		for (int b_num = 0; b_num < sizeof(bench_sizes)/sizeof(bench_sizes[0]); b_num++) {
			if(expe_num == 0 && b_num == 0) {
				sprintf(buffer_csv, "clock_source,pll_vco_freq,pll_div1,pll_div2,rosc_div,rosc_range,rosc_drive_freqa,rosc_drive_freqb,lposc_trim,vreg_output,set_as_ref,benchmark_name,benchmark_size,b_num,clock_freq\n");
			}
			print_configurations_csv(buffer_csv, &configs[expe_num], nb_expes, BENCHMARKS_TO_RUN, bench_sizes[b_num], b_num, configs[expe_num].clock_source == LPOSC, clk_src_freq);
		}
		iteration_end(PHASE, buffer_csv);
	}
	
	iteration_end(PHASE, buffer);
	return 0; // Should never reach here
}
