*This project has been created as part of the 42 curriculum by so-ait-l*

## Description

Codexion is a concurrency simulation inspired by the Dining Philosophers problem, reimagined in a software development context. N coders sit at a circular table, each needing two USB dongles to compile their code. Coders alternate between compiling, debugging, and refactoring. The simulation ends when a coder burns out (goes too long without compiling) or all coders reach the required number of compilations.

## Instructions

**Compilation:**
```bash
make
```
**Execution:**
```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```
**Example:**
```bash
./codexion 5 3500 200 200 200 3 100 fifo
./codexion 5 3500 200 200 200 3 100 edf
```
**Arguments:**
- `number_of_coders` — number of coders (minimum 2)
- `time_to_burnout` — time in ms before a coder burns out without compiling
- `time_to_compile` — time in ms to compile
- `time_to_debug` — time in ms to debug
- `time_to_refactor` — time in ms to refactor
- `number_of_compiles_required` — number of compilations each coder must complete
- `dongle_cooldown` — time in ms a dongle must wait before being reused
- `scheduler` — scheduling algorithm: `fifo` or `edf`

**Clean:**
```bash
make clean   # removes object files
make fclean  # removes object files and binary
make re      # full rebuild
```

## Resources

**Documentation:**
- The Linux Programming Interface — Michael Kerrisk (Chapters 29-31)
- POSIX Threads Programming — Blaise Barney (Lawrence Livermore National Laboratory)
- pthread_create, pthread_mutex_lock, pthread_cond_wait man pages

**AI Usage:**
Claude (Anthropic) was used throughout this project for understanding thread concepts (mutexes, condition variables, deadlocks), debugging data races detected by ThreadSanitizer and Helgrind, understanding heap data structure implementation for EDF scheduler, and code review and norm compliance guidance. The core logic, algorithms, and implementation decisions were made by the student.

## Blocking Cases Handled

**Deadlock Prevention — Lock Ordering:** Each coder needs two dongles to compile. Without precautions, all coders could grab one dongle each and wait forever for the second (circular wait). This is solved by always acquiring the lower-ID dongle first, then the higher-ID dongle, eliminating circular wait.

**Starvation Prevention — FIFO Scheduler:** Without ordering, some coders might never get access to dongles. The FIFO queue ensures every coder gets a turn in order of arrival — first come, first served.

**Priority Scheduling — EDF Scheduler:** The Earliest Deadline First scheduler uses a min-heap to always give priority to the coder closest to burning out, minimizing the risk of burnout under load.

**Race Conditions — Mutex Protection:** All shared variables are protected by mutexes: `log_mtx` protects printf output, `sim_mtx` protects simulation_over flag, `coder_mtx` protects coder state, and per-dongle mutex protects each dongle's queue/heap state.

**Dongle Cooldown:** After a dongle is released, it must wait `dongle_cooldown` milliseconds before being acquired again, preventing the same coder from immediately reacquiring it.

## Thread Synchronization Mechanisms

**Mutexes (`pthread_mutex_t`):** Used to protect all shared data. Each shared resource has its own dedicated mutex to minimize contention and maximize parallelism.

**Condition Variables (`pthread_cond_t`):** Each dongle has a condition variable used to wake waiting coders when the dongle is released. `pthread_cond_broadcast` wakes all waiting coders, who then check if it is their turn.

**POSIX Threads (`pthread_t`):** Each coder is represented by a thread created with `pthread_create`. A dedicated monitor thread watches for burnout and completion conditions. All threads are joined with `pthread_join` before cleanup.

**Atomic Operations:** `simulation_over` flag is always read and written under `sim_mtx` protection to ensure all threads see consistent state.
