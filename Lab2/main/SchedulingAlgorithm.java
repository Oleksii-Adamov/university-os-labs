package main;// Run() is called from main.Scheduling.main() and is where
// the scheduling algorithm written by the user resides.
// User modification should occur within the Run() function.

import main.ProcessTimeEstimate;
import main.Results;
import main.sProcess;

import java.util.Comparator;
import java.util.Objects;
import java.util.PriorityQueue;
import java.util.Vector;
import java.io.*;

public class SchedulingAlgorithm {

  public static Results Run(int runtime, Vector processVector, Results result, double agingCoef) {
    int comptime = 0;
    ProcessTimeEstimate currentProcess = null;
    ProcessTimeEstimate previousProcess = null;
    sProcess process = null;
    int size = processVector.size();
    int curProcessTime = 0;
    boolean sheduleNextProcess = true;
    String resultsFile = "Summary-Processes";
    result.schedulingType = "Interactive (Nonpreemptive)";
    result.schedulingName = "Shortest process next, agingCoef = " + agingCoef;

    try {
        // initialazing every process with zero time estimate
        PriorityQueue<ProcessTimeEstimate> processesTimeEstimates = new PriorityQueue<>(
                Comparator.comparingDouble((ProcessTimeEstimate processTimeEstimate) -> processTimeEstimate.timeEstimate));
        for (int processIndex = 0; processIndex < size; processIndex++) {
          processesTimeEstimates.add(new ProcessTimeEstimate(processIndex, 0));
        }

        PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
        // runtime cycle
        while (comptime < runtime) {
          if (sheduleNextProcess) {
            // if no processes left - break
            if (processesTimeEstimates.isEmpty()) break;
            // choose process with minimum time estimate
            previousProcess = currentProcess;
            currentProcess = Objects.requireNonNull(processesTimeEstimates.poll());
            process = (sProcess) processVector.elementAt(currentProcess.processIndex);
            // add previous process to scheduling
            if (previousProcess != null) {
              processesTimeEstimates.add(previousProcess);
            }
            sheduleNextProcess = false;
            out.println("main.Process: " + currentProcess.processIndex + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.numblocked + ")");
          }
          // process "executing"
          process.cpudone++;
          if (process.ioblocking > 0) {
            process.ionext++;
          }
          curProcessTime++;
          comptime++;
          // if process completed
          if (process.cpudone == process.cputime) {
            out.println("main.Process: " + currentProcess.processIndex + " completed... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.numblocked + ")");
            curProcessTime = 0;
            currentProcess = null;
            sheduleNextProcess = true;
          }
          // if process blocked for I/O
          if (process.ioblocking == process.ionext) {
            process.numblocked++;
            process.ionext = 0;
            out.println("main.Process: " + currentProcess.processIndex + " I/O blocked... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.numblocked + ")");
            // update estimate
            currentProcess.timeEstimate = agingCoef * currentProcess.timeEstimate +  (1 - agingCoef) * curProcessTime;
            //System.out.println("Process " + currentProcess.processIndex + " time estimate: " + currentProcess.timeEstimate);
            curProcessTime = 0;
            if (!processesTimeEstimates.isEmpty()) {
              sheduleNextProcess = true;
            }
            else {
              out.println("main.Process: " + currentProcess.processIndex + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.numblocked + ")");
            }
          }
        }
        out.close();
    } catch (IOException e) {
      e.printStackTrace();
      return result;
    }
    result.compuTime = comptime;
    return result;
  }
}
