cd /media/michal/Edison/fyp/new/sine/fine/fine_results/results/ttest
data=dlmread("alpha_009.txt_ttest", "\t", [0,0,9,3])

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
    pvalSig(end+1)=t_test_2(d1, d2)
    pvalErrorDiff(end+1)=t_test(d3,0)
endfor
endfor
