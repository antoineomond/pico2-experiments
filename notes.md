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
