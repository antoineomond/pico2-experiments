# 2026-05-11
- The VCO frequency has a low impact on energy consumption: results/experiments_run/vco_freq_impact/vco_freq_impact.pdf: less than 10% decrease in both cases (2.7% and 7.9%).
- Visually, it looks like the 150MHz frequency allows for the best energy consumption savings: results/experiments_run/pll_range/vco_freq_impact.pdf
- Using 1.20V and putting the PLL output frequency to 250MHz, then 240MHz, then 230MHz makes the UART output to produce garbage at 230MHz. Even when resetting the board using the bootsel, it keeps resetting itself and re-outputting garbage after few seconds.   
    - the issue was an out of bound index error on an array. The normal behavior is the board crashes and doesn't print anything. So the behavior described above is strange   
- Higher frequencies than 180MHz are reached either correctly, either divided by 2 (randomly). Increasing VREG output to 1.20 doesn't have a noticeable impact. 
- It looks like the frequency counter doesn't count or output the correct values. In results/experiments_run/pll_range_upper/vco_freq_impact.pdf it is shown that the power usage value for plots such as 250MHz or 230MHz show a power usage below 150MHz. They also show a completion time that is around twice the one of 240MHz (which seems to be printed correctly). That hints toward the fact that the frequency counter may advertise a certain frequency, while this frequency is in fact twice as low.
    - experiments showed that the PLL frequency is set once and doesn't fluctuate after having been set (20 iterations in same config with one second between each freq sampling). So it is something during the initialisation of the PLL frequency that causes the problem. There doesn't seem to be a regular pattern, except it looks like the divided frequency appears more often and each frequency appear maximum 3 times in a row. It starts from 190MHz (180MHz and below correctly output the frequency). 
    - setting using set_sys_clock_khz seems to fix the issue
- increasing PLL frequency from 150MHz to 200MHz doesn't seem to have a positive impact on energy consumption saving (check results/experiments_run/pll_range_upper/vco_freq_impact.pdf)

# 2026-05-08
- Doing noop expes in experiments where we measure energy consumption makes no sense. Because noop involves a wait, which doing varies according to processor frequency

# 1.1V to 0.8V and rebooting
- Alternating 2 experiments. The first one is default PLL configuration. The second one is LPOSC trim 0x00 VREG 0.8V. Launched these two experiments for 20 iterations. The monitoring station ceases to register results for more than 10 minutes after the 3rd iteration completed (i.e., when PLL experiment should start).
- Putting back the board in 1.1V then waiting 5 seconds before rebooting makes all 20 iteration to complete. 
- Another run: Waiting 5 seconds isn't necessary 
- Another run without setting 1.1V before rebooting confirms the necessity of putting back to 1.1V before rebooting (or at least not let it at 0.8V)

# benchmark completion time according to clock frequency 
- With PLL, going from 150MHz to 50MHz
    - benchmark completion times evolve linearly with clock frequency

# energy consumption according to benchmark size and pll clock freqs 
- clocks freqs in y
- size1: versionned
- size2: prime size:    15000 instead of 5000
-        mat mul iters:    10 instead of  200
- size3: prime size: 15000 alone
- size4: prime size: 10000 alone

## Observations
It looks like the reason why energy consumption varies may be due to the power usage variations when doing benchmarks. The prime number computation lowers the power usage compared to noop operations, while the multicore prime number computation increases the power usage compared to it. It is also the case for the matrix multiplication benchmark. The lower the power usage, the lower the variations of power usage between benchmarks. This may be the reason why the power usage variation and energy consumption varies non-linerarly between size of benchmarks.

# Multiple iterations
The standard deviation for power usage between iterations seems to stays more or less the same whether it is when unplugging/replugging manually or when rebooting from software. It varies by a few mW, especially on high clock frequencies. The deviation seems to get lower as clock frequency decreases.

# pll min f
For 0.95, deactivating subsystems makes the system to use more power for the noop, prime calculation and half of the multicore prime computation. After that, the power usage stays similar as for when subsystems aren't deactivated. 

The fact that power usage increases when subsystems are deactivated is surprising. Because the only hypothesis I currently have is that decreasing VREG output should only increase current consumption if more power are needed. Deactivating subsystems such as GPIO or clock generator should in theory leads to a decrease in the power required for the board to function. **To explain this variation, there may be a mecanism to adjust the current drawn, that is triggered at a later point during benchmark execution when subsystems are deactivated.** 

# energy consumption according to benchmark size
