if [ ! -d "results" ]; then
    mkdir results
fi
MAX_RETRIES=5
RETRY_DELAY=2
if [ -n "$1" ]; then
    if [ ! -d "results/$1" ]; then
        mkdir "results/$1"
    fi
    if [ ! -d "results/$1/concurrent" ]; then
        mkdir "results/$1/concurrent"
    fi
    if [ "$1" = "radixgraph" ]; then
        ./optimizer datasets/dota-league.v 8 > settings
    fi
    for a in 4 8 16 32
    do
        ATTEMPT=1
        while [ $ATTEMPT -le $MAX_RETRIES ]; do
            echo "Attempt $ATTEMPT..."
            
            ./build/gfe_driver -G datasets/dota-league.properties -u --log ./dota-league.graphlog --aging_timeout 48h -l $1 -r $a -w 32 --aging_release_memory true --mixed_workload true > results/$1/concurrent/dota-league-$a-read-threads-$1
            EXIT_CODE=$?

            # 139 is segmentation fault, but you can include other codes if needed
            if [ $EXIT_CODE -eq 0 ]; then
                echo "Program completed successfully."
                break
            elif [ $EXIT_CODE -eq 139 ]; then
                echo "Segmentation fault detected. Retrying in $RETRY_DELAY seconds..."
            else
                echo "Program exited with code $EXIT_CODE. Retrying in $RETRY_DELAY seconds..."
            fi

            ((ATTEMPT++))
            sleep $RETRY_DELAY
        done

        ATTEMPT=1
        while [ $ATTEMPT -le $MAX_RETRIES ]; do
            echo "Attempt $ATTEMPT..."
            
            ./build/gfe_driver -G datasets/dota-league.properties -u --log ./dota-league.graphlog --aging_timeout 48h -l $1 -r 32 -w $a --aging_release_memory true --mixed_workload true > results/$1/concurrent/dota-league-$a-write-threads-$1
            EXIT_CODE=$?

            # 139 is segmentation fault, but you can include other codes if needed
            if [ $EXIT_CODE -eq 0 ]; then
                echo "Program completed successfully."
                break
            elif [ $EXIT_CODE -eq 139 ]; then
                echo "Segmentation fault detected. Retrying in $RETRY_DELAY seconds..."
            else
                echo "Program exited with code $EXIT_CODE. Retrying in $RETRY_DELAY seconds..."
            fi

            ((ATTEMPT++))
            sleep $RETRY_DELAY
        done
    done
    # if [ "$1" = "radixgraph" ]; then
    #     ./optimizer datasets/graph500-24.v 8 > settings
    # fi
    # for a in 4 8 16 32
    # do
    #     ATTEMPT=1
    #     while [ $ATTEMPT -le $MAX_RETRIES ]; do
    #         echo "Attempt $ATTEMPT..."
            
    #         ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog -l $1 -r $a -w 32 --mixed_workload true > results/$1/concurrent/graph500-24-$a-read-threads-$1
    #         EXIT_CODE=$?

    #         # 139 is segmentation fault, but you can include other codes if needed
    #         if [ $EXIT_CODE -eq 0 ]; then
    #             echo "Program completed successfully."
    #             break
    #         elif [ $EXIT_CODE -eq 139 ]; then
    #             echo "Segmentation fault detected. Retrying in $RETRY_DELAY seconds..."
    #         else
    #             echo "Program exited with code $EXIT_CODE. Retrying in $RETRY_DELAY seconds..."
    #         fi

    #         ((ATTEMPT++))
    #         sleep $RETRY_DELAY
    #     done
        
    #     ATTEMPT=1
    #     while [ $ATTEMPT -le $MAX_RETRIES ]; do
    #         echo "Attempt $ATTEMPT..."
            
    #         ./build/gfe_driver -G datasets/graph500-24.properties -u --log ./graph500-24-1.0.graphlog -l $1 -r 32 -w $a --mixed_workload true > results/$1/concurrent/graph500-24-$a-write-threads-$1
    #         EXIT_CODE=$?

    #         # 139 is segmentation fault, but you can include other codes if needed
    #         if [ $EXIT_CODE -eq 0 ]; then
    #             echo "Program completed successfully."
    #             break
    #         elif [ $EXIT_CODE -eq 139 ]; then
    #             echo "Segmentation fault detected. Retrying in $RETRY_DELAY seconds..."
    #         else
    #             echo "Program exited with code $EXIT_CODE. Retrying in $RETRY_DELAY seconds..."
    #         fi

    #         ((ATTEMPT++))
    #         sleep $RETRY_DELAY
    #     done
    # done
    if [ "$1" = "radixgraph" ]; then
        rm settings
    fi
fi