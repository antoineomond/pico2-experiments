#include <stdio.h>
#include "experiments.h"
#include "target_configuration.h"

// Select experiments and phase to run
#define PHASE 1
//////////////////////////////////////

#define LINE_SIZE 50
extern const struct config configs[];
extern const uint nb_expes;
extern float TIME_RATE;

int main() {
	uint expe_num = watchdog_hw->scratch[1]%nb_expes;
	char buffer[LINE_SIZE];
	iteration_init(PHASE, configs[expe_num].vreg_output);
	
	uint clk_src_freq = switch_configuration_from_parameter(&configs[expe_num]);
	
	bool clock_source_lposc = configs[expe_num].clock_source == LPOSC ? true : false;
	uint8_t results = execute_benchmarks(clock_source_lposc);
	sprintf(buffer, "%d,%d,%d,%d,%b\n",expe_num,configs[expe_num].clock_source,clk_src_freq,configs[expe_num].vreg_output,results);
	
	iteration_end(PHASE, buffer);
	return 0; // Should never reach here
}
