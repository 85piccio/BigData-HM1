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
        // TODO code application logic here

        
        if(args.length<3){
            System.out.println("input: neighborhoodprofiles.jar k inputFile idNodo");
            return;
        }

        //read input file n_nodo e kstep
        Integer k = new Integer(args[0]);
        String inputFile = args[1];//"dataset/facebook_combined.txt";
        String nNodo = args[2];

        //neigborhood profile |N(v,i)|
        HashSet<String> kset = new HashSet();
        HashSet<String> visited = new HashSet();
        //add nodo da analizzare
        kset.add(nNodo);
        visited.add(nNodo);
        //foreach step
        for (int i = 0; i < k; i++) {

            System.out.print("Neighborhood profiles |N(v," + (i+1) + ")|\t");

            //  performance step
            kset = step.perform(kset, visited, inputFile);

            //stampa dimensione step
            System.out.print(kset.size() + "\n");
            //  print kset --> |N(v,i)|
//            for(String s: kset){
//                System.out.println(s+"\t");
//            }

        }
    }

}
