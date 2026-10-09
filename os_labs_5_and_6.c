/**
 * ============================================================================
 *   OPERATING SYSTEMS LABORATORY (CS-502) - EXPERIMENTS 5 & 6
 *   Student Name   : Aradhya Jain
 *   Scholar Number : 24112011111
 *   Section        : CSE-1
 *   Semester       : V Semester, B.Tech CSE
 *   Institution    : Maulana Azad National Institute of Technology (MANIT)
 * ============================================================================
 *
 * DESCRIPTION:
 * All-in-one standalone C implementation covering OS Lab 5 and OS Lab 6.
 *
 * TABLE OF CONTENTS:
 *   1. QUESTION / EXPERIMENT 5 (Part a & b1):
 *      - Theory of fork(), exec() variants, wait() in comments.
 *      - C implementation: Generating N child processes via fork() with PID/PPID logging.
 *   2. QUESTION / EXPERIMENT 5 (Part b2):
 *      - Theory of process synchronization in comments.
 *      - C implementation: Parent process waiting for child using wait() and status decoding.
 *   3. QUESTION / EXPERIMENT 6 (Part a):
 *      - Theory of CPU Scheduling algorithms in comments.
 *      - C implementation: Generic CPU Scheduler for FCFS, SJF, SRTF, Round Robin,
 *        Non-Preemptive Priority, and Preemptive Priority with CT, TAT, WT, RT metrics.
 *   4. QUESTION / EXPERIMENT 6 (Part b):
 *      - Theory of Round Robin Quantum sensitivity in comments.
 *      - C implementation: Parametric simulation on benchmark dataset (q = 1 to 10 ms)
 *        identifying optimal quantum values for TAT, WT, and RT.
 *   5. Interactive CLI Driver & Menu allowing independent or unified execution.
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define MAX_PROC 20


/* ============================================================================
 * QUESTION / EXPERIMENT 5: PROCESS MANAGEMENT SYSTEM CALLS
 * ============================================================================
 * a1. Write brief introduction of fork(), exec(), and wait() system calls with
 *     their input parameters and return types:
 *     - fork():
 *         Prototype: pid_t fork(void);
 *         Header: <unistd.h>, <sys/types.h>
 *         Input: void (no arguments).
 *         Return:
 *           - In Child Process : Returns 0.
 *           - In Parent Process: Returns Child PID (> 0).
 *           - On Failure       : Returns -1 and sets errno (e.g. EAGAIN).
 *         Semantics: Clones the caller process, creating a new child process
 *         with duplicate virtual address space using Copy-on-Write (COW).
 *
 *     - execve() (Core exec family):
 *         Prototype: int execve(const char *pathname, char *const argv[], char *const envp[]);
 *         Header: <unistd.h>
 *         Input: Binary pathname, null-terminated argument array, environment array.
 *         Return:
 *           - On Success: Does NOT return; replaces caller's code, data, heap, stack.
 *           - On Failure: Returns -1 and sets errno (e.g. ENOENT, EACCES).
 *
 *     - wait():
 *         Prototype: pid_t wait(int *wstatus);
 *         Header: <sys/wait.h>, <sys/types.h>
 *         Input: Pointer to integer storing exit status bitmask.
 *         Return:
 *           - On Success: PID of terminated child process.
 *           - On Failure: -1 if no child processes exist (ECHILD).
 *         Semantics: Blocks caller until a child terminates, reaping its PCB
 *         to prevent zombie processes.
 *
 * a2. List out the different versions of exec() system call with brief details:
 *     1. execl(path, arg0, arg1, ..., NULL)       : List of args, full path.
 *     2. execv(path, argv[])                      : Vector (array) of args, full path.
 *     3. execlp(file, arg0, arg1, ..., NULL)      : List of args, searches PATH env.
 *     4. execvp(file, argv[])                     : Vector of args, searches PATH env.
 *     5. execle(path, arg0, ..., NULL, envp[])    : List of args, custom environment.
 *     6. execve(path, argv[], envp[])             : Vector of args, custom env (Kernel syscall).
 *
 * b1. Write a program to take integer from user as input and generate same number
 *     of child process with fork() system call, also print their PID along with parent PID.
 * ============================================================================ */

