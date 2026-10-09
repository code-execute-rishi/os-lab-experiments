#!/usr/bin/env python3
"""
================================================================================
   OPERATING SYSTEMS LABORATORY (CS-502) - EXPERIMENTS 5 & 6
   Student Name   : Aradhya Jain
   Scholar Number : 24112011111
   Section        : CSE-1
   Semester       : V Semester, B.Tech CSE
   Institution    : Maulana Azad National Institute of Technology (MANIT)
================================================================================

DESCRIPTION:
All-in-one standalone, object-oriented Python source code for OS Labs 5 and 6.

TABLE OF CONTENTS:
  1. QUESTION / EXPERIMENT 5:
     - Process management system calls (fork, exec, wait).
     - Class Lab5_ProcessManagement implementing:
       * b1: Generating N child processes via fork() with PID/PPID logging.
       * b2: Parent waiting for child termination with waitpid() and exit code decoding.
  2. QUESTION / EXPERIMENT 6 (Part a):
     - CPU Scheduling Algorithms (FCFS, SJF, SRTF, RR, Priority NP, Priority Preemptive).
     - Class Lab6_CPUScheduler calculating CT, TAT, WT, RT, and averages.
  3. QUESTION / EXPERIMENT 6 (Part b):
     - Parametric Round Robin simulation sweep (q = 1 to 10 ms) on benchmark workload.
     - Class Lab6_RoundRobinQuantumSweep finding optimal quantum for TAT, WT, RT.
  4. Interactive CLI Menu & Unified Test Harness.
================================================================================
"""

import os
import sys
import time
import argparse
from dataclasses import dataclass
from typing import List, Dict, Optional


# ==============================================================================
# QUESTION / EXPERIMENT 5: PROCESS MANAGEMENT SYSTEM CALLS
# ==============================================================================
# a1. Write brief introduction of fork(), exec(), and wait() system calls with
#     their input parameters and return types:
#     - fork():
#         Input: void.
#         Return: 0 to child, Child PID to parent, -1 on failure.
#         Semantics: Clones current process address space using Copy-on-Write.
#     - exec() family:
#         Input: Executable path, argument list/vector, optional environment.
#         Return: Does NOT return on success; -1 on failure.
#         Semantics: Overlays current process image with new program binary.
#     - wait() / waitpid():
#         Input: Status pointer, options.
#         Return: Terminated child PID on success, -1 on failure.
#         Semantics: Blocks caller until child state changes; reaps zombie PCB.
#
# a2. List out the different versions of exec() system call:
#     1. execl()  : List of arguments, full path.
#     2. execv()  : Vector (array) of arguments, full path.
#     3. execlp() : List of arguments, searches PATH environment variable.
#     4. execvp() : Vector of arguments, searches PATH environment variable.
#     5. execle() : List of arguments, custom environment array.
#     6. execve() : Vector of arguments, custom environment array (True syscall).
#
# b1. Write a program to take integer from user as input and generate same number
#     of child process with fork() system call, also print their PID along with parent PID.
#
# b2. Write a program to demonstrate that a parent process is waiting for child
#     process to end.
# ==============================================================================

