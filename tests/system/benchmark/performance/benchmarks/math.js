let sum = 0;
for (let i = 1; i <= 50000; i++) {
    sum += (i * i) + (i % 7);
}
console.log(sum);
