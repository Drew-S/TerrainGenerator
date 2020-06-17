#!/bin/bash

if [ "$1" == "--clean" ]; then
    rm -rf uml/images

    # for i in $(ls help/*.html); do
    #     rm $i
    # done

    rm help/help.qhc
    rm help/help.qch
else
    mkdir uml/images &>/dev/null
    plantuml -tsvg -o ../images uml/src

    # for i in $(ls help/*.md); do
    #     markdown $i > $(echo $i | sed 's/md$/html/g')
    # done

    pushd help &>/dev/null
    qcollectiongenerator help.qhcp
    popd &>/dev/null
fi