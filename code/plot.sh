#!/bin/bash
./generator -p params.txt
./estimator output.txt_ev est_out.txt
gnuplot -persist << EOF
plot "./output.txt_ad" index 0 using 1:2 with lines, \
"./output.txt_ad" index 1 using 1:2 with lines, \
"./est_out.txt" using 1:2 with lines
EOF
