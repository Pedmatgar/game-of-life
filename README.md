# Game of Life (C) — Beginner Friendly Guide

If you are new to C projects, start here.  
This repository has two parts:

- `life/`: core simulation + benchmark executable
- `animations/`: animation generator app

---

## 1) Fastest way to run something

From the repository root:

```bash
cd life
make
```

What this does:

1. Compiles `src/life.c` + `src/life_test.c`
2. Creates `build/life_test`
3. Runs it immediately

Typical output:

```text
Performance benchmark: 71 x 71 world, 1000 iterations each

dense                           iters=1000  total=... s  avg=... s  throughput=... cells/s
checkerboard                    iters=1000  total=... s  avg=... s  throughput=... cells/s
sparse (cross)                  iters=1000  total=... s  avg=... s  throughput=... cells/s
```

If you can run this command, your setup is working.

---

## 2) Requirements

Minimum (for normal run):

- `gcc`
- `make`
- Linux/macOS environment

Optional (only for MPI/distributed run):

- `mpicc`
- `mpirun`

---

## 3) Install help (common systems)

### Ubuntu / Debian

```bash
sudo apt update
sudo apt install -y build-essential make
```

Optional MPI:

```bash
sudo apt install -y openmpi-bin libopenmpi-dev
```

### macOS (Homebrew)

```bash
brew install make gcc
```

Optional MPI:

```bash
brew install open-mpi
```

---

## 4) MPI mode (optional)

If you do **not** have MPI installed, skip this section.

Build MPI executable:

```bash
cd life
make life_mpi_test
```

Run with 4 processes:

```bash
mpirun -np 4 ./build/life_mpi_test
```

Note: normal `make` does not need MPI.

---

## 5) Troubleshooting (“I cannot run it”)

### `make: command not found`
Install `make` (see section 3).

### `gcc: command not found`
Install compiler tools (see section 3).

### `mpicc: No such file or directory`
You tried MPI build without MPI installed.  
Either:

- install MPI (section 3), or
- use regular mode only:

```bash
cd life
make
```

### Permission errors
Run inside your own writable clone directory and avoid system-protected paths.

### Command works but numbers differ from docs
That is expected. Timing depends on CPU, OS, background processes, and compiler.

---

## 6) Timing APIs (for developers)

Include:

```c
#include "life.h"
```

Available timing functions:

- `update_world_timed(...)`  
  Time for one generation
- `update_world_n_generations_timed(...)`  
  Time for `n` generations (optionally per-step)
- `update_world_n_generations_mpi_timed(...)` (only with `-DUSE_MPI`)  
  Time for `n` generations using MPI row decomposition

All return elapsed time in seconds (`double`).

---

## 7) Build animations app

```bash
cd animations
make
```
