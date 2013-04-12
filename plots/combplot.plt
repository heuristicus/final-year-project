reset

set terminal epslatex size 5.0,3.5 standalone color colortext header "\\usepackage{libertine}\n"
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#77FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#00FF00' linetype 1 linewidth 3 # green
set style line 3 linecolor rgb '#0000FF' linetype 1 linewidth 3 # blue
set style line 4 linecolor rgb 'magenta' linetype 1 linewidth 3 # 

set xlabel '$$t$$'
set ylabel '$$\lambda$$

plot "$1" using 1:2 with lines linestyle 1 title 'Characteristic function',\
     "$2" using 1:2 with lines linestyle 2 title 'Final estimate',\
     "$3" using 1:2 with lines linestyle 3 title 'Stream 1 estimate',\
     "$4" using ($$1+$5):2 with lines linestyle 4 title 'Stream 2 estimate (shifted by $5)'