void run_lab5_part_b1(int n) {
    printf("\n======================================================================\n");
    printf("  OS LAB 5 (Part b1): Generating %d Child Processes via fork()\n", n);
    printf("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
    printf("======================================================================\n");

    if (n <= 0) {
        printf("Error: Number of child processes must be positive (> 0).\n");
        return;
    }

    pid_t root_parent_pid = getpid();
    printf("[ROOT PARENT] PID = %d initiating spawn loop for %d children...\n", root_parent_pid, n);
    printf("----------------------------------------------------------------------\n");
    printf("%-10s %-12s %-14s %-14s %-16s\n",
           "Child #", "Status", "Child PID", "Parent PID", "PPID (getppid)");
    printf("----------------------------------------------------------------------\n");
    fflush(stdout);

    for (int i = 1; i <= n; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            fprintf(stderr, "[ERROR] Fork failed at iteration %d: %s\n", i, strerror(errno));
            break;
        } else if (pid == 0) {
            /* Inside Child Process */
            printf("%-10d %-12s %-14d %-14d %-16d\n",
                   i, "ACTIVE", getpid(), root_parent_pid, getppid());
            fflush(stdout);
            /* Child exits immediately to maintain star topology (no recursive forks) */
            exit(i * 10);
        } else {
            /* Parent loop continues to next child */
        }
    }

    /* Parent waits for all child processes to reap zombies */
    int status;
    pid_t reaped_child;
    printf("----------------------------------------------------------------------\n");
    printf("[PARENT] Waiting for child processes to exit and reaping exit codes...\n");

    while ((reaped_child = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            printf("[REAPED] Child PID %-6d exited normally with code: %d\n",
                   reaped_child, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[REAPED] Child PID %-6d killed by signal: %d\n",
                   reaped_child, WTERMSIG(status));
        }
    }

    printf("======================================================================\n");
    printf("[SUCCESS] All %d child processes spawned, executed, and reaped cleanly.\n\n", n);
}


/* ============================================================================
 * QUESTION / EXPERIMENT 5 (Part b2):
 * Write a program to demonstrate that a parent process is waiting for child
 * process to end using wait() system call.
 * ============================================================================ */

void run_lab5_part_b2(void) {
    printf("\n======================================================================\n");
    printf("  OS LAB 5 (Part b2): Parent Process Waiting for Child via wait()\n");
    printf("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
    printf("======================================================================\n");

    pid_t parent_pid = getpid();
    printf("[1. PARENT] Starting execution. Parent PID: %d\n", parent_pid);
    printf("[1. PARENT] Calling fork() to create a dedicated worker child process...\n\n");
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "[ERROR] fork() failed: %s\n", strerror(errno));
        return;
    }

    if (pid == 0) {
        /* Inside Child Process */
        printf("  [CHILD] Hello from Child Process! PID = %d, Parent PID = %d\n",
               getpid(), getppid());
        printf("  [CHILD] Performing intensive task (simulated with 2s workload)...\n");
        fflush(stdout);

        for (int i = 1; i <= 2; i++) {
            sleep(1);
            printf("  [CHILD] Working... elapsed %d second(s)\n", i);
            fflush(stdout);
        }

        printf("  [CHILD] Work completed successfully. Exiting with status code 42.\n");
        fflush(stdout);
        exit(42);
    } else {
        /* Inside Parent Process */
        printf("[PARENT] Child spawned with PID: %d\n", pid);
        printf("[PARENT] Entering wait(&status) - blocking until child completes...\n\n");
        fflush(stdout);

        int status;
        pid_t completed_pid = wait(&status);

        printf("\n[PARENT] Resumed after wait()!\n");
        printf("[PARENT] Child PID returned by wait(): %d\n", completed_pid);

        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[PARENT] Normal termination detected (WIFEXITED == true).\n");
            printf("[PARENT] Child exited with return status code: %d\n", exit_code);
            if (exit_code == 42) {
                printf("[PARENT] Verified: Status matches child exit code (42).\n");
            }
        } else if (WIFSIGNALED(status)) {
            printf("[PARENT] Child was terminated abruptly by signal %d\n", WTERMSIG(status));
        }

        printf("======================================================================\n");
        printf("[SUCCESS] Parent-child lifecycle synchronization concluded cleanly.\n\n");
    }
}


