#!/bin/sh


echo "<=================>"
echo "<= CLEANING DERN =>"
echo "<=================>"
sleep 0.5
./fclean.sh
echo "<=================>"
echo "<= BUILDING DERN =>"
echo "<=================>"
sleep 0.5
./build.sh
echo "<================>"
echo "<= RUNNING DERN =>"
echo "<================>"
sleep 0.5
./run.sh "$@"

