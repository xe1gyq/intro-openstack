#!/bin/bash

################################################################
# Download the memory pressure program
cd /home/ubuntu
curl -s http://10.1.1.8:8111/testprogram >testprogram
chmod +x testprogram
echo Downloaded testprogram

################################################################
# Periodically check if metadata has key "memorypressure"
# with value "yes".
# If so, launch the testprogram
# If not, kill it.

while :
do
    # Get metadata
    if ! curl -s 169.254.169.254/openstack/latest/meta_data.json > /tmp/metadata
    then
        # no access to metadata. Do nothing, try again next time.
        echo no metadata. Better luck next time?
        sleep 1
        continue
    fi
    # Check value of memorypressure key in metadata
    if grep -q '"memorypressure": "yes"' /tmp/metadata
    then
        # if testprogram is not running, launch it
        if ! pgrep testprogram >/dev/null
        then
            # it's not running
            echo memorypressure yes and testprogram NOT running
            /home/ubuntu/testprogram 1500 0 >> ~/testprogram.out 2>&1
            echo started testprogram
        # else: testprogram is running already: Nothing to do.
        fi
    else
        # Metadata contains no request for pressure.
        # Kill test program if it runs.
        if pgrep testprogram >/dev/null
        then
            echo memorypressure no and testprogram running
            pkill testprogram
            echo KILLED testprogram
        # else: testprogram is not running, no need to kill it.
        fi
    fi
    sleep 1
done &

