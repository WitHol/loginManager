#!/bin/bash

gcc src/main.cpp src/tui.cpp src/authenticator.cpp -o main -lstdc++ -lncurses -lpam_misc -lpam