#!/bin/bash

echo "-> Building the parser!"
ferite-yacc -Jclass=CParser c-parser.y

echo "-> Running program!"
ferite c.fe -- t.c