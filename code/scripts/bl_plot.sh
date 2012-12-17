#!/bin/sh

if [ $# -ne 4 ]
then
    echo -e "Missing arguments.\nusage: `basename $0` outfile stream_data_all IWLS_output baseline_output"
    exit
fi

gnuplot << EOF
# $1 is the file to output to, $2 is all data from the generator, $3 is the data produced by the IWLS, $4 is the output from the baseline estimate
call "../plots/blplot.plt" "$1" "$2" "$3" "$4"

EOF

latex -interaction=nonstopmode $1.tex
dvips -o $1.ps $1.dvi
ps2pdf $1.ps
rm $1.tex $1.ps $1.dvi $1.log $1.aux $1-inc.eps
evince $1.pdf&
