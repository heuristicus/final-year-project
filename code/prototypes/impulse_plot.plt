set terminal postscript enhanced color

set output "lambda_time.pdf"

set title "{/Symbol l} over time"
set xlabel "Time (seconds)"
set ylabel "{/Symbol l}"
set yrange [0.5:3.5]
set key top left
set style line 1 lt 1 lw 2 lc 1
set style line 2 lt 1 lw 2 lc 2
plot "nonhom" index 1 using 1:(1.0) title "Event times" with impulses ls 2,\
"nonhom" index 0 using 1:2 title "{/Symbol l} variation" with lines ls 1