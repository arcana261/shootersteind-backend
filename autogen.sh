#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"


cd $DIR && git fetch --tags
cd $DIR && git shortlog -e -s -n > AUTHORS
cd $DIR && git tag -l -n1000 > NEWS
cd $DIR && cp README.md README
cd $DIR && cp LICENSE COPYING

if [ x$1 != "--no-changelog" ]; then
    echo "" > $DIR/ChangeLog
    for hash in `cd $DIR && git log --pretty=format:"%H"`; do
        echo `cd $DIR && git log -1 --date=short --pretty=format:"%ad%x09%an%x09<%ae>" $hash` >> $DIR/ChangeLog
        echo "" >> $DIR/ChangeLog
        for file in `cd $DIR && git show --pretty="" --name-only $hash`; do
            echo -e '\t' $file: `cd $DIR && git log -1 --pretty=format:"%s" $hash` >> $DIR/ChangeLog
        done
        echo "" >> $DIR/ChangeLog
    done
else
    cp $DIR/NEWS $DIR/ChangeLog
fi

cd $DIR && autoreconf --verbose --install
