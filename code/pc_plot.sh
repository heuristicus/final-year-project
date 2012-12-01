#!/bin/bash

./generator -p params.txt
./estimator_piecewise output.txt_stream_0_ev est_out.txt

sleep 1

gnuplot -persist << EOF
set key top left
plot "./output.txt_stream_0_ad" index 0 using 1:2 title "Actual function" with lines,\
"./output.txt_stream_0_ad" index 1 using 1:2 title "Events per second (original)" with lines,\
"./est_out.txt" using 1:2 title "Estimated function" with lines
#"./spline.gnu" using 1:2 smooth cspline
EOF

