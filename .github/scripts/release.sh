#!/bin/bash

file_path="platformio.ini"

# Replace -build_type = debug with build_type = release
#sed 's/build_type = debug/build_type = release/' $file_path > tmpfile && mv tmpfile $file_path

# Replace -DLOG_LEVEL=3 with -DLOG_LEVEL=0
#sed 's/-DLOG_LEVEL=3/-DLOG_LEVEL=0/' $file_path > tmpfile && mv tmpfile $file_path