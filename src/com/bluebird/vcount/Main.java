package com.bluebird.vcount;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Random;

public class Main {

    private static ArrayList<Bar> bars = new ArrayList<>(50);
    private static boolean onDebug = false;
    private static final int DEBUG_COUNT = 100000;
    private static long readTime = 0;
    private static long parseTime = 0;
    private static long writeFormatTime = 0;
    private static long writeTime = 0;
    private static long calculatedTotal = 0;

    static class Bar {

        private int count = 0;
        private final double xValue;
        private final double yValue;

        public Bar(double xValue, double yValue) {
            this.xValue = xValue;
            this.yValue = yValue;
        }

        void connectTo(int num) {
            bars.get(num).incrementCount();
            incrementCount();
        }

        void connectTo(Bar bar){
            bar.incrementCount();
            incrementCount();
        }

        void incrementCount(){
            count++;
        }

        double getXValue() {
            return xValue;
        }

        double getYValue() {
            return yValue;
        }

        int getConnectionCount() {
            return count;
        }
    }

    private static void calculateVisibility(){
        show("Calculating visibility..");
        int total = bars.size();
        int count = 0;
        calculatedTotal = System.currentTimeMillis();
        while (count < total) {
            Bar present = bars.get(count);
            int temp = count + 1;
            boolean isVisible;
            double ta = present.getXValue();
            double ya = present.getYValue();
            //   System.out.println("Checking for (" + ta + "," + ya + ") "+count);
            double tc = present.getXValue();
            double yc = present.getYValue();
            while (temp < total) {
                Bar target = bars.get(temp);
                if (temp - 1 == count) {
                    present.connectTo(target);
                    tc = target.getXValue();
                    yc = target.getYValue();
                } else {
                    double tb = target.getXValue();
                    double yb = target.getYValue();
                    double div = ((ya - yb) * ((tb - tc) / (tb - ta))) + yb;
               //     System.out.println("ya : "+ya+"\tyb : "+yb+"\ttb : "+tb+"\ttc : "+tc+"\tta : "+ta);
                    isVisible = (yc < div);
                    if (isVisible) {
                        present.connectTo(target);
                          //  System.out.println("\tConnected to : ("+tb+","+yb+") "+temp);
                           //  System.out.println(count+"->"+temp);
                        if (yb > yc) {
                             // System.out.println("Reassigned im to " + target.getXValue() + "," + target.getYValue());
                            tc = target.getXValue();
                            yc = target.getYValue();
                        }
                    }

                }
                temp++;
            }
            count++;
        }
        calculatedTotal = System.currentTimeMillis() - calculatedTotal;
        show("Visibility calculated..");
    }

    private static void show(String stat){
        System.out.println(LocalDateTime.now()+" : "+stat);
    }

    private static void parseInput(String f) throws IOException, NumberFormatException {
        File file = new File(f);
        FileInputStream fis = new FileInputStream(file);
        byte [] buffer = new byte[(int)file.length()];
        show("Reading file..");
        readTime = System.currentTimeMillis();
        fis.read(buffer);
        readTime = (System.currentTimeMillis() - readTime);
        fis.close();
        show("Parsing input..");
        parseTime = System.currentTimeMillis();
        String input = new String(buffer, StandardCharsets.UTF_8);
        int count = 1;
        for(String read : input.split("\n")) {
            Double value = Double.parseDouble(read);
            bars.add(new Bar(count, value));
            count++;
        }
        parseTime = System.currentTimeMillis() - parseTime;
        show("Parsing complete..");
        show("Values found : " + bars.size() + "..");
    }

    private static void writeOutput(String file) throws IOException {
        final int[] count = {1};
        show("Preparing write..");
        writeFormatTime = System.currentTimeMillis();
        StringBuilder sb = new StringBuilder();
        bars.forEach(bar -> sb.append(String.format("Bar : %3d\t\tValue : %12.6f\t\t", (count[0]++), bar.getYValue()))
                .append("Nodes : ")
                .append(bar.getConnectionCount())
                .append("\n"));
        writeFormatTime = System.currentTimeMillis() - writeFormatTime;
        FileOutputStream fileOutputStream = new FileOutputStream(new File(file));
        show("Writing out..");
        writeTime = System.currentTimeMillis();
        fileOutputStream.write(sb.toString().getBytes());
        fileOutputStream.close();
        writeTime = System.currentTimeMillis() - writeTime;
        show("Writing complete..");
    }

    private static void generateRandomData() throws FileNotFoundException {
        int count = 0;
        Random r = new Random();
        show("Generating random input..");
        StringBuilder sb = new StringBuilder();
        while (count < DEBUG_COUNT) {
            sb.append(r.nextInt(100000))
                    .append(r.nextDouble())
                    .append("\n");
            count++;
        }
        bars.ensureCapacity(DEBUG_COUNT);
        PrintWriter writer = new PrintWriter("debug_in");
        show("Creating input file..");
        writer.print(sb);
        writer.close();
        show("Input file created..");
    }

    private static void generateAbsoluteData(){
        bars.add(new Bar(1,59.0));
        bars.add(new Bar(2,20.0));
        bars.add(new Bar(3,25.0));
        bars.add(new Bar(4,59.0));
        bars.add(new Bar(5,20.0));
        bars.add(new Bar(6,25.0));
        bars.add(new Bar(7,59.0));
        bars.add(new Bar(8,20.0));
        bars.add(new Bar(9,25.0));
        bars.add(new Bar(10,59.0));
    }

    private static void printUsage(){
        System.out.println("Usage : java -jar VisibilityCount.jar input_file output_file");
    }

    private static void showStats() {
        show("Stats");
        show("=====");
        show("Total values : " + bars.size() + "..");
        show("Reading                =>\tTotal : " + readTime + "ms\tAverage : " + (float) readTime / bars.size() + "ms");
        show("Parsing                =>\tTotal : " + parseTime + "ms\tAverage : " + (float) parseTime / bars.size() + "ms");
        show("Visibility calculation =>\tTotal : " + calculatedTotal + "ms\tAverage : " + (float) calculatedTotal / bars.size() + "ms");
        show("Output formatting      =>\tTotal : " + writeFormatTime + "ms\tAverage : " + (float) writeFormatTime / bars.size() + "ms");
        show("Writing                =>\tTotal : " + writeTime + "ms\tAverage : " + (float) writeTime / bars.size() + "ms");
    }

    public static void main(String[] args) {
        if(onDebug){
            try {
                show("DEBUG MODE ENABLED");
                generateRandomData();
             //   generateAbsoluteData();
                parseInput("debug_in");
                calculateVisibility();
                writeOutput("debug_out");
                showStats();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        else {
            if (args.length != 2) {
                System.err.println("Wrong input!");
                printUsage();
            } else if (!new File(args[0]).exists()) {
                System.err.println("Input file does not exist!");
                printUsage();
            } else {
                try {
                    parseInput(args[0]);
                    calculateVisibility();
                    writeOutput(args[1]);
                    showStats();
                } catch (IOException e) {
                    System.err.println("Unable to complete! Fatal error!\n");
                    e.printStackTrace();
                }
            }
        }
    }
}
