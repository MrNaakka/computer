load r0 0 #sum
load r1 2 # the thing being summed
load r2 0 #loop index
load r3 8 #loop bound
load r10 1 #loop index add


loop:
lt r4 r3 r2
jmpc r4 end
add r0 r0 r1
add r2 r2 r10
jmpa loop
end:
halt


