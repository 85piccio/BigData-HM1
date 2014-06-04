
package it.uniroma1.bdc.hm1.neigborhoodprofiles.Step;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashSet;


public class NextNeighborhood {

    public static HashSet directGraph(HashSet kset, HashSet visited, String inputFile, Integer bufDim) throws FileNotFoundException, IOException {

        HashSet<String> nextSet = new HashSet();
        BufferedReader br;        
        if (bufDim < 0) {
            br = new BufferedReader(new FileReader(inputFile));
        } else {
            br = new BufferedReader(new FileReader(inputFile), bufDim );
        }
        String line;

        while ((line = br.readLine()) != null) {//per ogni riga in input
            // process the line.
            String part[] = line.split("\\s+");

            if (part.length >= 2) {//se il primo elemeno è in kset
                if (kset.contains(part[0])) {
                    if (!visited.contains(part[1])) {
                        nextSet.add(part[1]);//add secondo elemento in nextSet
                        visited.add(part[1]);//add secondo elemento tra i già visitati
                    }
                }
            }

        }
        br.close();
        return nextSet;
    }

    public static HashSet undirectGraph(HashSet kset, HashSet visited, String inputFile, Integer bufDim) throws FileNotFoundException, IOException {

        HashSet<String> nextSet = new HashSet();
        BufferedReader br;
        if (bufDim < 0) {
            br = new BufferedReader(new FileReader(inputFile));
        } else {
            br = new BufferedReader(new FileReader(inputFile), bufDim * 1024);
        }
        String line;

        while ((line = br.readLine()) != null) {//per ogni riga in input
            // process the line.
            String part[] = line.split("\\s+");

            if (part.length >= 2) {

                //verso 1
                //se il primo elemeno è in kset
                if (kset.contains(part[0])) {
                    if (!visited.contains(part[1])) {
                        nextSet.add(part[1]);//add secondo elemento in nextSet
                        visited.add(part[1]);//add secondo elemento tra i già visitati
                    }
                }

                //verso 2
                //se il secondo elemeno è in kset
                if (kset.contains(part[1])) {
                    if (!visited.contains(part[0])) {
                        nextSet.add(part[0]);//add secondo elemento in nextSet
                        visited.add(part[0]);//add secondo elemento tra i già visitati
                    }
                }
            }
        }
        br.close();
        return nextSet;
    }
}
