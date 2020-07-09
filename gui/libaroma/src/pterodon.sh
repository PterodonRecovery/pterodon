#!/bin/bash

find . -type f -exec sed -i -e '/:_/s/ //g' -e '/:NULL/s/ //g' -e '/:0,/s/ //g' -e '/:_/s/^/./' -e '/:NULL/s/^/./' -e '/:0,/s/^/./' -e 's/:NULL/ = NULL/g' -e 's/:_/ = _/g' -e 's/:0,/ = 0,/g' -e 's/, O_RDWR,/, O_CREAT | O_RDWR,/g' {} \;




