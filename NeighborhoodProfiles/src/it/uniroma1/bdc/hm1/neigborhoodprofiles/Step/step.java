/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.uniroma1.bdc.hm1.neigborhoodprofiles.Step;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;

/**
 *
 * @author piccio
 */
public class step {

    public static HashSet performance(HashSet kset, HashSet visited, String inputFile) throws FileNotFoundException, IOException {

        HashSet<String> nextSet = new HashSet();

        BufferedReader br = new BufferedReader(new FileReader(inputFile));
        String line;

        //****PER GRAFI DIRETTI
        while ((line = br.readLine()) != null) {//per ogni riga in input
            // process the line.
            String part[] = line.split("\\s+");//TODO:checl split

            if (part.length >= 2) {//se il primo elemeno è in kset
                if (kset.contains(part[0])) {
                    if (!visited.contains(part[1])) {
                        nextSet.add(part[1]);//add secondo elemento in nextSet
                        visited.add(part[1]);
                    }
                }
            }

        }
        br.close();

        //****PER GRAFI NON DIRETTI
        //per ogni riga in input
        //se uno dei due elementi è in kset
        //add altro elemento in nextSet
        return nextSet;
    }

}
