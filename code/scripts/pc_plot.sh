#!/bin/bash

if [ $# -ne 3 ]
then
    echo "Missing arguments.\nusage: `basename $0` outfile stream_data_all IWLS_output baseline_output"
    exit
fi

gnuplot << EOF
# $1 is the file to output to, $2 is all data from the generator, $3 is the data produced by the estimator
call "pcplot.plt" "$1" "$2" "$3"

EOF

latex -interaction=nonstopmode $1.tex
dvips -o $1.ps $1.dvi
ps2pdf $1.ps
rm $1.tex $1.ps $1.dvi $1.log $1.aux $1-inc.eps
evince $1.pdf&


# ./generator -p params.txt
# ./estimator_piecewise output.txt_stream_0_ev est_out.txt

# sleep 1

# gnuplot -persist << EOF
# set key top left
# plot "./output.txt_stream_0_ad" index 0 using 1:2 title "Actual function" with lines,\
# "./output.txt_stream_0_ad" index 1 using 1:2 title "Events per second (original)" with lines,\
# "./est_out.txt" using 1:2 title "Estimated function" with lines
# #"./spline.gnu" using 1:2 smooth cspline
# EOF
