#!/bin/sh

# thanks for gdal

symbol_list=""
for filename in arraylist.o debug.o json_c_version.o json_object.o json_tokener.o json_object_iterator.o json_util.o linkhash.o printbuf.o random_seed.o ; do
symbol_list="$symbol_list $(objdump -t $filename | grep text | awk '$2 ~ /g/ {print $6}' | grep -v .text)"
symbol_list="$symbol_list $(objdump -t $filename | grep .data.rel.local | awk '{print $6}' | grep -v .data.rel.local)"
done

echo "/* This is a generated file by dump_symbols.sh. *DO NOT EDIT MANUALLY ! */"
echo "#ifndef symbol_renames"
echo "#define symbol_renames"
echo ""
for symbol in $symbol_list
do
    echo "#define $symbol ag_${symbol}_"
done
echo ""
echo "#endif /*  symbol_renames */"

