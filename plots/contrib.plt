reset

set terminal epslatex size 5.0,3.5 standalone color colortext header "\\usepackage{libertine}\n"
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#00FF00' linetype 1 linewidth 3 # green

set xlabel '$$t$$'
set ylabel '$$\lambda$$'

set key on

set xtics 20
set ytics 20
plot "$1" using 1:2 with lines linestyle 1 title 'Gaussian Sum ($$\lambda(t)$$)',\
     "$2" using 1:2 with lines linestyle 2 title 'Constructing Gaussians'