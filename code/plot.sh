#!/bin/bash
./generator -p params.txt
./estimator output.txt_ev est_out.txt
gnuplot -persist << EOF
set key top left
plot "./output.txt_ad" index 0 using 1:2 title "Actual function" with lines, \
"./output.txt_ad" index 1 using 1:2 title "Events per second (original)" with lines, \
"./est_out.txt" using 1:2 title "Estimated function" with lines,\
EOF
#"./est_out.txt" using 1:3 title "Events per time interval (estimator)" with lines,\
#"./est_out.txt" using 1:4 title "Lambda estimates" with lines
