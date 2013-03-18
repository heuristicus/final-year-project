#!/bin/sh
OUTPUT_DIR=/home/michal/Dropbox/study/university/final_year/final-year-project/src/fine_streams
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
PARAM_FILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
for ALPHA in 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015; do
    # Change the alpha to a floating point value
    REAL_ALPHA="`echo $ALPHA | sed 's/^[[:digit:]]/0./g'`"
    # Replace the value in the parameter file with the value of alpha
    sed -i "s/alpha [0-9.]*/alpha $REAL_ALPHA/" $PARAM_FILE
    cd $OUTPUT_DIR/alpha_$ALPHA
    pwd
    $LAUNCHER_LOC -g $PARAM_FILE -c 10 -n 2 -t 3
    sleep 1
done