class Lab5_ProcessManagement:
    """
    Demonstrates Process Management system calls (fork, wait, process hierarchies)
    using POSIX OS primitives.
    """

    @staticmethod
    def fork_n_children(n: int):
        print(f"\n======================================================================")
        print(f"  OS LAB 5 (Part b1): Spawning {n} Child Processes via fork()")
        print(f"  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1")
        print(f"======================================================================")

        if not hasattr(os, "fork"):
            print("[INFO] os.fork() is not available on Windows. Simulating process tree:")
            parent_pid = os.getpid()
            print(f"[ROOT PARENT] PID = {parent_pid}")
            for i in range(1, n + 1):
                sim_pid = parent_pid + i
                print(f"Child #{i:<5} ACTIVE       Child PID: {sim_pid:<8} Parent PID: {parent_pid}")
            print(f"[SUCCESS] All {n} simulated children finished.\n")
            return

        parent_pid = os.getpid()
        print(f"[ROOT PARENT] PID = {parent_pid} spawning {n} children...")
        print(f"{'Child #':<10} {'Status':<12} {'Child PID':<14} {'Parent PID':<14} {'PPID':<14}")
        print("-" * 66)
        sys.stdout.flush()

        for i in range(1, n + 1):
            pid = os.fork()
            if pid == 0:
                # Inside Child Process
                print(f"{i:<10} {'ACTIVE':<12} {os.getpid():<14} {parent_pid:<14} {os.getppid():<14}")
                sys.stdout.flush()
                # Child exits immediately to avoid recursive forks
                os._exit(i * 10)
            else:
                pass

        # Parent reaps all children
        print("-" * 66)
        print("[PARENT] Waiting for children to exit and harvesting exit codes...")
        while True:
            try:
                reaped_pid, status = os.wait()
                if os.WIFEXITED(status):
                    print(f"[REAPED] Child PID {reaped_pid:<6} exited with code: {os.WEXITSTATUS(status)}")
            except ChildProcessError:
                break
        print(f"[SUCCESS] All {n} child processes spawned and reaped cleanly.\n")

    @staticmethod
    def parent_wait_child():
        print(f"\n======================================================================")
        print(f"  OS LAB 5 (Part b2): Parent Waiting for Child Completion via wait()")
        print(f"  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1")
        print(f"======================================================================")

        if not hasattr(os, "fork"):
            print("[INFO] os.fork() not available on Windows. Emulating synchronization:")
            print(f"[PARENT] Parent PID {os.getpid()} spawned Child PID {os.getpid()+1}")
            print("[PARENT] Blocking inside wait(&status)...")
            time.sleep(1)
            print("[CHILD] Completed task. Exiting with exit code 42.")
            print("[PARENT] Resumed! Child exited normally with code 42.\n")
            return

        parent_pid = os.getpid()
        print(f"[PARENT] PID: {parent_pid} creating worker child via fork()...")
        sys.stdout.flush()

        pid = os.fork()
        if pid == 0:
            # Child task
            print(f"  [CHILD] PID: {os.getpid()}, Parent PID: {os.getppid()}. Working (2s sleep)...")
            sys.stdout.flush()
            time.sleep(2)
            print("  [CHILD] Work completed! Terminating with exit code 42.")
            sys.stdout.flush()
            os._exit(42)
        else:
            # Parent blocks in wait
            print(f"[PARENT] Spawned child PID: {pid}. Entering wait()...")
            sys.stdout.flush()

            reaped_pid, status = os.waitpid(pid, 0)
            print(f"\n[PARENT] Resumed after wait()! Child PID {reaped_pid} terminated.")
            if os.WIFEXITED(status):
                code = os.WEXITSTATUS(status)
                print(f"[PARENT] Normal exit verified. Exit status code: {code}")
            print("[SUCCESS] Process synchronization complete.\n")


# ==============================================================================
# QUESTION / EXPERIMENT 6: CPU SCHEDULING ALGORITHMS
# ==============================================================================
# a. Write generic code in C of CPU Scheduling Algorithms:
#    - FCFS, SJF (Non-preemptive), SRTF (Preemptive SJF), Round Robin,
#      Non-preemptive Priority, Preemptive Priority.
#    Input: Process No, Arrival Time, Burst Time, Priority.
#    Output: CT, TAT, WT, RT for each process and their averages.
#
# b. Round Robin Simulation & Optimal Quantum Sweep:
#    Benchmark: P1(0,10), P2(1,4), P3(3,6), P4(4,7), P5(5,2).
#    Sweep q = 1 to 10 ms, analyze TAT, WT, RT, and identify optimal quantum.
# ==============================================================================

@dataclass
class ProcessItem:
    pid: int
    at: int       # Arrival Time
    bt: int       # Burst Time
    priority: int # Priority (lower value = higher priority)
    ct: int = 0   # Completion Time
    tat: int = 0  # Turnaround Time
    wt: int = 0   # Waiting Time
    rt: int = 0   # Response Time
    rem_bt: int = 0
    first_response: int = -1
    completed: bool = False