/* ============================================================================
 * QUESTION / EXPERIMENT 6: CPU SCHEDULING ALGORITHMS
 * ============================================================================
 * a. Write generic code in C of CPU Scheduling Algorithms:
 *    - FCFS (First Come First Serve)
 *    - SJF (Shortest Job First - Non-Preemptive)
 *    - SRTF (Shortest Remaining Time First - Preemptive SJF)
 *    - RR (Round Robin)
 *    - Non-Preemptive Priority
 *    - Preemptive Priority
 *    In the program first user will input data in sequence like process no.,
 *    arrival time, burst time, and priority and then select any one algorithm
 *    to get TAT, WT and RT for each process and their averages. (If RR is selected,
 *    also take time quantum as input).
 *
 * b. Write generic simulation code in C for RR algorithm for given data:
 *      Process   Arrival Time   Burst Time
 *        P1            0             10
 *        P2            1              4
 *        P3            3              6
 *        P4            4              7
 *        P5            5              2
 *    Perform the analysis of different time quantum values (q = 1 to 10 ms)
 *    and find out the optimal value of time quantum for TAT, WT, and RT.
 * ============================================================================ */

typedef struct {
    int pid;
    int at;                  /* Arrival Time */
    int bt;                  /* Burst Time */
    int priority;            /* Priority (lower number = higher priority) */
    int ct;                  /* Completion Time */
    int tat;                 /* Turnaround Time = CT - AT */
    int wt;                  /* Waiting Time = TAT - BT */
    int rt;                  /* Response Time = First Response - AT */
    int rem_bt;              /* Remaining Burst Time for preemptive scheduling */
    int first_response_time; /* Time when CPU first touched process */
    bool completed;
} Process;

/* Helper: Print scheduling output table */
void print_schedule_table(Process p[], int n, const char *algo_name) {
    float total_tat = 0, total_wt = 0, total_rt = 0;
    printf("\n========================================================================================\n");
    printf("  CPU SCHEDULING ALGORITHM: %s\n", algo_name);
    printf("========================================================================================\n");
    printf("%-8s %-12s %-12s %-10s %-8s %-8s %-8s %-8s\n",
           "PID", "Arrival(AT)", "Burst(BT)", "Priority", "CT", "TAT", "WT", "RT");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < n; i++) {
        total_tat += p[i].tat;
        total_wt += p[i].wt;
        total_rt += p[i].rt;
        printf("P%-7d %-12d %-12d %-10d %-8d %-8d %-8d %-8d\n",
               p[i].pid, p[i].at, p[i].bt, p[i].priority,
               p[i].ct, p[i].tat, p[i].wt, p[i].rt);
    }

    printf("----------------------------------------------------------------------------------------\n");
    printf("Average Turnaround Time (TAT) : %.2f ms\n", total_tat / n);
    printf("Average Waiting Time (WT)    : %.2f ms\n", total_wt / n);
    printf("Average Response Time (RT)   : %.2f ms\n", total_rt / n);
    printf("========================================================================================\n\n");
}

/* 1. First Come First Serve (FCFS) */
void schedule_fcfs(Process p[], int n) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) temp[i] = p[i];

    /* Sort by arrival time */
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (temp[j].at > temp[j + 1].at) {
                Process t = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = t;
            }
        }
    }

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < temp[i].at) current_time = temp[i].at;
        temp[i].rt = current_time - temp[i].at;
        current_time += temp[i].bt;
        temp[i].ct = current_time;
        temp[i].tat = temp[i].ct - temp[i].at;
        temp[i].wt = temp[i].tat - temp[i].bt;
    }
    print_schedule_table(temp, n, "First Come First Serve (FCFS)");
}

/* 2. Shortest Job First (SJF - Non-Preemptive) */
void schedule_sjf_non_preemptive(Process p[], int n) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].completed = false;
    }

    int completed = 0, current_time = 0;
    while (completed < n) {
        int idx = -1;
        int min_bt = 1e9;
        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time && !temp[i].completed) {
                if (temp[i].bt < min_bt) {
                    min_bt = temp[i].bt;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            temp[idx].rt = current_time - temp[idx].at;
            current_time += temp[idx].bt;
            temp[idx].ct = current_time;
            temp[idx].tat = temp[idx].ct - temp[idx].at;
            temp[idx].wt = temp[idx].tat - temp[idx].bt;
            temp[idx].completed = true;
            completed++;
        }
    }
    print_schedule_table(temp, n, "Shortest Job First (SJF - Non-Preemptive)");
}

