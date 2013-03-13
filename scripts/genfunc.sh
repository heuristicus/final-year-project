#!/bin/sh
for ALPHA in 04 08 1 2 3; do
    echo "Setup parameters for alpha=$ALPHA"
    read RESP
    for FNUM in 1 2 3 4 5; do
	echo "Moving to directory rand_streams/alpha_$ALPHA/f$FNUM"
	cd /home/michal/Dropbox/study/university/final_year/final-year-project/src/rand_streams/alpha_$ALPHA/f$FNUM
	../../../deltastream -g ../../../../data/params.txt -f rand -c 5 -n 2 -t 3 -u
	sleep 1
    done
done