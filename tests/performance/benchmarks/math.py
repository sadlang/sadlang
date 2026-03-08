sum_val = 0
for i in range(1, 50001):
    sum_val += (i * i) + (i % 7)
print(sum_val)
