#!/bin/bash

if [ "$1" == "--clean" ]; then
    rm -rf "$(pwd)/uml/images"
else
    mkdir "$(pwd)/uml/images"
    plantuml -tsvg -o "$(pwd)/uml/images" "$(pwd)/uml/src"
fi