const arr = [];
for (let i = 0; i < 10000; i++) {
    arr.push(i * 2);
}
const total = arr.reduce((a, b) => a + b, 0);
console.log(total);
