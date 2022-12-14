package main;// This file contains the main() function for the main.Scheduling
// simulation.  Init() initializes most of the variables by
// reading from a provided file.  main.SchedulingAlgorithm.Run() is
// called from main() to run the simulation.  Summary-main.Results
// is where the summary results are written, and Summary-Processes
// is where the process scheduling summary is written.

// Created by Alexander Reeder, 2001 January 06

import java.io.*;
import java.time.LocalDateTime;
import java.util.*;

public class Scheduling {

  private static int processnum = 5;
  private static int meanDev = 1000;
  private static int standardDev = 100;
  private static int runtime = 1000;
  private static double agingCoef = 0.5;
  private static Vector<sProcess> processVector = new Vector<>();
  private static Results result = new Results("null","null",0);
  private static String resultsFile = "Summary-Results";

  private static void parseAgingCoef(String string) {
    try {
      agingCoef = Double.parseDouble(string.trim());
    } catch (NumberFormatException nfe) {
      System.out.println("NumberFormatException: " + nfe.getMessage());
    }
    if (agingCoef < 0 || agingCoef > 1) {
      System.out.println("Invalid agingCoef: must be within [0,1]. Using default agingCoef = 0.5");
      agingCoef = 0.5;
    }
  }

  private static void Init(String file) {
    File f = new File(file);
    String line;
    double X = 0.0;
    int processId = 1;
    try {   
      //BufferedReader in = new BufferedReader(new FileReader(f));
      DataInputStream in = new DataInputStream(new FileInputStream(f));
      while ((line = in.readLine()) != null) {
        if (line.startsWith("numprocess")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          processnum = Common.s2i(st.nextToken());
        }
        if (line.startsWith("mean")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          meanDev = Common.s2i(st.nextToken());
        }
        if (line.startsWith("standdev")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          standardDev = Common.s2i(st.nextToken());
        }
        if (line.startsWith("process")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          int runTimeBeforeBlocking = Common.s2i(st.nextToken());
          int blockDuration = Common.s2i(st.nextToken());
          int arrivalTime = Common.s2i(st.nextToken());
          X = Common.R1();
          while (X == -1.0) {
            X = Common.R1();
          }
          X = X * standardDev;
          int cpuTime = (int) X + meanDev;
          processVector.add(new sProcess(processId, cpuTime, runTimeBeforeBlocking, blockDuration, arrivalTime));
          processId++;
        }
        if (line.startsWith("runtime")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          runtime = Common.s2i(st.nextToken());
        }
        if (line.startsWith("agingCoef")) {
          StringTokenizer st = new StringTokenizer(line);
          st.nextToken();
          parseAgingCoef(st.nextToken());
        }
      }
      in.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  private static void debug() {
    int i = 0;

    System.out.println("processnum " + processnum);
    System.out.println("meandevm " + meanDev);
    System.out.println("standdev " + standardDev);
    int size = processVector.size();
    for (i = 0; i < size; i++) {
      sProcess process = processVector.elementAt(i);
      System.out.println("process " + i + " " + process.cpuTime + " " + process.runTimeBeforeBlocking + " " + process.cpuDone + " " + process.numBlocked);
    }
    System.out.println("runtime " + runtime);
  }

  public static void main(String[] args) {
    int i = 0;

    if (args.length != 1) {
      System.out.println("Usage: 'java main.Scheduling <INIT FILE>'");
      System.exit(-1);
    }
    File f = new File(args[0]);
    if (!(f.exists())) {
      System.out.println("main.Scheduling: error, file '" + f.getName() + "' does not exist.");
      System.exit(-1);
    }  
    if (!(f.canRead())) {
      System.out.println("main.Scheduling: error, read of " + f.getName() + " failed.");
      System.exit(-1);
    }
    System.out.println("Working...");
    Init(args[0]);
    if (processVector.size() < processnum) {
      i = 0;
      int processId = processVector.size() + 1;
      while (processVector.size() < processnum) {       
          double X = Common.R1();
          while (X == -1.0) {
            X = Common.R1();
          }
          X = X * standardDev;
        int cpuTime = (int) X + meanDev;
        processVector.add(new sProcess(processId, cpuTime,i*100,i * 10,0));
        i++;
        processId++;
      }
    }
    result = SchedulingAlgorithm.run(runtime, processVector, result, agingCoef);
    try {
      //BufferedWriter out = new BufferedWriter(new FileWriter(resultsFile));
      PrintStream out = new PrintStream(new FileOutputStream(resultsFile));
      out.println("DateTime: " + LocalDateTime.now());
      out.println("main.Scheduling Type: " + result.schedulingType);
      out.println("main.Scheduling Name: " + result.schedulingName);
      out.println("Simulation Run Time: " + result.compuTime);
      out.println("Mean: " + meanDev);
      out.println("Standard Deviation: " + standardDev);
      out.println("main.Process #\tCPU Time\tIO Blocking\tCPU Completed\tCPU Blocked");
      for (i = 0; i < processVector.size(); i++) {
        sProcess process = processVector.elementAt(i);
        out.print(Integer.toString(i));
        if (i < 100) { out.print("\t\t"); } else { out.print("\t"); }
        out.print(Integer.toString(process.cpuTime));
        if (process.cpuTime < 100) { out.print(" (ms)\t\t"); } else { out.print(" (ms)\t"); }
        out.print(Integer.toString(process.runTimeBeforeBlocking));
        if (process.runTimeBeforeBlocking < 100) { out.print(" (ms)\t\t"); } else { out.print(" (ms)\t"); }
        out.print(Integer.toString(process.cpuDone));
        if (process.cpuDone < 100) { out.print(" (ms)\t\t"); } else { out.print(" (ms)\t"); }
        out.println(process.numBlocked + " times");
      }
      out.close();
    } catch (IOException e) { /* Handle exceptions */ }
  System.out.println("Completed.");
  }
}

