#!/bin/bash

if [ -z "$1" ] ; then
    echo "Missing argument - filename!"
    exit 1
fi

output_name=$(echo "$1" | sed 's/.html//')
output_filename="${output_name}.h"

echo """#ifndef page_${output_name^^}_H
#define page_${output_name^^}_H

// This file was generated using xxd""" > $output_filename

# Gzip file and write to static array
gzip --best "$1" -c > "page_${output_name}"
xxd -i -u "page_${output_name}" >> $output_filename
rm "page_${output_name}"

echo -e "\n#endif" >> $output_filename