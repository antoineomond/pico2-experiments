# comparison PLL with ROSC at same frequency
Setting PLL to a specific frequency can be done using the set_sys_clock_khz provided by the pico-sdk. Before that, I manually deinit then reinit the PLL with a different frequency. This was a more complex process, because the system clock had to be switched from the PLL to the XOSC before turning off the PLL. Additionally, this caused issues with the frequency when frequencies higher then 180MHz had to be reached (see #2026-05-11).

Using this function, experiments were conducted for PLL frequency going up to 250MHz. This didn't have a positive impact on energy consumption (see #2026-05-11).

Experiments were conducted using the ROSC instead of the PLL. The idea is that experiments are conducted using the ROSC and PLL for the same set of frequencies. First, from 20 to 150MHz, then from 150 to 250MHz, with a step of 10MHz. To do so, the ROSC must be configured to reach a specific frequency. The pico-extras repository provides a function to find the configuration of the rosc for a target frequency. This function is limited to only one parameter and doesn't use the rest of them.

To search for frequency matching the studied range, an exhaustive search was made and results of the search were stored in a csv. Using this csv, configurations providing frequencies close to the studied range could be extracted. Using the default voltage (1.1V), the frequencies could be obtained between 20 to 150MHz. Using the lowest voltage, 0.85V, frequencies could be obtained up to 110MHz. Additional search need to be done to reach 150MHz and above.  

Frequencies obtained for the ROSC have very low standard deviation. For 40 iterations across two manual resets (20 iteration, reset, 20 iterations), the standard deviation was less than 0.1%. Considering our setup and in our context (board, ambient temperature, noises in the environment like wireless communication, computers or so), the ROSC provides a way to have a controllable frequency.  

Results for 1.1V showed that using the ROSC is counter productive and consumes in most cases the same or more than the PLL. When reaching low frequencies, it looks like the energy consumption is better when using the ROSC, but not by a lot. When going at 0.85V, results showed a much more interesting trade-off, with up to 40% less power usage at higher frequencies compared to the PLL. 

At first, the ROSC frequencies got from the execution of benchmark and the ones obtained by the freq count script were different. The issue was because, in the expe code, the PLL was used instead of the XOSC to count the frequencies, with a voltage at 0.85V. Normally, executing computing benchmarks using the PLL for this voltage results in a crash. But to count the frequency it was working. However, the results were always slightly lower than the expected frequency. It is possible that the PLL was working in a degraded way, where it may have missed lots of signal edges when counting the frequency. 

Resetting the board using the 3V3 voltage pin is a bad idea. As specified in the forums, doing so can damage the board and make it work incorrectly. This is what happened with the two previous boards. At some point, they produced garbage values or worked at much slower paces. The correct way to reset the board is by shorting the RUN pin low (connect to ground). The expe setup evolved to add wires to short the pin low when the board should be reset. For now, it seems to have fixed the issue. 

# 2026-05-11
- The VCO frequency has a low impact on energy consumption: results/experiments_run/vco_freq_impact/vco_freq_impact.pdf: less than 10% decrease in both cases (2.7% and 7.9%).
- Visually, it looks like the 150MHz frequency allows for the best energy consumption savings: results/experiments_run/pll_range/vco_freq_impact.pdf
- Using 1.20V and putting the PLL output frequency to 250MHz, then 240MHz, then 230MHz makes the UART output to produce garbage at 230MHz. Even when resetting the board using the bootsel, it keeps resetting itself and re-outputting garbage after few seconds.   
    - Higher frequencies than 180MHz are reached either correctly, either divided by 2 (randomly). Increasing VREG output to 1.20 doesn't have a noticeable impact. 
    - the issue was an out of bound index error on an array. The normal behavior is the board crashes and doesn't print anything. So the behavior described above is strange   
- It looks like the frequency counter doesn't count or output the correct values. In results/experiments_run/pll_range_upper/vco_freq_impact.pdf it is shown that the power usage value for plots such as 250MHz or 230MHz show a power usage below 150MHz. They also show a completion time that is around twice the one of 240MHz (which seems to be printed correctly). That hints toward the fact that the frequency counter may advertise a certain frequency, while this frequency is in fact twice as low.
    - experiments showed that the PLL frequency is set once and doesn't fluctuate after having been set (20 iterations in same config with one second between each freq sampling). So it is something during the initialisation of the PLL frequency that causes the problem. There doesn't seem to be a regular pattern, except it looks like the divided frequency appears more often and each frequency appear maximum 3 times in a row. It starts from 190MHz (180MHz and below correctly output the frequency). 
    - setting using set_sys_clock_khz seems to fix the issue
- Increasing PLL frequency from 150MHz to 200MHz doesn't seem to have a positive impact on energy consumption saving (check results/experiments_run/pll_range_upper/vco_freq_impact.pdf). The energy consumption values stays stable accross all configurations

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
