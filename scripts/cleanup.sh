#!/bin/sh

#rm -f log

for file in $(find . -iregex ".*\/src\/.*\.[cht]x?t?$"); do
    new_fn=$(echo $file | sed 's/\/[Ss][Rr][Cc]\//\//')
    new_fd=$(echo $new_fn | sed 's/\(.*\)\/.*/\1/')
    mkdir -p $new_fd
    git mv $file $new_fn
done

for file in $(find . -iregex ".*\/inc\/.*\.[cht]x?t?$"); do
    new_fn=$(echo $file | sed 's/\/[Ii][Nn][Cc]\//\//')
    new_fd=$(echo $new_fn | sed 's/\(.*\)\/.*/\1/')
    mkdir -p $new_fd
    git mv $file $new_fn
done

for file in $(find . -iregex ".*\/include\/.*\.[cht]x?t?$"); do
    new_fn=$(echo $file | sed 's/\/[Ii][Nn][Cc][Ll][Uu][Dd][Ee]\//\//')
    new_fd=$(echo $new_fn | sed 's/\(.*\)\/.*/\1/')
    mkdir -p $new_fd
    git mv $file $new_fn
done

for file in $(find . -iregex ".*\.[chSst]x?t?$"); do
    vi $file -c call\ SetFFUnix\(\) -c wq
    git add $file
done
