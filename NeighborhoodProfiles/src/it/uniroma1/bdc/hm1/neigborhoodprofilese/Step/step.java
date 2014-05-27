/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package it.uniroma1.bdc.hm1.neigborhoodprofilese.Step;

import java.util.HashSet;

/**
 *
 * @author piccio
 */
public class step {

    public static HashSet performance(HashSet kset, String inputFile) {
        
        HashSet<String> nextSet = new HashSet();
        
        //****PER GRAFI DIRETTI
        //per ogni riga in input
            //se il primo elemeno è in kset
                //add secondo elemento in nextSet
        //****PER GRAFI NON DIRETTI
        //per ogni riga in input
            //se uno dei due elementi è in kset
                //add altro elemento in nextSet
        
        return nextSet;
    }
    
}
