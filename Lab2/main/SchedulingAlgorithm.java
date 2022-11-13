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
    int currentProcess = 0;
    int previousProcess = 0;
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
        for (int processIndex = 0; i < size; i++) {
          processesTimeEstimates.add(new ProcessTimeEstimate(processIndex, 0));
        }
        ProcessTimeEstimate currentProcessTimeEstimate = Objects.requireNonNull(processesTimeEstimates.poll());

        //BufferedWriter out = new BufferedWriter(new FileWriter(resultsFile));
        //OutputStream out = new FileOutputStream(resultsFile);
        PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
        sProcess process = (sProcess) processVector.elementAt(currentProcessTimeEstimate.processIndex);
        out.println("main.Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
        while (comptime < runtime) {
          if (process.cpudone == process.cputime) {
            completed++;
            out.println("main.Process: " + currentProcess + " completed... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
            if (completed == size) {
              result.compuTime = comptime;
              out.close();
              return result;
            }
            // choose process with minimum time estimate
            currentProcessTimeEstimate = Objects.requireNonNull(processesTimeEstimates.poll());
            process = (sProcess) processVector.elementAt(currentProcessTimeEstimate.processIndex);
            out.println("main.Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
          }
          if (process.ioblocking == process.ionext) {
            out.println("main.Process: " + currentProcess + " I/O blocked... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
            process.numblocked++;
            process.ionext = 0;
            currentProcessTimeEstimate.timeEstimate = coef * currentProcessTimeEstimate.timeEstimate +  (1 - coef) * curProcessTime;
            processesTimeEstimates.add(currentProcessTimeEstimate);
            curProcessTime = 0;
            currentProcessTimeEstimate = Objects.requireNonNull(processesTimeEstimates.poll());
            process = (sProcess) processVector.elementAt(currentProcess);
            out.println("main.Process: " + currentProcess + " registered... (" + process.cputime + " " + process.ioblocking + " " + process.cpudone + " " + process.cpudone + ")");
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