class Lab6_CPUScheduler:
    """
    Implements 6 classical CPU Scheduling algorithms with full metric calculation.
    """

    @staticmethod
    def _print_table(procs: List[ProcessItem], algo_name: str):
        total_tat = sum(p.tat for p in procs)
        total_wt = sum(p.wt for p in procs)
        total_rt = sum(p.rt for p in procs)
        n = len(procs)

        print("\n" + "=" * 88)
        print(f"  CPU SCHEDULING ALGORITHM: {algo_name}")
        print("=" * 88)
        print(f"{'PID':<8} {'Arrival(AT)':<12} {'Burst(BT)':<12} {'Priority':<10} {'CT':<8} {'TAT':<8} {'WT':<8} {'RT':<8}")
        print("-" * 88)
        for p in procs:
            print(f"P{p.pid:<7} {p.at:<12} {p.bt:<12} {p.priority:<10} {p.ct:<8} {p.tat:<8} {p.wt:<8} {p.rt:<8}")
        print("-" * 88)
        print(f"Average Turnaround Time (TAT) : {total_tat / n:.2f} ms")
        print(f"Average Waiting Time (WT)    : {total_wt / n:.2f} ms")
        print(f"Average Response Time (RT)   : {total_rt / n:.2f} ms")
        print("=" * 88 + "\n")

    @classmethod
    def fcfs(cls, procs: List[ProcessItem]):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority) for p in procs]
        plist.sort(key=lambda x: x.at)
        curr = 0
        for p in plist:
            if curr < p.at:
                curr = p.at
            p.rt = curr - p.at
            curr += p.bt
            p.ct = curr
            p.tat = p.ct - p.at
            p.wt = p.tat - p.bt
        cls._print_table(plist, "First Come First Serve (FCFS)")

    @classmethod
    def sjf_non_preemptive(cls, procs: List[ProcessItem]):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority) for p in procs]
        completed = 0
        curr = 0
        n = len(plist)
        while completed < n:
            available = [p for p in plist if p.at <= curr and not p.completed]
            if not available:
                curr += 1
                continue
            best = min(available, key=lambda x: x.bt)
            best.rt = curr - best.at
            curr += best.bt
            best.ct = curr
            best.tat = best.ct - best.at
            best.wt = best.tat - best.bt
            best.completed = True
            completed += 1
        cls._print_table(plist, "Shortest Job First (SJF - Non-Preemptive)")

    @classmethod
    def srtf_preemptive(cls, procs: List[ProcessItem]):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority, rem_bt=p.bt) for p in procs]
        completed = 0
        curr = 0
        n = len(plist)
        while completed < n:
            available = [p for p in plist if p.at <= curr and not p.completed and p.rem_bt > 0]
            if not available:
                curr += 1
                continue
            best = min(available, key=lambda x: x.rem_bt)
            if best.first_response == -1:
                best.first_response = curr
                best.rt = curr - best.at
            best.rem_bt -= 1
            curr += 1
            if best.rem_bt == 0:
                best.ct = curr
                best.tat = best.ct - best.at
                best.wt = best.tat - best.bt
                best.completed = True
                completed += 1
        cls._print_table(plist, "Shortest Remaining Time First (SRTF - Preemptive)")

    @classmethod
    def round_robin(cls, procs: List[ProcessItem], quantum: int):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority, rem_bt=p.bt) for p in procs]
        plist.sort(key=lambda x: x.at)
        curr = 0
        completed = 0
        n = len(plist)
        queue = []
        in_queue = set()

        for p in plist:
            if p.at <= curr:
                queue.append(p)
                in_queue.add(p.pid)

        while completed < n:
            if not queue:
                curr += 1
                for p in plist:
                    if p.at <= curr and p.pid not in in_queue and not p.completed:
                        queue.append(p)
                        in_queue.add(p.pid)
                continue

            curr_proc = queue.pop(0)
            in_queue.remove(curr_proc.pid)

            if curr_proc.first_response == -1:
                curr_proc.first_response = curr
                curr_proc.rt = curr - curr_proc.at

            slice_time = min(quantum, curr_proc.rem_bt)
            curr_proc.rem_bt -= slice_time
            curr += slice_time

            for p in plist:
                if p.pid != curr_proc.pid and p.at <= curr and p.pid not in in_queue and not p.completed:
                    queue.append(p)
                    in_queue.add(p.pid)

            if curr_proc.rem_bt > 0:
                queue.append(curr_proc)
                in_queue.add(curr_proc.pid)
            else:
                curr_proc.ct = curr
                curr_proc.tat = curr_proc.ct - curr_proc.at
                curr_proc.wt = curr_proc.tat - curr_proc.bt
                curr_proc.completed = True
                completed += 1

        cls._print_table(plist, f"Round Robin (RR, Quantum = {quantum} ms)")

    @classmethod
    def priority_non_preemptive(cls, procs: List[ProcessItem]):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority) for p in procs]
        completed = 0
        curr = 0
        n = len(plist)
        while completed < n:
            available = [p for p in plist if p.at <= curr and not p.completed]
            if not available:
                curr += 1
                continue
            best = min(available, key=lambda x: x.priority)
            best.rt = curr - best.at
            curr += best.bt
            best.ct = curr
            best.tat = best.ct - best.at
            best.wt = best.tat - best.bt
            best.completed = True
            completed += 1
        cls._print_table(plist, "Priority Scheduling (Non-Preemptive)")

    @classmethod
    def priority_preemptive(cls, procs: List[ProcessItem]):
        plist = [ProcessItem(p.pid, p.at, p.bt, p.priority, rem_bt=p.bt) for p in procs]
        completed = 0
        curr = 0
        n = len(plist)
        while completed < n:
            available = [p for p in plist if p.at <= curr and not p.completed and p.rem_bt > 0]
            if not available:
                curr += 1
                continue
            best = min(available, key=lambda x: x.priority)
            if best.first_response == -1:
                best.first_response = curr
                best.rt = curr - best.at
            best.rem_bt -= 1
            curr += 1
            if best.rem_bt == 0:
                best.ct = curr
                best.tat = best.ct - best.at
                best.wt = best.tat - best.bt
                best.completed = True
                completed += 1
        cls._print_table(plist, "Priority Scheduling (Preemptive)")


