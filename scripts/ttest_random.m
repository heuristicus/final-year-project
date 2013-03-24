cd /media/michal/Edison/fyp/new/random/rand_results/results/ttest
files=dir(fullfile("/media/michal/Edison/fyp/new/random/rand_results/results/ttest", '*'))

for ind = 1:2
  files(ind).name

  data = dlmread(files(ind).name, "\t", [0,0,4,3])

  for i = 1:4
    for j = i:4
      if j==i
	continue
      endif
      d1=data(:,i)
      d2=data(:,j)
      d3=d1-d2
      i
      j
      pvalSig=t_test_2(d1, d2)
      pvalErrorDiff=t_test(d3,0)
    endfor
  endfor
endfor
