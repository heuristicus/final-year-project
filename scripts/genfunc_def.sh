OUTPUT_DIR=/home/michal/Dropbox/study/university/final_year/final-year-project/src/fine_streams
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
PARAM_FILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt

AVALS=(001 002 003 004 005 006 007 008 009 010 011 012 013 014 015)
NFUNCS=10
APREF=alpha_
DIVISOR=1000

for ALPHA in ${AVALS[@]}; do
    # Change the alpha to a floating point value
    NUM=$(echo "scale=3; $((10#$ALPHA))/$DIVISOR" | bc)
    # Replace the value in the parameter file with the value of alpha
    sed -i "s/alpha [0-9.]*/alpha $NUM/" $PARAM_FILE
    cd $OUTPUT_DIR/$APREF$ALPHA
    $LAUNCHER_LOC -g $PARAM_FILE -c $NFUNCS -n 2 -t 3
    sleep 1
done