class Lab6_RoundRobinQuantumSweep:
    """
    Parametric simulation of Round Robin on the benchmark dataset across q=1..10 ms.
    """

    BENCHMARK = [
        ProcessItem(1, 0, 10, 0),
        ProcessItem(2, 1,  4, 0),
        ProcessItem(3, 3,  6, 0),
        ProcessItem(4, 4,  7, 0),
        ProcessItem(5, 5,  2, 0)
    ]

    @classmethod
    def run_sweep(cls):
        print("\n" + "=" * 80)
        print("  OS LAB 6 (Part b): Round Robin Parametric Quantum Sweep (q = 1..10 ms)")
        print("  Benchmark Workload: P1(0,10), P2(1,4), P3(3,6), P4(4,7), P5(5,2)")
        print("=" * 80)
        print(f"{'Quantum (q)':<14} {'Avg TAT (ms)':<16} {'Avg WT (ms)':<16} {'Avg RT (ms)':<16} {'Context Switches':<12}")
        print("-" * 80)

        min_tat = float("inf")
        min_wt = float("inf")
        min_rt = float("inf")
        opt_tat_q = 1
        opt_wt_q = 1
        opt_rt_q = 1

        for q in range(1, 11):
            plist = [ProcessItem(p.pid, p.at, p.bt, p.priority, rem_bt=p.bt) for p in cls.BENCHMARK]
            curr = 0
            completed = 0
            switches = 0
            n = len(plist)
            queue = []
            in_queue = set()

            for p in plist:
                if p.at <= curr:
                    queue.append(p)
                    in_queue.add(p.pid)

            while completed < n:
                if not queue:
                    curr += 1
                    for p in plist:
                        if p.at <= curr and p.pid not in in_queue and not p.completed:
                            queue.append(p)
                            in_queue.add(p.pid)
                    continue

                curr_proc = queue.pop(0)
                in_queue.remove(curr_proc.pid)
                switches += 1

                if curr_proc.first_response == -1:
                    curr_proc.first_response = curr
                    curr_proc.rt = curr - curr_proc.at

                slice_time = min(q, curr_proc.rem_bt)
                curr_proc.rem_bt -= slice_time
                curr += slice_time

                for p in plist:
                    if p.pid != curr_proc.pid and p.at <= curr and p.pid not in in_queue and not p.completed:
                        queue.append(p)
                        in_queue.add(p.pid)

                if curr_proc.rem_bt > 0:
                    queue.append(curr_proc)
                    in_queue.add(curr_proc.pid)
                else:
                    curr_proc.ct = curr
                    curr_proc.tat = curr_proc.ct - curr_proc.at
                    curr_proc.wt = curr_proc.tat - curr_proc.bt
                    curr_proc.completed = True
                    completed += 1

            avg_tat = sum(p.tat for p in plist) / n
            avg_wt = sum(p.wt for p in plist) / n
            avg_rt = sum(p.rt for p in plist) / n

            print(f"q = {q:<9} {avg_tat:<16.2f} {avg_wt:<16.2f} {avg_rt:<16.2f} {switches:<12}")

            if avg_tat < min_tat:
                min_tat = avg_tat
                opt_tat_q = q
            if avg_wt < min_wt:
                min_wt = avg_wt
                opt_wt_q = q
            if avg_rt < min_rt:
                min_rt = avg_rt
                opt_rt_q = q

        print("-" * 80)
        print("[OPTIMAL PARAMETRIC VALUES]:")
        print(f"  Optimal Quantum for Turnaround Time (TAT) : q = {opt_tat_q} ms (Avg TAT = {min_tat:.2f} ms)")
        print(f"  Optimal Quantum for Waiting Time (WT)    : q = {opt_wt_q} ms (Avg WT  = {min_wt:.2f} ms)")
        print(f"  Optimal Quantum for Response Time (RT)   : q = {opt_rt_q} ms (Avg RT  = {min_rt:.2f} ms)")
        print("=" * 80 + "\n")


