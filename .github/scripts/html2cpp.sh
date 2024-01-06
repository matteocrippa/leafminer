#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 <input_html_file>"
  exit 1
fi

input_file=$1
output_file="${input_file%.html}.h"

# Check if the input HTML file exists
if [ ! -f "$input_file" ]; then
  echo "Error: Input HTML file not found."
  exit 1
fi

# Minify HTML by removing unnecessary whitespace and comments
html_content=$(cat "$input_file" | tr -d '\n\r\t' | sed 's/<!--.*-->//g' | sed 's/> *</></g' | sed 's/"/\\"/g')

# Write the C++ variable to the output .h file
cat <<EOF > "$output_file"
#ifndef HTML_FILE_H
#define HTML_FILE_H

#include <string>

const std::string html = "$html_content";

#endif // HTML_FILE_H
EOF

echo "Minified C++ variable saved to $output_file"
