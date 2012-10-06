set terminal postscript enhanced color

set output "lambda_time.pdf"

set title "{/Symbol l} over time"
set xlabel "Time (seconds)"
set ylabel "{/Symbol l}"
set key top right
set style line 1 lt 1 lw 2 lc 1
set style line 2 lt 1 lw 2 lc 3
plot "nonhom" index 1 using 1:(1.0) title "Event times" with impulses ls 2,\
"nonhom" index 0 using 1:2 title "{/Symbol l}" with lines ls 1

