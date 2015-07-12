#!/bin/bash

gcc -lwiringPi -o app app.c -std=c99 && sudo ./app
