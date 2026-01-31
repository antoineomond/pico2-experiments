import sys
import time
import board
import adafruit_ina228
import pigpio
from statistics import mean, stdev, median

DEADLINE_ITERATION = 300
EXPE_PIN = 27
expe_num = 0
done = 0
started = 0
init = 0 # Necessary when expe_pin is initially at 1 and reset to 0
s = 0
deadline = time.time()
start_time = time.time()
timing_samples = []

if(len(sys.argv) < 1 or not isinstance(sys.argv[0], int)):
    print("Precise number of experiments (int)")
    exit()
nb_expes = int(sys.argv[0])
live = int(sys.argv[1]) if len(sys.argv) > 1 else 0

def next_expe(user_gpio, level, tick):
    global init
    global expe_num
    global started
    global done
    global s
    global deadline
    if(level == 1):
        init = 1
        started = 1
        deadline = time.time()
        s = tick
    if(init == 1 and level == 0):
        t = tick-s
        # tick is 32 bit timer that wraps around from 4294967295 to 0 if overflow, the following condition handles that case 
        if(tick < s):
            t += 4294967295
        timing_samples.append(t)
        started = 0
        expe_num += 1
        if(expe_num >= nb_expes):
             done = 1
        print(expe_num)

i2c = board.I2C()
ina228 = adafruit_ina228.INA228(i2c)

print("INA calibration")

# The shunt resistor is 1 Ohm
ina228.set_calibration(1.00, 0.2)

# Configuration of the INA: trade-off longer conversion time for better accuracy
ina228.mode = adafruit_ina228.Mode.CONTINUOUS
ina228.bus_voltage_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.shunt_voltage_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.temp_conv_time = adafruit_ina228.ConversionTime.TIME_1052_US
ina228.averaging_count = adafruit_ina228.AveragingCount.COUNT_16

# Start measurements
pi = pigpio.pi()
if not pi.connected:
    print("pigpiod need to run in background")
    exit(0)
pi.callback(EXPE_PIN, pigpio.EITHER_EDGE, next_expe)
current_samples = [[] for _ in range(nb_expes)]
print("Sampling starts")
while not done and (time.time() - deadline) < DEADLINE_ITERATION:
    val = ina228.current*1000
    if started:  # only measure current when expe starts 
        current_samples[expe_num].append((val, round(time.time()-start_time, 2)))
    if live:
        if len(current_samples[expe_num])>2:
            print(f"{val:.3f}, mean: {mean(current_samples[expe_num]):.3f}, std: {stdev(current_samples[expe_num]):.3f}, median: {median(current_samples[expe_num]):.3f}, max: {max(current_samples[expe_num]):.3f}, min: {min(current_samples[expe_num]):.3f}")
    time.sleep(0.0050) # >100Hz sampling (getting current is <0.0050s). 

# Write results
print("Sampling ends")
result_file = "results.csv"
with open(result_file, "w") as f:
    f.write("expe_num,current_sample,timing_sample\n")
    for expe_num, samples in enumerate(current_samples):
        for current_sample in samples:
            f.write(f"{expe_num},{current_sample},\n")
    for expe_num, timing_sample in enumerate(timing_samples):
        f.write(f"{expe_num},,{timing_sample}\n")
print("Done")
