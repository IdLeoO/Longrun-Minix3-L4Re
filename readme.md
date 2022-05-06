# Longrun Test for L4Re and Minix3

In this work simple longrun tests for L4Re and Minix3 are developed. 

## Setup

1. Install [Minix3 on VirtualBox](https://wiki.minix3.org/doku.php?id=usersguide:runningonvirtualbox) and [set it up](https://wiki.minix3.org/doku.php?id=usersguide:doinginstallation).

2. Get [docker image](https://docs.sel4.systems/projects/dockerfiles/), here we use the building environment for SeL4.

3. Log in the building image, build l4Re with:

   ```bash
   # Download and compile l4re with:
   wget https://l4re.org/download/snapshots/l4re-base-22.04.0.tar.xz
   tar xvf l4re-base-22.04.0.tar.xz
   cd l4re-base-22.04.0/
   make setup
   make -j7
   cd ..
   
   # Download and compile the test with:
   wget http://retis.santannapisa.it/luca/AdvancedOS/Old-19/l4-POSIX_test.tgz
   tar xvf l4-POSIX_test.tgz
   ```

## Experiment

* Modify `NTHREADS ` to test for different thread number

### Minix3

1. Use [pkgin](https://wiki.minix3.org/doku.php?id=usersguide:installingbinarypackages) to install **clang** and **vim**

   [SSH on Minix3](https://wiki.minix3.org/doku.php?id=usersguide:settingupssh) is supported

2. Generate testing code as `minix3/longrun.c`

3. Compile and run with

   ```bash
   clang longrun.c -o longrun -lmthread
   ./longrun
   ```

### L4Re

1. Modify `POSIX_Test/server/src/main.c` to proposed code in `l4re/main.c`

2. Compile and test with

   ```bash
   # Compile the test with:
   cd POSIX_Test
   make L4DIR=$(pwd)/../l4re-base-22.04.0/src/l4 O=$(pwd)/../l4re-base-22.04.0/obj/l4/x86
   
   # Test with:
   cd ../l4re-base-22.04.0
   
   qemu-system-i386 -nographic -kernel obj/l4/x86/bin/x86_gen/bootstrap -append bootstrap -initrd "obj/fiasco/ia32/fiasco -serial_esc,obj/l4/x86/bin/x86_gen/l4f/sigma0,obj/l4/x86/bin/x86_gen/l4f/moe --init=rom/posix_test,obj/l4/x86/bin/x86_gen/l4f/l4re,obj/l4/x86/bin/x86_gen/l4f/posix_test"
   ```



## Reference

* Advanced Operating Systems: http://retis.sssup.it/luca/AdvancedOS/