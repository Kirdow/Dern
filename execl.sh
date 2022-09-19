#!/bin/sh

while true
do
	clear
	./exec.sh test.dern
	read -p "Press any key to run again... (Ctrl+C to stop) " -n1 -s
	echo 
done