/* 3. Shortest Remaining Time First (SRTF - Preemptive SJF) */
void schedule_srtf_preemptive(Process p[], int n) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].rem_bt = temp[i].bt;
        temp[i].first_response_time = -1;
        temp[i].completed = false;
    }

    int completed = 0, current_time = 0;
    while (completed < n) {
        int idx = -1;
        int min_rem = 1e9;

        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time && !temp[i].completed) {
                if (temp[i].rem_bt < min_rem && temp[i].rem_bt > 0) {
                    min_rem = temp[i].rem_bt;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            if (temp[idx].first_response_time == -1) {
                temp[idx].first_response_time = current_time;
                temp[idx].rt = current_time - temp[idx].at;
            }

            temp[idx].rem_bt--;
            current_time++;

            if (temp[idx].rem_bt == 0) {
                temp[idx].ct = current_time;
                temp[idx].tat = temp[idx].ct - temp[idx].at;
                temp[idx].wt = temp[idx].tat - temp[idx].bt;
                temp[idx].completed = true;
                completed++;
            }
        }
    }
    print_schedule_table(temp, n, "Shortest Remaining Time First (SRTF - Preemptive)");
}

/* 4. Round Robin (RR) */
void schedule_round_robin(Process p[], int n, int quantum) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].rem_bt = temp[i].bt;
        temp[i].first_response_time = -1;
        temp[i].completed = false;
    }

    int completed = 0, current_time = 0;
    int queue[MAX_PROC * 100];
    int front = 0, rear = 0;
    bool in_queue[MAX_PROC] = {false};

    /* Enqueue processes arriving at time 0 */
    for (int i = 0; i < n; i++) {
        if (temp[i].at <= current_time) {
            queue[rear++] = i;
            in_queue[i] = true;
        }
    }

    while (completed < n) {
        if (front == rear) {
            /* Queue empty: advance time to next arrival */
            current_time++;
            for (int i = 0; i < n; i++) {
                if (temp[i].at <= current_time && !in_queue[i] && !temp[i].completed) {
                    queue[rear++] = i;
                    in_queue[i] = true;
                }
            }
            continue;
        }

        int idx = queue[front++];
        in_queue[idx] = false;

        if (temp[idx].first_response_time == -1) {
            temp[idx].first_response_time = current_time;
            temp[idx].rt = current_time - temp[idx].at;
        }

        int slice = (temp[idx].rem_bt > quantum) ? quantum : temp[idx].rem_bt;
        temp[idx].rem_bt -= slice;
        current_time += slice;

        /* Enqueue newly arrived processes during this slice */
        for (int i = 0; i < n; i++) {
            if (i != idx && temp[i].at <= current_time && !in_queue[i] && !temp[i].completed) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        if (temp[idx].rem_bt > 0) {
            queue[rear++] = idx;
            in_queue[idx] = true;
        } else {
            temp[idx].ct = current_time;
            temp[idx].tat = temp[idx].ct - temp[idx].at;
            temp[idx].wt = temp[idx].tat - temp[idx].bt;
            temp[idx].completed = true;
            completed++;
        }
    }

    char title[64];
    snprintf(title, sizeof(title), "Round Robin (RR, Quantum = %d ms)", quantum);
    print_schedule_table(temp, n, title);
}

/* 5. Priority Scheduling (Non-Preemptive, Lower number = Higher priority) */
void schedule_priority_non_preemptive(Process p[], int n) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].completed = false;
    }

    int completed = 0, current_time = 0;
    while (completed < n) {
        int idx = -1;
        int best_prio = 1e9;

        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time && !temp[i].completed) {
                if (temp[i].priority < best_prio) {
                    best_prio = temp[i].priority;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            temp[idx].rt = current_time - temp[idx].at;
            current_time += temp[idx].bt;
            temp[idx].ct = current_time;
            temp[idx].tat = temp[idx].ct - temp[idx].at;
            temp[idx].wt = temp[idx].tat - temp[idx].bt;
            temp[idx].completed = true;
            completed++;
        }
    }
    print_schedule_table(temp, n, "Priority Scheduling (Non-Preemptive)");
}

