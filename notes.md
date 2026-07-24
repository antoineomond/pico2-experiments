# power usage comparison between using busy wait (noop) and using wfe plus alarm
In the current master branch (dated 03.07.26), there is a bug in sleep_ms preventing the board from doing wfe due to the event flag not being cleared (https://github.com/raspberrypi/pico-sdk/issues/3052). This can be fixed by using the hardware spin lock instead of the software implementation. When using the hardware implementation, the board wfe as expected. When using the software implementation, the board loops trying to wfe and doing spin_unlock. It is not exactly noop, but it is still busy doing stuff. To use the hardware lock, the  PICO_USE_SW_SPIN_LOCKS variable in spin_lock.h must be set to 0.

## results
Results are in noop_wfe_busy_wait/results.pdf. All three groups of lines wait for 10 seconds, and another has been added (prime computation, going further than 10 seconds). The top group uses a loop where the sw spin_lock is repeatidely locked and unlocked. The second group is the prime number computation. The third group uses the timer_busy_wait_until function to actively wait a timer while doing a noop. The bottom group uses wfe to gate the cpu clocks and wait for an interrupt. 5 iterations are done and displayed. 

As expected, out of all, doing wfe uses the less power (62.53mW). It has very few dispertion compared to the other twos. It is however surprising that gating the cpu clocks results in only 62.53mW. One would assume that entering sleep mode would allow to reach power usage near the ones attained when using low power clocks. 

Using noop is also a surprising result, regarding how dispersed the results between iteration and inside a single iteration. It is surprising as it doesn't follow any pattern, while the PLL frequency doesn't change through time. Compared to the prime number computation, it is much less linear and erratic. An (uneducated) hypothesis could be that because the busy_wait has a lower cpu time percentage than prime number computation, it benefits less from the stable clock frequency at which runs the cpu, ensuring a more linear cpu usage thus power usage. On the contrary, a lower cpu time means that the power usage may vary according to the time and consumption of fetching data from memory. However, it would be strange to have such erratic and dispersed values considering a single memory address being fetched all the time. The assembly from the busy_wait shows the load instruction, always getting the same address.  

Using spin loop (initial sdk code) results in a more even power usage in and between iterations. One possibility is that the cpu usage is lower than doing only noop. It is still an even curve.

## ccl
- The power usage and its disperation can vary greatly depending on the type of instruction executed. Performing prime computation consumes less power than save/disabling interrupts then locking a spin lock. Performing prime computation consumes more than doing noops while fetching the value of a timer from memory. Performing noop while fetching the value causes a much more erratic and dispersed power traces. The latter is strange. As far as I remember, the power usage due to the matrix multiplication workload was much more consistent and less dispersed.
- Using wfe can reduce power usage compared to active wait 

# preliminary check: is the board doing wfe when using sleep_ms or not
## debugging wfe
The event flag is a single bit that is set to 1, and consumed by the wfe call. Meaning that regardless of the instruction setting the flag, the flag is either set or unchanged (e.g., printf sets the flag). It looks like the software spinlock causes the event flag to be set (https://github.com/raspberrypi/pico-sdk/issues/1812, apparently it is to respect the arm-v8 standard). In the sleep_ms implem, the event flag is never cleared. So when doing wfe at spin_lock_blocking instruction, the wfe returns immediately. However, even when using the hardware lock (setting PICO_USE_SW_SPIN_LOCKS to 0), the wfe still returns immediately. It has to do with the add_alarm_at function that triggers an IRQ with ta_force_irq.

## ccl
- the latest release at the time of the issue was 29.07.25. The new release was 03.07.26,

# comparison PLL with ROSC at same frequency
## setting PLL frequencies from 20 to 250MHz
Setting PLL to a specific frequency can be done using the set_sys_clock_khz provided by the pico-sdk. Before that, I manually deinit then reinit the PLL with a different frequency. This was a more complex process, because the system clock had to be switched from the PLL to the XOSC before turning off the PLL. Additionally, this caused issues with the frequency when frequencies higher then 180MHz had to be reached (see #2026-05-11).

Using this function, experiments were conducted for PLL frequency going up to 250MHz. This didn't have a positive impact on energy consumption (see #2026-05-11).

## setting ROSC freq to the same freqs
The pico-extras repository provides a function to find the configuration of the rosc for a target frequency. This function is limited to only one parameter and doesn't use the rest of them. Additional code were provided to use all ROSC parameters (drive range, drive strength, divider) + vreg output. This code was used to find all reachable frequencies according to these parameters. First, for 1.1V: results/freq_count/rosc_freqs.csv.

Results showed that there was no energy consumption benefits from doing so (see "results"). Therefore, additional experiments were conducted to find the ROSC frequency with the lowest VREG output. The following csv provides the maximum frequencies attainable for VREG outputs ranging from 0.85V to 1.25V: results/freq_count/max_freq_vreg_rosc/freqs.csv.

Frequencies ranging from 20 to 250MHz while being lower or equal than the associated minimum vreg output must be reached. Because the ROSC is much less accurate than the PLL, a tolerance has been set. This tolerance is set to 3%, meaning that each frequency is set to the target frequency with more or less 3%. Results are provided here: results/freq_count/max_freq_vreg_rosc/max_freq.csv. 

## results
Results for 1.1V showed that using the ROSC is counter productive and consumes in most cases the same or more than the PLL. When reaching low frequencies, it looks like the energy consumption is better when using the ROSC, but not by a lot. When going at 0.85V, results showed a much more interesting trade-off, with up to 40% less power usage at higher frequencies compared to the PLL.

When reducing the VREG output to the lowest possible for each frequency (with a 3% tolerance in frequency accuracy), ROSC results offer a better energy consumption overall (compare results/experiments_run/pll_range/vco_freq_impact.pdf with results/experiments_run/rosc_range_low_vreg/vco_freq_impact.pdf. This is especially the case for frequencies from 150MHz to 20Mhz.For these frequencies, the using the ROSC allows for a better energy consumption savings compared to using the PLL. Energy consumption savings range from 2 to 34%, corresponding to frequencies from 220MHz to 30MHz. The energy consumption saving at 150MHz is 12%: results/experiments_run/11V_PLL_lowvreg_ROSC.csv. These are the results when setting the VREG output at 1.1V when using the PLL.  

When reducing the VREG output to the lowest attainable while using the PLL, results for the ROSC become much less interesting. Above 110MHz, using the ROSC instead of the PLL increases the energy consumption and power usage. The increase is between 10% (120MHz) to 29% (220MHz). At 110MHz and below, using the ROSC decreases the energy consumption. The energy consumption savings range between 8 (110MHz) to 19% (30MHz).

One surprising result is that at 20MHz, the ROSC uses more power compared to 30MHz (from 15 to 20mW). This is not the case when using the PLL. This leads to a large increase in energy consumption when using the ROSC instead of the PLL for 20MHz (36% increase). 

# rosc freq counting
At first, the ROSC frequencies got from the execution of benchmark and the ones obtained by the freq count script were different. The issue was because, in the expe code, the PLL was used instead of the XOSC to count the frequencies, with a voltage at 0.85V. Normally, executing computing benchmarks using the PLL for this voltage results in a crash. But to count the frequency it was working. However, the results were always slightly lower than the expected frequency. It is possible that the PLL was working in a degraded way, where it may have missed lots of signal edges when counting the frequency. 

Frequencies obtained for the ROSC have very low standard deviation. For 40 iterations across two manual resets (20 iteration, reset, 20 iterations), the standard deviation was less than 0.1%. Considering our setup and in our context (board, ambient temperature, noises in the environment like wireless communication, computers or so), the ROSC provides a way to have a controllable frequency.  

The ROSC frequencies also vary according to the board itself. Doing frequency count on one board yields different result than from another one. A csv obtained from using one board showed a frequency of 155MHz for a config, while this same config on another board showed 157MHz. Need more experiments to confirm or quantify the variability of the ROSC on different boards.

# expe setup
Resetting the board using the 3V3 voltage pin is a bad idea. As specified in the forums, doing so can damage the board and make it work incorrectly. This is what happened with the two previous boards. At some point, they produced garbage values or worked at much slower paces. The correct way to reset the board is by shorting the RUN pin low (connect to ground). The expe setup evolved to add wires to short the pin low when the board should be reset. For now, it seems to have fixed the issue. 

# timer in the Pico 2
sleep_ms calls the [sleep_until](/home/aomond/research/projet_sensor_loic_2025/pico/pico-sdk/src/common/pico_time/time.c) function. In the default Pico 2 conf, this function adds an alarm and loops by acquiring and unlock a spin lock, and tries to wfe in between. The spin lock is acquired by disabling interrupts, to prevent a race condition. Also, a data memory barrier (dmb) is inserted after acquiring the lock. So sleep_ms doesn't actually do a busy wait, it actively tries to wfe until the timer is reached. 

When a timer is created, an entry in an alarm pool is added. This entry contains the target time the alarm should fires and the callback to call when the alarm fires. An alarm is thus just that entry. It doesn't seem to schedule a mecanism to trigger an event or interrupt.

## sleeping from the LPOSC 
Setting the LPOSC as reference clock impacts the system timers, which cannot produce accurate ticks due to floating, inaccurate and sub-GHz frequency. It should be possible to have a sleep_until equivalent for the LPOSC using the AON timer and interrupt generated from ALARM triggered by this timer:
- create a time instance that is not based on the system timers
    - use powman_set_timer, there is no time instance (but then how to handle interrupt service routines)
- add an alarm on the AON timer that triggers an interrupt at a specific time
- go into wfe until this alarm triggers

## 

- gp20 -> dio
- gp8 -> busy
- gp9 -> reset
- rx -> miso/sdo
* orange: clk
* white/grey: rx
* yellow: tx
* blue: cs

problematic setup and observation:
- 3V3 power supply from a converter
- connect gnds of the power supply to both waveshare sx1262 and adafruit BME680
- connect the positive wire of the power supply to the 3V3 pin of sx1262, powering the sx1262
- connect the SDO pin (MISO) from BME680 to the MISO pin of sx1262
- the BME680 is powered on while it shouldn't (the green led wired to the power input is turned on)
- checked that there is no continuity between pins in bme680 or sx1262 (except gnds)
How is it possible?
- back-powering:
    - MISO: 2.9V
    - VIN: 2.8V
    - 3V3: continuously increases, stopped at 400mV  
- there is ESD protection diodes on SCK and SDI, but not on SDO

refs:
- Grounds must be connected when two different power supplies are used: https://forum.arduino.cc/t/will-spi-work-between-two-systems-with-different-power-supplies/168680
- What exactly is green LED on BME680: https://forums.adafruit.com/viewtopic.php?t=175379 (last comment)

# extension with LoRa contrib
Study impact of configurations in the initial paper on time/energy consumption performances when doing I2C/SPI/UART.

## Benchmark
Read values from BME680 and send them via LoRa receiver.

## Protocol
- if bme and LoRa hasn't been initialised, initialise of bme and lora and put in scratch register that the devices have been initialised
- toggle start of experiment
- read bme680 reading using the custom trigger_bme680_mrsmt function
- write to the TX LoRa buffer using the sdk sx126x_write_buffer function
- set the LoRa device in TX mode using the sdk sx126x_set_tx function
- once the message is sent, the custom dio_gpio_callback callback function is called
- toggle the end of experiment when entering this callback
- reset the board

## Parameters
- Clock source/clock frequency/vreg output (initial paper parameters)
- Baud rate
- Protocol:
    - SPI
    - I2C
    - UART
- payload length to send via the LoRa

## Metrics
power usage, energie consumption (initial paper metrics)

# meeting 2026-06-04
- [x] energy consumption summary in table
- [x] hard to give ccl because parameters are mixed
- [x] gather VREG experiments and clock freq expes together?
- [x] include same frequency experiments
- [x] relevance of describing everything? Especially energy consummption? Smaller paragraphs?

# meeting 2026-05-26
- [x] comparison benchs wo noop bench noop (figures paper with figures in wo_noop/)
- [x] comparison same frequencies PLL and ROSC for different VREGS outputs: 11V_PLL_lowvreg_ROSC_bis.pdf and lowvreg_PLL_lowvreg_ROSC.pdf
- [x] target pour le papier

# comparison PLL with ROSC at same frequency
## setting PLL frequencies from 20 to 250MHz
Setting PLL to a specific frequency can be done using the set_sys_clock_khz provided by the pico-sdk. Before that, I manually deinit then reinit the PLL with a different frequency. This was a more complex process, because the system clock had to be switched from the PLL to the XOSC before turning off the PLL. Additionally, this caused issues with the frequency when frequencies higher then 180MHz had to be reached (see #2026-05-11).

Using this function, experiments were conducted for PLL frequency going up to 250MHz. This didn't have a positive impact on energy consumption (see #2026-05-11).

## setting ROSC freq to the same freqs
The pico-extras repository provides a function to find the configuration of the rosc for a target frequency. This function is limited to only one parameter and doesn't use the rest of them. Additional code were provided to use all ROSC parameters (drive range, drive strength, divider) + vreg output. This code was used to find all reachable frequencies according to these parameters. First, for 1.1V: results/freq_count/rosc_freqs.csv.

Results showed that there was no energy consumption benefits from doing so (see "results"). Therefore, additional experiments were conducted to find the ROSC frequency with the lowest VREG output. The following csv provides the maximum frequencies attainable for VREG outputs ranging from 0.85V to 1.25V: results/freq_count/max_freq_vreg_rosc/freqs.csv.

Frequencies ranging from 20 to 250MHz while being lower or equal than the associated minimum vreg output must be reached. Because the ROSC is much less accurate than the PLL, a tolerance has been set. This tolerance is set to 3%, meaning that each frequency is set to the target frequency with more or less 3%. Results are provided here: results/freq_count/max_freq_vreg_rosc/max_freq.csv. 

## results
Results for 1.1V showed that using the ROSC is counter productive and consumes in most cases the same or more than the PLL. When reaching low frequencies, it looks like the energy consumption is better when using the ROSC, but not by a lot. When going at 0.85V, results showed a much more interesting trade-off, with up to 40% less power usage at higher frequencies compared to the PLL.

When reducing the VREG output to the lowest possible for each frequency (with a 3% tolerance in frequency accuracy), ROSC results offer a better energy consumption overall (compare results/experiments_run/pll_range/vco_freq_impact.pdf with results/experiments_run/rosc_range_low_vreg/vco_freq_impact.pdf. This is especially the case for frequencies from 150MHz to 20Mhz.For these frequencies, the using the ROSC allows for a better energy consumption savings compared to using the PLL. Energy consumption savings range from 2 to 34%, corresponding to frequencies from 220MHz to 30MHz. The energy consumption saving at 150MHz is 12%: results/experiments_run/11V_PLL_lowvreg_ROSC.csv. These are the results when setting the VREG output at 1.1V when using the PLL.  

When reducing the VREG output to the lowest attainable while using the PLL, results for the ROSC become much less interesting. Above 110MHz, using the ROSC instead of the PLL increases the energy consumption and power usage. The increase is between 10% (120MHz) to 29% (220MHz). At 110MHz and below, using the ROSC decreases the energy consumption. The energy consumption savings range between 8 (110MHz) to 19% (30MHz).

One surprising result is that at 20MHz, the ROSC uses more power compared to 30MHz (from 15 to 20mW). This is not the case when using the PLL. This leads to a large increase in energy consumption when using the ROSC instead of the PLL for 20MHz (36% increase). 

# rosc freq counting
At first, the ROSC frequencies got from the execution of benchmark and the ones obtained by the freq count script were different. The issue was because, in the expe code, the PLL was used instead of the XOSC to count the frequencies, with a voltage at 0.85V. Normally, executing computing benchmarks using the PLL for this voltage results in a crash. But to count the frequency it was working. However, the results were always slightly lower than the expected frequency. It is possible that the PLL was working in a degraded way, where it may have missed lots of signal edges when counting the frequency. 

Frequencies obtained for the ROSC have very low standard deviation. For 40 iterations across two manual resets (20 iteration, reset, 20 iterations), the standard deviation was less than 0.1%. Considering our setup and in our context (board, ambient temperature, noises in the environment like wireless communication, computers or so), the ROSC provides a way to have a controllable frequency.  

The ROSC frequencies also vary according to the board itself. Doing frequency count on one board yields different result than from another one. A csv obtained from using one board showed a frequency of 155MHz for a config, while this same config on another board showed 157MHz. Need more experiments to confirm or quantify the variability of the ROSC on different boards.

# expe setup
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
==== BASE ====
