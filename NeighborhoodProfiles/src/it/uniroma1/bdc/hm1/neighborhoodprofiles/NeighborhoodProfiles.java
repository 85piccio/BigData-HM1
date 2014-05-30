/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.uniroma1.bdc.hm1.neighborhoodprofiles;

import it.uniroma1.bdc.hm1.neigborhoodprofiles.Step.step;
import java.io.IOException;
import java.util.HashSet;

/**
 *
 * @author piccio
 */
public class NeighborhoodProfiles {

    /**
     * @param args the command line arguments
     * @throws java.io.IOException
     */
    public static void main(String[] args) throws IOException {

        if (args.length < 3) {
            System.out.println("input:\nneighborhoodprofiles.jar k inputFile idNodo [d/u] [buffer dimension]");
            return;
        }

        //read input file n_nodo e kstep
        Integer k = new Integer(args[0]);
        String inputFile = args[1];//"dataset/p2p-Gnutella3.txt";
        String nNodo = args[2];
        String graphType = "d";

        Integer bufDim = -1;

        if (args.length >= 4 && args[3].compareTo("") != 0) {
            graphType = args[3];
        }
        if (args.length >= 5 && args[4].compareTo("") != 0) {
            bufDim = new Integer(args[4]);
        }

        //all Neighborhood profiles
        if (k < 0) {
            k = Integer.MAX_VALUE;
        }

        //neigborhood profile |N(v,i)|
        HashSet<String> kset = new HashSet();
        HashSet<String> visited = new HashSet();
        //add nodo da analizzare
        kset.add(nNodo);
        visited.add(nNodo);

        System.out.println("Neighborhood profiles |N(v,0)|\t1");

        //foreach step
        for (int i = 0; i < k; i++) {

            System.out.print("Neighborhood profiles |N(v," + (i + 1) + ")|\t");

            //  performance step
            if (graphType.compareTo("d") == 0) {
                kset = step.performDirectGraph(kset, visited, inputFile, bufDim);
            } else if (graphType.compareTo("u") == 0) {
                kset = step.performUndirectGraph(kset, visited, inputFile, bufDim);
            } else {
                System.out.println("paramentro non riconosciuto");
            }

            //stampa dimensione step
            System.out.print(kset.size() + "\n");

            if (kset.size() < 1) {
                System.out.println("No more neighborhood --> stop processing");
                return;
            }

            //Stampa elementi del set
            //  print kset --> |N(v,i)|
//            for(String s: kset){
//                System.out.println(s+"\t");
//            }
        }
    }

}