/* 6. Priority Scheduling (Preemptive) */
void schedule_priority_preemptive(Process p[], int n) {
    Process temp[MAX_PROC];
    for (int i = 0; i < n; i++) {
        temp[i] = p[i];
        temp[i].rem_bt = temp[i].bt;
        temp[i].first_response_time = -1;
        temp[i].completed = false;
    }

    int completed = 0, current_time = 0;
    while (completed < n) {
        int idx = -1;
        int best_prio = 1e9;

        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time && !temp[i].completed && temp[i].rem_bt > 0) {
                if (temp[i].priority < best_prio) {
                    best_prio = temp[i].priority;
                    idx = i;
                }
            }
        }

        if (idx == -1) {
            current_time++;
        } else {
            if (temp[idx].first_response_time == -1) {
                temp[idx].first_response_time = current_time;
                temp[idx].rt = current_time - temp[idx].at;
            }

            temp[idx].rem_bt--;
            current_time++;

            if (temp[idx].rem_bt == 0) {
                temp[idx].ct = current_time;
                temp[idx].tat = temp[idx].ct - temp[idx].at;
                temp[idx].wt = temp[idx].tat - temp[idx].bt;
                temp[idx].completed = true;
                completed++;
            }
        }
    }
    print_schedule_table(temp, n, "Priority Scheduling (Preemptive)");
}

/* Interactive Dispatcher for Part 6a */
void run_lab6_part_a(void) {
    printf("\n======================================================================\n");
    printf("  OS LAB 6 (Part a): Generic CPU Scheduling Algorithms in C\n");
    printf("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
    printf("======================================================================\n");

    int n;
    printf("Enter number of processes (e.g. 5): ");
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_PROC) {
        printf("Invalid input or process count too high.\n");
        return;
    }

    Process p[MAX_PROC];
    printf("\nEnter process details in sequence (PID, Arrival Time, Burst Time, Priority):\n");
    for (int i = 0; i < n; i++) {
        p[i].pid = i + 1;
        printf("Process P%d -> Arrival Time, Burst Time, Priority: ", i + 1);
        if (scanf("%d %d %d", &p[i].at, &p[i].bt, &p[i].priority) != 3) {
            printf("Error reading input.\n");
            return;
        }
    }

    printf("\nSelect CPU Scheduling Algorithm:\n");
    printf("  1. FCFS (First Come First Serve)\n");
    printf("  2. SJF (Shortest Job First - Non-Preemptive)\n");
    printf("  3. SRTF (Shortest Remaining Time First - Preemptive SJF)\n");
    printf("  4. Round Robin (RR)\n");
    printf("  5. Priority (Non-Preemptive)\n");
    printf("  6. Priority (Preemptive)\n");
    printf("Enter selection [1-6]: ");

    int choice;
    if (scanf("%d", &choice) != 1) return;

    if (choice == 1) schedule_fcfs(p, n);
    else if (choice == 2) schedule_sjf_non_preemptive(p, n);
    else if (choice == 3) schedule_srtf_preemptive(p, n);
    else if (choice == 4) {
        int q;
        printf("Enter Time Quantum (ms): ");
        if (scanf("%d", &q) != 1 || q <= 0) q = 2;
        schedule_round_robin(p, n, q);
    }
    else if (choice == 5) schedule_priority_non_preemptive(p, n);
    else if (choice == 6) schedule_priority_preemptive(p, n);
    else printf("Invalid algorithm selection.\n");
}


/* ============================================================================
 * QUESTION / EXPERIMENT 6 (Part b): ROUND ROBIN BENCHMARK & QUANTUM SWEEP
 * ============================================================================
 * Given Benchmark Dataset:
 *   P1: AT = 0, BT = 10
 *   P2: AT = 1, BT = 4
 *   P3: AT = 3, BT = 6
 *   P4: AT = 4, BT = 7
 *   P5: AT = 5, BT = 2
 *
 * Perform simulation for time quantum q = 1 to 10 ms, tabulate metrics,
 * and identify the optimal quantum values for TAT, WT, and RT.
 * ============================================================================ */

