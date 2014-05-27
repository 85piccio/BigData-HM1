/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package it.uniroma1.bdc.hm1.neighborhoodprofiles;

import it.uniroma1.bdc.hm1.neigborhoodprofilese.Step.step;
import java.util.HashSet;



/**
 *
 * @author piccio
 */
public class NeighborhoodProfiles {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        
        //read input file n_nodo e kstep
        Integer k = 6;
        String inputFile = "/";
        String nNodo = "1";
        
        //neigborhood profile |N(v,i)|
        HashSet<String> kset = new HashSet();
        //foreach step
        for(int i = 0; i< k; i++){
            
            System.out.println("Neighborhood profiles |N(v," + i +")|\n" );
            
            //  performance step
            kset = step.performance(kset, inputFile);
            
            //  print kset --> |N(v,i)|
            for(String s: kset){
                System.out.println(s+"\t");
            }
            
            
                        
            
        
        }
    }
    
}
