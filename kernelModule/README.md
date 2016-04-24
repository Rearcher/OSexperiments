# Linux Kernel Module Hello World

## Build the module
```bash
sudo make
sudo insmod helloworld.ko
```

## Test the module
```bash
gcc benchmark.c -p benchmark
sudo ./benchmark
```

## See the kernel output
```bash
dmesg | tail -n10
```

## Remove & Clean
```bash
sudo rmmod helloworld
sudo make clean
```
