#!/bin/sh

export DISPLAY=:0
/usr/bin/raspivid -t 0 -f -op 200
