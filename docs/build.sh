#!/bin/bash

if [ "$1" == "--clean" ]; then
    rm source_code/uml/*.svg &>/dev/null

    rm ../assets/help/help.qhc &>/dev/null
    rm ../assets/help/help.qch &>/dev/null
    rm ../assets/help/*.md &>/dev/null
    rm -rf ../assets/help/images &>/dev/null
    rm -rf ../assets/help/icons &>/dev/null
    rm -rf source_code/html &>/dev/null
    rm -rf source_code/latex &>/dev/null
else
    # Ensure we work from a clean setup
    ./build.sh --clean

    # Create images
    plantuml -tsvg -o . source_code/uml

    # Build help pdf file
    pushd help &>/dev/null
    for f in $(ls | egrep '[0-9]+.*'); do
        cat $f >> help.md
        echo -e "\n\n" >> help.md
        # Copy files to help for internal application help
        cp $f ../../assets/help/$f
    done
    sed -i 's/\[\([0-9a-zA-Z _-]\+\)\]([0-9a-zA-Z_-]\+\.md)/\1/g' help.md
    pandoc --toc --toc-depth=6 help.md -o help.pdf
    rm help.md
    popd &>/dev/null

    # Build markdown files for internal help
    cat help/01_overview.md >> ../assets/help/index.md
    echo -e "\n\n" >> ../assets/help/index.md
    cat help/index.md >> ../assets/help/index.md

    cat help/05_dataflow.md >> ../assets/help/05_dataflow.md
    echo -e "\n\n" >> ../assets/help/05_dataflow.md
    cat help/dataflow_nodes.md >> ../assets/help/05_dataflow.md

    # Copy image assets for application
    cp -r help/images ../assets/help
    cp -r help/icons ../assets/help

    if [ "$1" == "--wiki" ]; then
        pushd ../assets/help &>/dev/null
        for i in $(ls *.md); do
            sed -i 's/(images\//(https:\/\/github\.com\/Drew-S\/TerrainGenerator\/blob\/master\/docs\/help\/images\//g' $i
            sed -i 's/(icons\//(https:\/\/github\.com\/Drew-S\/TerrainGenerator\/blob\/master\/docs\/help\/icons\//g' $i
        done    
        popd &>/dev/null
    fi
    
    # Generate help for application
    pushd ../assets/help &>/dev/null

    qcollectiongenerator help.qhcp

    popd &>/dev/null

    pushd ..  &>/dev/null

    doxygen doxygen.conf
    
    popd &>/dev/null
fi