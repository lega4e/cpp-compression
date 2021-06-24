#!/bin/bash





echo "Encoding..."
for file in io/*
do
	if (( `expr "$file" : '.*\.dec'` != 0 || `expr "$file" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "Enconding '$file' to '$file.enc'"
	./lz77 -e "$file" -o "$file.enc"

	if [[ $? != "0" ]]
	then
		exit 1
	fi
done
echo "Success"
echo



echo "Decoding"
for file in io/*.enc
do
	echo "Decoding '$file' to '$file.dec'"
	./lz77 -d "$file" -o "$file.dec"

	if [[ $? != "0" ]]
	then
		exit 1
	fi
done
echo "Success"
echo



echo "Check for algorithm validity"
for file in io/*
do
	if (( `expr "$file" : '.*\.dec'` != 0 || `expr "$file" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "Comparison '$file' and '$file.enc.dec'"
	diff "$file" "$file.enc.dec" &>/dev/null
	if [[ $? != "0" ]]
	then
		echo "Files '$file' and '$file.enc.dec' is't equal"
		exit 1
	fi
done
echo "Success"
echo



echo "Check compression efficiency"
for file in io/*
do
	if (( `expr "$file" : '.*\.dec'` != 0 || `expr "$file" : '.*\.enc'` != 0 ))
	then
		continue
	fi

	echo "$file.enc" "$file" "`wc -c \"$file.enc\" | cut -d' ' -f1`" "`wc -c \"$file\" | cut -d' ' -f1`" | 
		awk '{printf "%.3f : %s %s\n", $3/$4, $1,$2}'
done





# END
