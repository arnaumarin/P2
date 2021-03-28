#!/bin/bash

# Be sure that this file has execution permissions:
# Use the nautilus explorer or chmod +x run_vad.sh

# Write here the name and path of your program and database
DB=$HOME/PAV/P2/db.v4
CMD=bin/vad
rm -f out_alpha.txt
for index in $(seq 0 100); do
    echo "i = $index"
    #echo -n "$index ">> out_alpha1.txt
    k0=$(bc <<<"scale=2; $index*25") #runs the vad program modifying k0 in each loop (in this case it ranges from 4 to 6 with steps of 0.1)
    echo "k0 = $k0"
    echo -n "$k0 ">> out_alpha.txt

    for filewav in $DB/*/*wav; do

        if [[ ! -f $filewav ]]; then
            echo "Wav file not found: $filewav" >&2
            exit 1
        fi

        filevad=${filewav/.wav/.vad}
        
        #$CMD -i $filewav -o $filevad -0 $k0 -1 20 -2 6 || exit 1
        $CMD -i $filewav -o $filevad -2 $k0 || exit 1

        # Alternatively, uncomment to create output wave files
        #    filewavOut=${filewav/.wav/.vad.wav}
        #    $CMD $filewav $filevad $filewavOut || exit 1

    done
    
    scripts/vad_evaluation_printless.pl $DB/*/*lab 
    echo ""
done

exit 0
