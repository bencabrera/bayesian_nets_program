#!/bin/bash
rm -r *.png
for dot_file in *.dot; do
	i=$(basename ${dot_file} .dot);
	dot -Tpng ${dot_file} >> ${i}.png
done
