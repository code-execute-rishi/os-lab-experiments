# Operating Systems Laboratory (CS-502) — Experiments 5 & 6

[![Language: C](https://img.shields.io/badge/language-C99%20%2F%20POSIX-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Python 3.7+](https://img.shields.io/badge/python-3.7+-green.svg)](https://www.python.org/downloads/)
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

> Complete, modular, and verified implementations for **Operating Systems Laboratory (CS-502)** Experiments 5 and 6. Features both **native POSIX C** implementations and an **object-oriented Python suite**.

---

## 👨‍🎓 Student & Academic Details

* **Student Name:** Aradhya Jain
* **Scholar Number:** `24112011111`
* **Department & Section:** Department of Computer Science & Engineering, `CSE-1`
* **Institution:** Maulana Azad National Institute of Technology (MANIT), Bhopal
* **Course:** Operating Systems Laboratory (`CS-502`)
* **Semester:** V Semester, B.Tech CSE

---

## 📋 Table of Experiments

| Experiment | Title & Focus | C Function / Module | Python Class | CLI Flag |
| :---: | :--- | :--- | :--- | :---: |
| **Lab 5 (a)** | **Theory of Process System Calls**<br>• `fork()`, `execve()`, `wait()` prototypes & semantics<br>• In-depth matrix of all 6 `exec` variants (`execl`, `execv`, `execle`, `execve`, `execlp`, `execvp`) | *(Documented in source comments)* | *(Documented in comments)* | — |
| **Lab 5 (b1)** | **Process Hierarchy & Forking**<br>• User enters integer $N$<br>• Spawns $N$ sibling child processes via `fork()`<br>• Prints Child PID, Parent PID, and PPID dynamically<br>• Reaps child exit codes avoiding zombies | `run_lab5_part_b1(n)` | `Lab5_ProcessManagement.fork_n_children(n)` | `--lab5b1 4` |
| **Lab 5 (b2)** | **Process Synchronization (`wait`)**<br>• Demonstrates parent process blocking on `wait()`<br>• Child performs 2s task and terminates with exit code 42<br>• Parent decodes exit status via `WIFEXITED` & `WEXITSTATUS` | `run_lab5_part_b2()` | `Lab5_ProcessManagement.parent_wait_child()` | `--lab5b2` |
| **Lab 6 (a)** | **Generic CPU Scheduling Algorithms**<br>• **FCFS** (First Come First Serve)<br>• **SJF** (Shortest Job First - Non-Preemptive)<br>• **SRTF** (Shortest Remaining Time First - Preemptive SJF)<br>• **Round Robin (RR)** with configurable time quantum $q$<br>• **Priority (Non-Preemptive)** & **Priority (Preemptive)**<br>• Calculates CT, TAT, WT, RT, and averages | `run_lab6_part_a()` | `Lab6_CPUScheduler` | `--lab6a` |
| **Lab 6 (b)** | **Round Robin Benchmark & Quantum Sweep**<br>• Workload: $P_1(0,10)$, $P_2(1,4)$, $P_3(3,6)$, $P_4(4,7)$, $P_5(5,2)$<br>• Sweeps time quantum $q = 1$ to $10\text{ ms}$<br>• Context switch tracking<br>• Identifies optimal quantum for TAT, WT, and RT | `run_lab6_part_b()` | `Lab6_RoundRobinQuantumSweep` | `--lab6b` |

---

## ⚡ Quick Start & Compilation

### Clone the Repository
```bash
git clone https://github.com/code-execute-rishi/os-lab-experiments.git
cd os-lab-experiments
```

### 1. Compile & Run the Native C Engine
Compile using `make` or any standard C compiler (`gcc` / `clang`):
```bash
# Using Makefile
make

# Or compile directly
gcc -Wall -Wextra -O2 os_labs_5_and_6.c -o os_labs_5_and_6

# Run full automated test suite
./os_labs_5_and_6 --all

# Or run interactively
./os_labs_5_and_6
```

### 2. Run the Object-Oriented Python Version
Requires no third-party packages (uses standard Python 3):
```bash
# Run all demonstrations
python3 os_labs_5_and_6.py --all

# Run specific experiment
python3 os_labs_5_and_6.py --lab5b1 4
python3 os_labs_5_and_6.py --lab5b2
python3 os_labs_5_and_6.py --lab6a
python3 os_labs_5_and_6.py --lab6b
```

---

## 📊 Benchmark Simulation Results (Lab 6 Part B)

Simulation of the benchmark workload across $q = 1$ to $10\text{ ms}$:

| Quantum ($q$) | Avg TAT (ms) | Avg WT (ms) | Avg RT (ms) | Context Switches |
| :---: | :---: | :---: | :---: | :---: |
| $q = 1$ | 19.00 | 13.20 | **1.20** | 29 |
| $q = 2$ | 18.80 | 13.00 | 3.00 | 15 |
| $q = 3$ | 20.00 | 14.20 | 4.60 | 12 |
| $q = 4$ | 19.40 | 13.60 | 6.20 | 9 |
| $q = 5$ | 19.80 | 14.00 | 6.80 | 8 |
| $q = 6$ | 18.80 | 13.00 | 8.20 | 7 |
| $q = 7$ | 18.80 | 13.00 | 9.20 | 6 |
| $q = 8$ | 19.60 | 13.80 | 10.00 | 6 |
| $q = 9$ | 20.40 | 14.60 | 10.80 | 6 |
| $q = 10$ | **17.40** | **11.60** | 11.60 | 5 |

### 🏆 Optimal Quantum Values:
* **Optimal Quantum for Turnaround Time (TAT):** $q = 10\text{ ms}$ (Avg TAT = $17.40\text{ ms}$)
* **Optimal Quantum for Waiting Time (WT):** $q = 10\text{ ms}$ (Avg WT = $11.60\text{ ms}$)
* **Optimal Quantum for Response Time (RT):** $q = 1\text{ ms}$ (Avg RT = $1.20\text{ ms}$)

---

## 🔬 Sample Terminal Traces

<details>
<summary><b>Click to expand sample execution output</b></summary>

```text
======================================================================
  OS LAB 5 (Part b1): Generating 4 Child Processes via fork()
  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1
======================================================================
[ROOT PARENT] PID = 42100 initiating spawn loop for 4 children...
----------------------------------------------------------------------
Child #    Status       Child PID      Parent PID     PPID (getppid)
----------------------------------------------------------------------
1          ACTIVE       42101          42100          42100         
2          ACTIVE       42102          42100          42100         
3          ACTIVE       42103          42100          42100         
4          ACTIVE       42104          42100          42100         
----------------------------------------------------------------------
[PARENT] Waiting for child processes to exit and reaping exit codes...
[REAPED] Child PID 42101  exited normally with code: 10
[REAPED] Child PID 42102  exited normally with code: 20
[REAPED] Child PID 42103  exited normally with code: 30
[REAPED] Child PID 42104  exited normally with code: 40
======================================================================
[SUCCESS] All 4 child processes spawned, executed, and reaped cleanly.

========================================================================================
  CPU SCHEDULING ALGORITHM: Shortest Remaining Time First (SRTF - Preemptive)
========================================================================================
PID      Arrival(AT)  Burst(BT)    Priority   CT       TAT      WT       RT      
----------------------------------------------------------------------------------------
P1       0            10           3          29       29       19       0       
P2       1            4            1          5        4        0        0       
P3       3            6            4          13       10       4        4       
P4       4            7            2          20       16       9        9       
P5       5            2            5          7        2        0        0       
----------------------------------------------------------------------------------------
Average Turnaround Time (TAT) : 12.20 ms
Average Waiting Time (WT)    : 6.40 ms
Average Response Time (RT)   : 2.60 ms
========================================================================================
```

</details>

---

## 📄 License

This repository is distributed under the [MIT License](LICENSE).