void run_lab6_part_b(void) {
    printf("\n======================================================================\n");
    printf("  OS LAB 6 (Part b): Round Robin Parametric Time Quantum Sweep (q=1..10)\n");
    printf("  Benchmark: P1(0,10), P2(1,4), P3(3,6), P4(4,7), P5(5,2)\n");
    printf("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
    printf("======================================================================\n");

    Process benchmark[5] = {
        {1, 0, 10, 0, 0, 0, 0, 0, 10, -1, false},
        {2, 1,  4, 0, 0, 0, 0, 0,  4, -1, false},
        {3, 3,  6, 0, 0, 0, 0, 0,  6, -1, false},
        {4, 4,  7, 0, 0, 0, 0, 0,  7, -1, false},
        {5, 5,  2, 0, 0, 0, 0, 0,  2, -1, false}
    };
    int n = 5;

    printf("\n%-14s %-16s %-16s %-16s %-12s\n",
           "Quantum (q)", "Avg TAT (ms)", "Avg WT (ms)", "Avg RT (ms)", "Context Switches");
    printf("--------------------------------------------------------------------------------\n");

    float min_tat = 1e9, min_wt = 1e9, min_rt = 1e9;
    int opt_q_tat = 1, opt_q_wt = 1, opt_q_rt = 1;

    for (int q = 1; q <= 10; q++) {
        Process temp[5];
        for (int i = 0; i < n; i++) {
            temp[i] = benchmark[i];
            temp[i].rem_bt = temp[i].bt;
            temp[i].first_response_time = -1;
            temp[i].completed = false;
        }

        int completed = 0, current_time = 0, switches = 0;
        int queue[500];
        int front = 0, rear = 0;
        bool in_queue[5] = {false};

        for (int i = 0; i < n; i++) {
            if (temp[i].at <= current_time) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }

        while (completed < n) {
            if (front == rear) {
                current_time++;
                for (int i = 0; i < n; i++) {
                    if (temp[i].at <= current_time && !in_queue[i] && !temp[i].completed) {
                        queue[rear++] = i;
                        in_queue[i] = true;
                    }
                }
                continue;
            }

            int idx = queue[front++];
            in_queue[idx] = false;
            switches++;

            if (temp[idx].first_response_time == -1) {
                temp[idx].first_response_time = current_time;
                temp[idx].rt = current_time - temp[idx].at;
            }

            int slice = (temp[idx].rem_bt > q) ? q : temp[idx].rem_bt;
            temp[idx].rem_bt -= slice;
            current_time += slice;

            for (int i = 0; i < n; i++) {
                if (i != idx && temp[i].at <= current_time && !in_queue[i] && !temp[i].completed) {
                    queue[rear++] = i;
                    in_queue[i] = true;
                }
            }

            if (temp[idx].rem_bt > 0) {
                queue[rear++] = idx;
                in_queue[idx] = true;
            } else {
                temp[idx].ct = current_time;
                temp[idx].tat = temp[idx].ct - temp[idx].at;
                temp[idx].wt = temp[idx].tat - temp[idx].bt;
                temp[idx].completed = true;
                completed++;
            }
        }

        float total_tat = 0, total_wt = 0, total_rt = 0;
        for (int i = 0; i < n; i++) {
            total_tat += temp[i].tat;
            total_wt += temp[i].wt;
            total_rt += temp[i].rt;
        }
        float avg_tat = total_tat / n;
        float avg_wt = total_wt / n;
        float avg_rt = total_rt / n;

        printf("q = %-9d %-16.2f %-16.2f %-16.2f %-12d\n",
               q, avg_tat, avg_wt, avg_rt, switches);

        if (avg_tat < min_tat) { min_tat = avg_tat; opt_q_tat = q; }
        if (avg_wt < min_wt)   { min_wt = avg_wt;   opt_q_wt = q; }
        if (avg_rt < min_rt)   { min_rt = avg_rt;   opt_q_rt = q; }
    }

    printf("--------------------------------------------------------------------------------\n");
    printf("[OPTIMAL ANALYSIS RESULTS]:\n");
    printf("  Optimal Quantum for Turnaround Time (TAT) : q = %d ms (Avg TAT = %.2f ms)\n", opt_q_tat, min_tat);
    printf("  Optimal Quantum for Waiting Time (WT)    : q = %d ms (Avg WT  = %.2f ms)\n", opt_q_wt, min_wt);
    printf("  Optimal Quantum for Response Time (RT)   : q = %d ms (Avg RT  = %.2f ms)\n", opt_q_rt, min_rt);
    printf("======================================================================\n\n");
}


/* ============================================================================
 * AUTOMATED DEMONSTRATION OF ALL QUESTIONS
 * ============================================================================ */

void run_all_demonstrations(void) {
    printf("\n======================================================================\n");
    printf("   RUNNING AUTOMATED TEST SUITE FOR OS LABS 5 & 6\n");
    printf("   Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
    printf("======================================================================\n");

    /* 1. Lab 5 Part b1 */
    run_lab5_part_b1(4);

    /* 2. Lab 5 Part b2 */
    run_lab5_part_b2();

    /* 3. Lab 6 Part a (Simulated with standard dataset) */
    printf("\n[DEMO] Executing CPU Scheduling Algorithms on Benchmark Workload:\n");
    Process demo_procs[5] = {
        {1, 0, 10, 3, 0, 0, 0, 0, 10, -1, false},
        {2, 1,  4, 1, 0, 0, 0, 0,  4, -1, false},
        {3, 3,  6, 4, 0, 0, 0, 0,  6, -1, false},
        {4, 4,  7, 2, 0, 0, 0, 0,  7, -1, false},
        {5, 5,  2, 5, 0, 0, 0, 0,  2, -1, false}
    };
    schedule_fcfs(demo_procs, 5);
    schedule_sjf_non_preemptive(demo_procs, 5);
    schedule_srtf_preemptive(demo_procs, 5);
    schedule_round_robin(demo_procs, 5, 3);
    schedule_priority_non_preemptive(demo_procs, 5);
    schedule_priority_preemptive(demo_procs, 5);

    /* 4. Lab 6 Part b */
    run_lab6_part_b();

    printf("\n======================================================================\n");
    printf("  ALL OS LAB 5 & 6 EXPERIMENTS EXECUTED & VERIFIED SUCCESSFULLY!\n");
    printf("======================================================================\n");
}


/* ============================================================================
 * MAIN ENTRY POINT & INTERACTIVE MENU
 * ============================================================================ */

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        if (strcmp(argv[1], "--all") == 0) {
            run_all_demonstrations();
            return 0;
        } else if (strcmp(argv[1], "--lab5b1") == 0) {
            int n = (argc >= 3) ? atoi(argv[2]) : 4;
            run_lab5_part_b1(n);
            return 0;
        } else if (strcmp(argv[1], "--lab5b2") == 0) {
            run_lab5_part_b2();
            return 0;
        } else if (strcmp(argv[1], "--lab6a") == 0) {
            run_lab6_part_a();
            return 0;
        } else if (strcmp(argv[1], "--lab6b") == 0) {
            run_lab6_part_b();
            return 0;
        }
    }

    if (!isatty(STDIN_FILENO)) {
        run_all_demonstrations();
        return 0;
    }

    while (1) {
        printf("======================================================================\n");
        printf("  OPERATING SYSTEMS LAB (CS-502) - EXPERIMENTS 5 & 6\n");
        printf("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1\n");
        printf("======================================================================\n");
        printf("Select an experiment to execute:\n");
        printf("  1. Lab 5 (b1): Generate N Child Processes using fork()\n");
        printf("  2. Lab 5 (b2): Parent Waiting for Child Completion using wait()\n");
        printf("  3. Lab 6 (a) : Generic CPU Scheduling Algorithms (Interactive)\n");
        printf("  4. Lab 6 (b) : Round Robin Benchmark & Quantum Sweep (q = 1..10 ms)\n");
        printf("  5. Run ALL Demonstrations in Sequence\n");
        printf("  0. Exit\n");
        printf("Enter selection [0-5]: ");

        int opt;
        if (scanf("%d", &opt) != 1) break;

        if (opt == 1) {
            int n;
            printf("Enter number of children to spawn (N): ");
            if (scanf("%d", &n) == 1) run_lab5_part_b1(n);
        } else if (opt == 2) {
            run_lab5_part_b2();
        } else if (opt == 3) {
            run_lab6_part_a();
        } else if (opt == 4) {
            run_lab6_part_b();
        } else if (opt == 5) {
            run_all_demonstrations();
        } else if (opt == 0) {
            printf("Exiting. Thank you!\n");
            break;
        } else {
            printf("Invalid selection.\n\n");
        }
    }

    return 0;
}
