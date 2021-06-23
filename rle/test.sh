#!/bin/bash





echo "Encoding..."
for img in imgs/*
do
	if (( `expr "$img" : '.*\.dec'` != 0 || `expr "$img" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "Enconding '$img' to '$img.enc'"
	./rle -e "$img" -o "$img.enc"

	if [[ $? != "0" ]]
	then
		exit 1
	fi
done
echo "Success"
echo



echo "Decoding"
for img in imgs/*.enc
do
	echo "Decoding '$img' to '$img.dec'"
	./rle -d "$img" -o "$img.dec"

	if [[ $? != "0" ]]
	then
		exit 1
	fi
done
echo "Success"
echo



echo "Check for algorithm validity"
for img in imgs/*
do
	if (( `expr "$img" : '.*\.dec'` != 0 || `expr "$img" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "Comparison '$img' and '$img.enc.dec'"
	diff "$img" "$img.enc.dec" &>/dev/null
	if [[ $? != "0" ]]
	then
		echo "Files '$img' and '$img.enc.dec' is't equal"
		exit 1
	fi
done
echo "Success"
echo



echo "Check compression efficiency"
for img in imgs/*
do
	if (( `expr "$img" : '.*\.dec'` != 0 || `expr "$img" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "$img.enc" "$img" "`wc -c \"$img.enc\" | cut -d' ' -f1`" "`wc -c \"$img\" | cut -d' ' -f1`" | 
		awk '{printf "%.3f : %s %s\n", $3/$4, $1,$2}'
done





# END
