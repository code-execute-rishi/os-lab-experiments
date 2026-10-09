# Operating Systems Laboratory (CS-502) — Experiments 5 & 6

[![Language: C](https://img.shields.io/badge/language-C99%20%2F%20POSIX-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Python 3.7+](https://img.shields.io/badge/python-3.7+-green.svg)](https://www.python.org/downloads/)
[![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://opensource.org/licenses/MIT)

> Complete, modular, and verified implementations and theoretical foundation for **Operating Systems Laboratory (CS-502)** Experiments 5 and 6. Features both **native POSIX C** implementations and an **object-oriented Python suite**.

---

## 👨‍🎓 Student & Academic Details

* **Student Name:** Aradhya Jain
* **Scholar Number:** `24112011111`
* **Department & Section:** Department of Computer Science & Engineering, `CSE-1`
* **Institution:** Maulana Azad National Institute of Technology (MANIT), Bhopal
* **Course:** Operating Systems Laboratory (`CS-502`)
* **Semester:** V Semester, B.Tech CSE

---

## 📖 Comprehensive Theory: Lab 4 vs. Lab 5 & Process Management

### 1. The Architectural Bridge: What Lab 4 Is and How It Connects to Lab 5

In Operating Systems, **Experiment 4** and **Experiment 5** represent the foundational transition from **Resource Management (I/O & Files)** to **Execution Management (Processes & Concurrency)**:

```
┌────────────────────────────────────────────────────────────────────────┐
│                   USER SPACE (Ring 3 - Unprivileged)                   │
├───────────────────────────────────┬────────────────────────────────────┤
│   Lab 4: Low-Level File I/O       │   Lab 5: Process Lifecycle         │
│   • open("data.txt", O_RDONLY)    │   • fork()                         │
│   • read(fd, buf, count)          │   • execve(path, argv, envp)       │
│   • write(fd, buf, count)         │   • wait(&status)                  │
│   • close(fd)                     │   • exit(code)                     │
└─────────────────┬─────────────────┴──────────────────┬─────────────────┘
                  │   System Call Trap (syscall / svc) │
┌─────────────────▼────────────────────────────────────▼─────────────────┐
│                  KERNEL SPACE (Ring 0 - Privileged)                    │
├───────────────────────────────────┬────────────────────────────────────┤
│   VFS & File Descriptor Table     │   Task Scheduler & Process Table   │
│   • Process FD Table [0, 1, 2...] │   • task_struct (PCB)              │
│   • System Open File Table        │   • Address Space (mm_struct)      │
│   • Inode / vnode structures      │   • PID, PPID, CPU Registers       │
└───────────────────────────────────┴────────────────────────────────────┘
```

#### How Lab 4 Leads Directly into Lab 5:
1. **File Descriptors Inherited on Fork:** When `fork()` is called in Lab 5, the kernel duplicates the calling process's File Descriptor Table created in Lab 4. Both parent and child share open file handles and file offset pointers.
2. **File Descriptors and Exec:** When `exec()` is called in Lab 5, open file descriptors remain accessible to the new program unless opened with the `O_CLOEXEC` flag from Lab 4.
3. **System Call Mechanism:** Both labs illustrate how user programs request kernel services across the CPU privilege boundary (Ring 3 $\to$ Ring 0) via software interrupts.

---

### 2. In-Depth Theory for Lab 5 (Process Management System Calls)

#### 2.1 What is a Process? (Program vs. Process)
* **Program:** A passive collection of instructions and binary data stored as an executable file on secondary storage (e.g., an ELF binary on disk).
* **Process:** An active program in execution. It encompasses:
  * **Text Segment:** Machine code instructions.
  * **Data & BSS Segments:** Initialized and uninitialized global/static variables.
  * **Heap:** Dynamically allocated memory (`malloc()` / `free()`).
  * **Stack:** Stack frames containing local variables, parameters, and return addresses.
  * **Process Control Block (PCB / `task_struct` in Linux):** Kernel metadata tracking PID, PPID, priority, memory map, open file descriptors, and scheduling state.

---

#### 2.2 The `fork()` System Call & Copy-on-Write (COW)
* **Prototype:** `pid_t fork(void);`
* **Purpose:** Creates an exact replica of the calling process.
* **Return Values:**
  * **To the Child:** Returns `0`. *(The child can always find its parent via `getppid()`)*.
  * **To the Parent:** Returns the **Child's PID** ($> 0$). *(The parent must know the child's identity to track, synchronize, or kill it)*.
  * **On Failure:** Returns `-1` and sets `errno` (e.g., `EAGAIN` if the system process limit is reached).
* **Copy-on-Write (COW) Optimization:** Rather than immediately duplicating entire memory address spaces (which would be expensive), the Linux kernel marks the parent's memory pages as *read-only* and shares them with the child. Only when either process attempts to *modify* a page does a page-fault trigger the kernel to allocate a distinct physical page for that process.

```
       Parent Process (PID: 1000)
             │
             ├── calls fork()
             │
             ├──────────────────────────┐
             ▼                          ▼
   Parent Continues           New Child Created (PID: 1001)
   (fork returns 1001)        (fork returns 0)
```

---

#### 2.3 Process Hierarchy & Spawning $N$ Children (Part b1)
When creating $N$ child processes in a loop, a critical design decision arises:
* **The "Fork Bomb" Trap (Exponential Cascade):** If child processes are allowed to continue executing the loop, each iteration doubles the number of processes ($2^N$), rapidly exhausting system resources.
* **Star Hierarchy (Sibling Creation):** To create exactly $N$ children belonging to one root parent:
  ```c
  for (int i = 1; i <= n; i++) {
      pid_t pid = fork();
      if (pid == 0) {
          // Inside Child: execute task and EXIT IMMEDIATELY!
          printf("Child #%d | PID: %d | Parent PID: %d\n", i, getpid(), getppid());
          exit(0); // Prevents child from looping!
      }
  }
  // Parent waits for all N children
  while (wait(NULL) > 0);
  ```

---

#### 2.4 The `exec()` System Call Family
* **Purpose:** Replaces the current process image (code, data, heap, stack) with a completely new binary executable. The **PID remains unchanged**.
* **Why Does `exec()` Not Return?** If `exec()` succeeds, the original calling code no longer exists in memory—it was overwritten by the new program. It only returns if an error occurs (returning `-1`).
* **Deciphering the 6 Variants:**
  * `l` = **List:** Arguments passed as a comma-separated list ending in `NULL`.
  * `v` = **Vector:** Arguments passed as an array of string pointers `char *argv[]`.
  * `p` = **Path:** Automatically searches directories in the `PATH` environment variable.
  * `e` = **Environment:** Accepts a custom environment array `char *envp[]` rather than inheriting `environ`.

| Function | Argument Format | Path Resolution | Environment | Type |
| :--- | :---: | :---: | :---: | :---: |
| `execl(path, arg0, ..., NULL)` | List (`l`) | Absolute / Relative Path | Inherited | Library wrapper |
| `execv(path, argv[])` | Vector (`v`) | Absolute / Relative Path | Inherited | Library wrapper |
| `execlp(file, arg0, ..., NULL)` | List (`l`) | Searches `$PATH` (`p`) | Inherited | Library wrapper |
| `execvp(file, argv[])` | Vector (`v`) | Searches `$PATH` (`p`) | Inherited | Library wrapper |
| `execle(path, arg0, ..., NULL, envp)` | List (`l`) | Absolute / Relative Path | Custom (`e`) | Library wrapper |
| `execve(path, argv[], envp)` | Vector (`v`) | Absolute / Relative Path | Custom (`e`) | **True Kernel System Call** |

---

#### 2.5 Process Synchronization: `wait()` & `waitpid()` (Part b2)
* **Prototype:** `pid_t wait(int *wstatus);`
* **Purpose:** Suspends execution of the calling parent process until one of its child processes terminates.
* **Status Bitmask Decoding:** The `wstatus` integer contains packed exit information decoded via POSIX macros:
  * `WIFEXITED(status)`: Evaluates to true if the child terminated normally (via `exit()` or returning from `main`).
  * `WEXITSTATUS(status)`: Extracts the 8-bit exit code (0–255) passed to `exit()`.
  * `WIFSIGNALED(status)`: Evaluates to true if the child was abruptly killed by an unhandled signal (e.g., `SIGSEGV`, `SIGKILL`).
  * `WTERMSIG(status)`: Returns the signal number that caused the termination.

---

#### 2.6 Zombie vs. Orphan Processes
* **Zombie Process (`<defunct>`):**
  * **Occurs when:** A child terminates via `exit()`, but its parent has not yet called `wait()`.
  * **State:** The child's memory is released, but its PCB and PID entry remain in the kernel process table so the parent can read the exit code.
  * **Hazard:** Accumulating thousands of zombies exhausts system PID slots.
* **Orphan Process:**
  * **Occurs when:** A parent process terminates before its child process completes.
  * **Resolution:** The Linux kernel immediately **re-parents** the orphan to `systemd` / `init` (PID 1), which periodically reaps its children automatically.

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

## 📄 License

This repository is distributed under the [MIT License](LICENSE).
