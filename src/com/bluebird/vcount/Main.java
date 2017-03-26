package com.bluebird.vcount;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Random;

public class Main {

    private static ArrayList<Bar> bars = new ArrayList<>();
    private static boolean onDebug = false;

    static class Bar {

        private Integer count = 0;
        private final Integer xValue;
        private final Double yValue;

        public Bar(Integer xValue, Double yValue) {
            this.xValue = xValue;
            this.yValue = yValue;
        }

        void connectTo(int num) {
            bars.get(num).incrementCount();
            incrementCount();
        }

        void incrementCount(){
            count++;
        }

        Integer getXValue() {
            return xValue;
        }

        Double getYValue() {
            return yValue;
        }

        public Integer getConnectionCount() {
            return count;
        }
    }

    private static void calculateVisibility(){
        show("Calculating visibility..");
        int total = bars.size();
        int count = 0;
        while (count < total) {
            Bar present = bars.get(count);
            int temp = count + 1;
            boolean isVisible;
            double ta = Double.parseDouble(present.getXValue() + ".0");
            double ya = present.getYValue();
            //   System.out.println("Checking for (" + ta + "," + ya + ") "+count);
            Bar intermediate = present;
            while (temp < total) {
                Bar target = bars.get(temp);
                double tb = Double.parseDouble(target.getXValue() + ".0");
                double yb = target.getYValue();
                if (temp - 1 == count) {
                    present.connectTo(temp);
                    intermediate = target;
                } else {
                    double tc = Double.parseDouble(intermediate.getXValue() + ".0");
                    double yc = intermediate.getYValue();
                    double div = (ya - yb) * ((tb - tc) / (tb - ta));
               //     System.out.println("ya : "+ya+"\tyb : "+yb+"\ttb : "+tb+"\ttc : "+tc+"\tta : "+ta);
                    isVisible = (yc < (div + yb));
                    if (isVisible) {
                        present.connectTo(temp);
                          //  System.out.println("\tConnected to : ("+tb+","+yb+") "+temp);
                           //  System.out.println(count+"->"+temp);
                        if (yb > intermediate.getYValue()) {
                             // System.out.println("Reassigned im to " + target.getXValue() + "," + target.getYValue());
                            intermediate = target;
                        }
                    }

                }
                temp++;
            }
            count++;
        }
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
        fis.read(buffer);
        fis.close();
        show("Parsing input..");
        String input = new String(buffer, StandardCharsets.UTF_8);
        int count = 1;
        for(String read : input.split("\n")) {
            Double value = Double.parseDouble(read);
            bars.add(new Bar(count, value));
            count++;
        }
        show("Parsing complete..");
    }

    private static void writeOutput(String file) throws IOException {
        final int[] count = {1};
        show("Preparing write..");
        StringBuilder sb = new StringBuilder();
        bars.forEach(bar-> sb.append(String.format("Bar : %3d\t\tValue : %12g\t\t", (count[0]++), bar.getYValue()))
                .append("Nodes : ")
                .append(bar.getConnectionCount())
                .append("\n"));
        FileOutputStream fileOutputStream = new FileOutputStream(new File(file));
        show("Writing out..");
        fileOutputStream.write(sb.toString().getBytes());
        fileOutputStream.close();
        show("Writing complete..");
    }

    private static void generateRandomData() throws FileNotFoundException {
        int count = 0;
        Random r = new Random();
        show("Generating random input..");
        StringBuilder sb = new StringBuilder();
        while(count<1000000){
            sb.append(r.nextInt(100000))
                    .append(r.nextDouble())
                    .append("\n");
            count++;
        }
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

    public static void main(String[] args) {
        if(onDebug){
            try {
                show("DEBUG MODE ENABLED");
                generateRandomData();
             //   generateAbsoluteData();
                parseInput("debug_in");
                calculateVisibility();
                writeOutput("debug_out");
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
                } catch (IOException e) {
                    System.err.println("Unable to parse input! Fatal error!\n");
                    e.printStackTrace();
                }
            }
        }
    }
}