# ==============================================================================
# MAIN DISPATCHER & INTERACTIVE MENU
# ==============================================================================

def run_all_suite():
    print("\n" + "=" * 70)
    print("  RUNNING COMPLETE DEMONSTRATION SUITE FOR OS LABS 5 & 6")
    print("  Student: Aradhya Jain | Scholar No: 24112011111 | Section: CSE-1")
    print("=" * 70)

    # 1. Lab 5 b1
    Lab5_ProcessManagement.fork_n_children(4)

    # 2. Lab 5 b2
    Lab5_ProcessManagement.parent_wait_child()

    # 3. Lab 6 a (Benchmark procs)
    demo_procs = [
        ProcessItem(1, 0, 10, 3),
        ProcessItem(2, 1,  4, 1),
        ProcessItem(3, 3,  6, 4),
        ProcessItem(4, 4,  7, 2),
        ProcessItem(5, 5,  2, 5)
    ]
    Lab6_CPUScheduler.fcfs(demo_procs)
    Lab6_CPUScheduler.sjf_non_preemptive(demo_procs)
    Lab6_CPUScheduler.srtf_preemptive(demo_procs)
    Lab6_CPUScheduler.round_robin(demo_procs, 3)
    Lab6_CPUScheduler.priority_non_preemptive(demo_procs)
    Lab6_CPUScheduler.priority_preemptive(demo_procs)

    # 4. Lab 6 b
    Lab6_RoundRobinQuantumSweep.run_sweep()


def main():
    parser = argparse.ArgumentParser(description="OS Labs 5 & 6 Runner - Aradhya Jain")
    parser.add_argument("--all", action="store_true", help="Run all demonstrations")
    parser.add_argument("--lab5b1", type=int, default=0, help="Run Lab 5 b1 with N children")
    parser.add_argument("--lab5b2", action="store_true", help="Run Lab 5 b2 parent wait child")
    parser.add_argument("--lab6a", action="store_true", help="Run Lab 6 a scheduling demo")
    parser.add_argument("--lab6b", action="store_true", help="Run Lab 6 b Round Robin sweep")

    args = parser.parse_args()

    if args.all:
        run_all_suite()
    elif args.lab5b1 > 0:
        Lab5_ProcessManagement.fork_n_children(args.lab5b1)
    elif args.lab5b2:
        Lab5_ProcessManagement.parent_wait_child()
    elif args.lab6a:
        demo_procs = [
            ProcessItem(1, 0, 10, 3),
            ProcessItem(2, 1,  4, 1),
            ProcessItem(3, 3,  6, 4),
            ProcessItem(4, 4,  7, 2),
            ProcessItem(5, 5,  2, 5)
        ]
        Lab6_CPUScheduler.fcfs(demo_procs)
        Lab6_CPUScheduler.sjf_non_preemptive(demo_procs)
        Lab6_CPUScheduler.srtf_preemptive(demo_procs)
        Lab6_CPUScheduler.round_robin(demo_procs, 3)
        Lab6_CPUScheduler.priority_non_preemptive(demo_procs)
        Lab6_CPUScheduler.priority_preemptive(demo_procs)
    elif args.lab6b:
        Lab6_RoundRobinQuantumSweep.run_sweep()
    else:
        run_all_suite()


if __name__ == "__main__":
    main()
