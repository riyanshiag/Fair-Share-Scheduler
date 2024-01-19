# Fair-Share-Scheduler

This repository contains C code implementation for the Fair Share Scheduler

## Input file

This file contains the jobs that are to be scheduled
First line represents the number of jobs to be scheduled
Each job has 3 lines
<br/>
First line: Group id, Base Priority, Arrival Time, Number Of CPU Bursts
<br/>
Second line: The array Worst Case CPU bursts
<br/>
Third Line: The array of I/O bursts (No line if it's a CPU bound process)

## Note
Number of IO bursts is always assumed to be one less than number of CPU bursts
## Executing

To run the code:

```bash
make
./a.out <input_file> <time_slice>
```

