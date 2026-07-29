# parameters
EXPE_NAME=wfe_busy_wait
CONF_NAME=configurations-onlyprime.csv
FILE_TO_DEPLOY='build/experiments/experiments_run/experiments_run_adhoc.elf'
NB_EXPES=1
NB_ITERS=3
NB_BENCHMARKS=1
DEADLINE_ITERATION=3600

cd build/ && make -j4 && cd -
scp -r "$FILE_TO_DEPLOY" dell_local:research/pico2-experiments/"$FILE_TO_DEPLOY"
ssh raspberrypi "cd /root/dw_ina && source venv/bin/activate && nohup python -u measurements.py $NB_EXPES $NB_ITERS 0 0 $NB_BENCHMARKS $DEADLINE_ITERATION > measurements.log 2>&1 < /dev/null" < /dev/null &
PID=$!
ssh -t dell_local "cd research/pico2-experiments/openocd && sudo src/openocd -s tcl -f interface/cmsis-dap.cfg -f target/rp2350.cfg -c 'adapter speed 5000' -c 'program ../$FILE_TO_DEPLOY verify reset exit'"
tail --pid=$PID -f /dev/null

mkdir -p "results/$EXPE_NAME"
cp "results/$CONF_NAME" "results/$EXPE_NAME/configurations.csv"
scp raspberrypi:/root/dw_ina/results.csv "results/$EXPE_NAME/results.csv"
cd results && Rscript plot.r "$EXPE_NAME/" && cd -

echo "Showing results/$EXPE_NAME/result.pdf"
evince "results/$EXPE_NAME/result.pdf"
