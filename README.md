# Mispredict Feedback Example

This project demonstrates how to leverage PMU feedback with SEP on Windows or `perf` on Linux.

## CMake Module

For portability, this project is described as a CMake project.

Included is an attempt at an `HWPGO.cmake` module which can help to add HWPGO feedback to an existing project.

The top-level `CMakeLists.txt` contains comments describing how to use `HWPGO.cmake`, which should be suitable for use in other CMake projects, although it has not been widely tested.

## Usage

Building without HWPGO:

    cmake -G Ninja -B nopgo -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=icx
    cmake --build nopgo --parallel

Building with HWPGO:

    cmake -G Ninja -B hwpgo -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=icx -DHWPGO=On
    cmake --build hwpgo --parallel

## Results

### Mispredicts Triggering Aggresive Speculation and `cmov`

In the case of the `unpredictable` executable the "if" condition in the hot loop is difficult to predict:
https://github.com/tcreech-intel/hwpgo-mispredict-example/blob/493f07bfa8778828827d13cadc6d403bed9c2bc8/unpredictable.c#L24-L31

Profitability heuristics based on static analysis are unlikely to eliminate control flow and implement the selection of `p` with `cmov` because it would require unconditionally computing `z` in case its value is needed.
HWPGO allows the compiler to understand that more aggressive speculation is worthwhile, and so it implements the loop body with conditional moves rather than a branch.

    $ time ./nopgo/unpredictable
    ./nopgo/unpredictable  0.99s user 0.00s system 99% cpu 0.992 total

    $ time ./hwpgo/unpredictable
    ./hwpgo/3  0.19s user 0.00s system 99% cpu 0.197 total

The result is approximately a 5x speedup.

### Lack of Mispredicts Preserving Control Flow

The `predictable` example shows the case where a branch is predictable and `cmov` conversion would be harmful.

Although the branch remains taken about 50% of the time, the taken/not-taken pattern is predictable to hardware.
HWPGO generates an empty mispredict profile in this case, indicating that there is no mispredict problem.
As a result, `cmov` is not used and performance is preserved.

https://github.com/tcreech-intel/hwpgo-mispredict-example/blob/493f07bfa8778828827d13cadc6d403bed9c2bc8/predictable.c#L22-L29
