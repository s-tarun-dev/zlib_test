#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <directory_path> <compression_level>"
    exit 1
fi

directory_path=$1
compression_level=$2
csv_file="gzip_c${compression_level}.csv"

if [ ! -d "$directory_path" ]; then
    echo "Directory not found!"
    exit 1
fi

echo "Original File Name,File Size(B),Compressed File Size(B),Time Taken by deflate(ms),Time Taken in Compression(ms),Maximum Resident Size(B), CPU Usage(%)" > "$csv_file"

for input_file in "$directory_path"/*; do
    if [ -f "$input_file" ]; then
        for i in {1..3}; do
            file_name=$(basename "$input_file")
            output_file="${input_file}.gz"

            ./test2 "$input_file" "$compression_level" > temp_output.txt 2>&1

            original_size=$(wc -c < "$input_file" | awk '{$1=$1;print}')  
            compressed_size=$(wc -c < "$output_file" | awk '{$1=$1;print}')  
            compression_time=$(grep "Time taken for overall compression" temp_output.txt | awk '{print $6}')
            deflate_compression_time=$(grep "Time Taken by deflate" temp_output.txt | awk '{print $5}')
            cpu_utilization=$(grep "CPU Utilization by overall compression" temp_output.txt | awk '{print $6}')
            resident_set_size=$(grep "Maximum RSS" temp_output.txt | awk '{print $5}')

            echo "${file_name},${original_size},${compressed_size},${deflate_compression_time},${compression_time},${resident_set_size},${cpu_utilization}" >> "$csv_file"

            rm temp_output.txt
            rm "$output_file" 
        done
    fi
done

echo "Compression data logged in $csv_file"
