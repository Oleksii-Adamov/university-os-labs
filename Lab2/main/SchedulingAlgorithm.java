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

  public static Results Run(int runtime, Vector processVector, Results result) {
    int i = 0;
    int comptime = 0;
    ProcessTimeEstimate currentProcess;
    ProcessTimeEstimate previousProcess;
    int size = processVector.size();
    int completed = 0;
    int curProcessTime = 0;
    String resultsFile = "Summary-Processes";
    result.schedulingType = "Interactive";
    result.schedulingName = "Shortest process next";
    double coef = 0.5;

    try {
      if (size > 0) {
        // initialazing every process with zero time estimate, and picking currentProcess
        PriorityQueue<ProcessTimeEstimate> processesTimeEstimates = new PriorityQueue<>(
                Comparator.comparingDouble((ProcessTimeEstimate processTimeEstimate) -> processTimeEstimate.timeEstimate));
        for (int processIndex = 0; processIndex < size; processIndex++) {
          processesTimeEstimates.add(new ProcessTimeEstimate(processIndex, 0));
        }
        currentProcess = Objects.requireNonNull(processesTimeEstimates.poll());

        //BufferedWriter out = new BufferedWriter(new FileWriter(resultsFile));
        //OutputStream out = new FileOutputStream(resultsFile);
        PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
        sProcess process = (sProcess) processVector.elementAt(currentProcess.processIndex);
        out.println("main.Process: " + currentProcess.processIndex + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
        while (comptime < runtime) {
          if (process.cpudone == process.cputime) {
            completed++;
            out.println("main.Process: " + currentProcess.processIndex + " completed... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
            if (completed == size) {
              result.compuTime = comptime;
              out.close();
              return result;
            }
            // choose process with minimum time estimate
            curProcessTime = 0;
            currentProcess = Objects.requireNonNull(processesTimeEstimates.poll());
            process = (sProcess) processVector.elementAt(currentProcess.processIndex);
            out.println("main.Process: " + currentProcess.processIndex + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
          }
          if (process.ioblocking == process.ionext) {
            out.println("main.Process: " + currentProcess.processIndex + " I/O blocked... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
            process.numblocked++;
            process.ionext = 0;
            // update estimate
            currentProcess.timeEstimate = coef * currentProcess.timeEstimate +  (1 - coef) * curProcessTime;
            curProcessTime = 0;
            if (!processesTimeEstimates.isEmpty()) {
              // choose process with minimum time estimate
              previousProcess = currentProcess;
              currentProcess = Objects.requireNonNull(processesTimeEstimates.poll());
              process = (sProcess) processVector.elementAt(currentProcess.processIndex);
              // add previous process to scheduling
              processesTimeEstimates.add(previousProcess);
            }
            out.println("main.Process: " + currentProcess.processIndex + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
          }
          process.cpudone++;
          if (process.ioblocking > 0) {
            process.ionext++;
          }
          curProcessTime++;
          comptime++;
        }
        out.close();
      }
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    result.compuTime = comptime;
    return result;
  }
}
