package main;// Run() is called from main.Scheduling.main() and is where
// the scheduling algorithm written by the user resides.
// User modification should occur within the Run() function.

import java.util.Comparator;
import java.util.Objects;
import java.util.PriorityQueue;
import java.util.Vector;
import java.io.*;

public class SchedulingAlgorithm {

  private SchedulingAlgorithm() {}

  public static String processActionInfo(sProcess process, String action) {
    return "main.Process: " + process.id + " " + action + " (" + process.processInfo() + ")";
  }

  public static Results run(int runtime, Vector<sProcess> processVector, Results result, double agingCoef) {
    int curTime = 0;
    sProcess curProcess = null;
    int numProcesses = processVector.size();
    int numCompletedProcesses = 0;
    int curProcessTime = 0;
    boolean sheduleNextProcess = true;
    String resultsFile = "Summary-Processes";
    result.schedulingType = "Interactive (Nonpreemptive)";
    result.schedulingName = "Shortest process next, agingCoef = " + agingCoef;

    try {
        // initialazing every process with zero time estimate
        PriorityQueue<sProcess> processesHeap = new PriorityQueue<>(
                Comparator.comparingDouble((sProcess process) -> process.timeEstimate));

        PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
        // runtime cycle
        while (curTime < runtime) {
          // checking arrival and unblocking
          for (int processIndex = 0; processIndex < numProcesses; processIndex++) {
            boolean addToSheduling = false;
            sProcess checkedProcess = processVector.elementAt(processIndex);
            if (checkedProcess.arrivalTime == curTime) {
              addToSheduling = true;
              out.println(processActionInfo(checkedProcess, "arrived and added to scheduling"));
            }
            if (checkedProcess.isBlocked && curTime - checkedProcess.lastTimeExecuted == checkedProcess.blockDuration) {
              checkedProcess.isBlocked = false;
              addToSheduling = true;
              out.println(processActionInfo(checkedProcess, "unblocked and added to scheduling"));
            }
            if (addToSheduling) {
              processesHeap.add(checkedProcess);
            }
          }

          // scheduling new process if needed
          if (sheduleNextProcess && !processesHeap.isEmpty()) {
            // choose process with minimum time estimate
            curProcess = processesHeap.poll();
            sheduleNextProcess = false;
            out.println(processActionInfo(curProcess, "registered"));
          }

          // process "executing"
          curProcess.cpuDone++;
          if (!curProcess.isBlocked && curProcess.runTimeBeforeBlocking > 0) {
            curProcess.ioNext++;
          }
          curProcessTime++;
          curTime++;

          // if process completed
          if (curProcess.cpuDone == curProcess.cpuTime) {
            out.println(processActionInfo(curProcess, "completed"));
            numCompletedProcesses++;
            if (numCompletedProcesses == numProcesses) break;
            curProcessTime = 0;
            sheduleNextProcess = true;
          }

          // if process blocked for I/O
          if (curProcess.runTimeBeforeBlocking == curProcess.ioNext) {
            curProcess.numBlocked++;
            curProcess.isBlocked = true;
            curProcess.ioNext = 0;
            curProcess.lastTimeExecuted = curTime;
            out.println(processActionInfo(curProcess, "I/O blocked"));
            // update estimate
            curProcess.timeEstimate = agingCoef * curProcess.timeEstimate +  (1 - agingCoef) * curProcessTime;
            out.println("Process " + curProcess.id + " new time estimate: " + curProcess.timeEstimate);
            curProcessTime = 0;
            sheduleNextProcess = true;
          }
        }
        out.close();
    } catch (IOException e) {
      e.printStackTrace();
      return result;
    }
    result.compuTime = curTime;
    return result;
  }
}
