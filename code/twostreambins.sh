./generator -p params.txt

gnuplot<<EOF

call "twostreambins.plt" "$1" "$2" "$3"

EOF

latex -interaction=nonstopmode $1.tex
dvips -o $1.ps $1.dvi
ps2pdf $1.ps
rm $1.tex $1.ps $1.dvi $1.log $1.aux $1-inc.eps
evince $1.pdf&