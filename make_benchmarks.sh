#!/usr/bin/env bash

make host-sched N=4
make host-sched N=8
make host-sched N=16
make host-sched N=32
make host-sched N=64
make host-sched N=128
make host-sched N=256

#make arm64-run-sched N=4
#make arm64-run-sched N=8
#make arm64-run-sched N=16
#make arm64-run-sched N=32
#make arm64-run-sched N=64
#make arm64-run-sched N=128
#make arm64-run-sched N=256
