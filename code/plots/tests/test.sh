#!/bin/sh

gnuplot<<EOF

call "test.gp" $1

EOF

# Run tex on the file to create a dvi file and an eps file
latex -interaction=nonstopmode $1.tex
# Convert dvi file to postscript
dvips -o $1.ps $1.dvi
#rm $1.aux $1.log $1.dvi
evince $1.ps&