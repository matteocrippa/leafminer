#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 <input_html_file>"
  exit 1
fi

input_file=$1
output_file="${input_file%.html}"
uppercase_input_file=$(echo "$output_file" | tr '[:lower:]' '[:upper:]')

# Check if the input HTML file exists
if [ ! -f "$input_file" ]; then
  echo "Error: Input HTML file not found."
  exit 1
fi

# Minify HTML and CSS by removing unnecessary whitespace and comments
html_content=$(cat "$input_file" | tr -d '\n\r\t' | sed 's/<!--.*-->//g' | sed 's/> *</></g' | sed 's/ \+/ /g' | sed 's/{ /{/g' | sed 's/ }/}/g')

# Escape double quotes for C++ string and wrap in R"=====( )
escaped_html_content=$(echo "$html_content" | sed 's/"/\\"/g')
wrapped_html_content="\"$escaped_html_content\";"

# Write the C++ variable to the output .h file
cat <<EOF > "${output_file}.h"
/*
 * ${output_file}.h
 * Generated from ${input_file}
 */

#ifndef HTML_${uppercase_input_file}_H
#define HTML_${uppercase_input_file}_H

#include <string>

const std::string html_${output_file} = $wrapped_html_content

#endif // HTML_${uppercase_input_file}_H
EOF

echo "Minified C++ variable saved to ${output_file}.h"
