#!/bin/bash
mkdir log
for i in {1..128..8}
do
	make DEPTH=$i
done
