# Signals-Threading
A program in which one thread reads a signal from an input file, sends this signal to another thread using shared memory, and writes this signal to an output file. The program should continue running until all data in the input file is exhausted.

## Signal

The signal can take one of two forms:

1. ConnectionRequest
- **header** - structure
- **connectionId** – 32 bits – connection identifier number assigned by a higher layer
- **configPreset** – 4 bits – number from 0 to 10 identifying the configuration preset
- **signalStrength** – 10 bits – encoded value describing the incoming signal strength

2. DownlinkData
- **header** - structure
- **noOfConnections** – 4 bits – 0 to 15 connections; specifies the number of objects in the array below
- **connectionDownlinkDataDetails[]** - array of structures

The following structures are parts of the signals:

- **header:**
  - **signalType** – 4 bits – signal identifier number:
    - 1 – connection request
    - 4 – DownlinkData
    - Other values reserved for other signals
  - **signalSize** – 12 bits – signal size (including header) in bytes

- **connectionDownlinkDataDetails:**
  - **connectionId** – 32 bits
  - **mcs** – 5 bits – modulating code scheme
  - **tbs** – 7 bits – transport block size
  - **dataSize** – 32 bits – data size in bytes

## Program

- Multithreaded program.
- Threads communicate exclusively through shared memory.
- A signal occupies one continuous block of shared memory.
- The signal occupies the minimum possible amount of memory in the shared memory area.
- Each signal is exactly one line.
- Each line consists of a series of positive integers separated by spaces, representing consecutive values of the fields of the structures representing the signals.
- The program is resilient to incorrect input data (such lines are ignored).
- The signal is printed in a format consistent with JSON.
- The program terminates after processing the last line of input and printing the last line of output.
- Reading input from the file input.in and writing output to the file output.out.