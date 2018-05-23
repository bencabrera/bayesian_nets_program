#!/bin/bash
# set -x
mkdir png
rm -r png/*.png
rm -r *.gif
for dot_file in dot/*.dot; do
	i=$(basename ${dot_file} .dot);
	echo ${i}
	dot -Tpng ${dot_file} >> png/${i}.png
done

#get max width and height
width_max=0
for png_file in png/*.png; do
	val=$(convert $png_file -print "%w" info)
	width_max=$(( width_max > val ? width_max : val ))
done
echo $width_max

height_max=0
for png_file in png/*.png; do
	val=$(convert $png_file -print "%h" info)
	height_max=$(( height_max > val ? height_max : val ))
done
echo $height_max
rm -r info

for png_file in png/*.png; do
	convert $png_file -gravity center -background white -extent ${width_max}x${height_max} $png_file
done

convert -delay 200 -loop 0 png/*.png animation.gif
