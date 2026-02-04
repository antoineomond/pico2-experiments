cd openocd
cd ../build/ && make -j4 && cd - && sudo src/openocd -s tcl -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c "adapter speed 5000" -c "program ../build/experiments/lposc/lposc_benchmarks.elf verify reset exit"
