package main;

public class sProcess {
  public int id;
  public int cpuTime;
  public int runTimeBeforeBlocking;
  public int cpuDone = 0;
  public int ioNext = 0;
  public int numBlocked = 0;

  public boolean isBlocked = false;

  public int blockDuration;

  public int arrivalTime;

  public int lastTimeExecuted = -1;
  // initializing every process with zero time estimate
  public double timeEstimate = 0;

  public sProcess(int id, int cpuTime, int runTimeBeforeBlocking, int blockDuration, int arrivalTime) {
    this.id = id;
    this.cpuTime = cpuTime;
    this.runTimeBeforeBlocking = runTimeBeforeBlocking;
    this.blockDuration = blockDuration;
    this.arrivalTime = arrivalTime;
  }
  public String processInfo() {
    return "Process info: " +
            "cpuTime=" + cpuTime +
            ", runTimeBeforeBlocking=" + runTimeBeforeBlocking +
            ", cpuDone=" + cpuDone +
            ", numBlocked=" + numBlocked +
            ", blockDuration=" + blockDuration +
            ", arrivalTime=" + arrivalTime +
            ", timeEstimate=" + timeEstimate;
  }
}
