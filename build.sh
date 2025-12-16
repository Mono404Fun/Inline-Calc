#!/usr/bin/env bash

# some colors
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
CYAN="\033[36m"
RESET="\033[0m"

SRC="advanced-calc.cpp" # our cute calculator
OUT_DIR="build"         # output directory
PROJECT="app"           # default name

if [[ -f "$SRC" ]]; then
    BASE=$(basename "$SRC" .cpp)
    PROJECT="$BASE"
fi

CXX="g++" # used compiler
CXXFLAGS="-std=c++23 -Os -s" # compile with c++23 and add some extra optimiations
DEBUG_FLAGS="-g -O0" # for debugging (debug mode)
RELEASE_FLAGS="-O3 -march=native" # release mode

MODE="release" # default mode set to release

print_help() { # helper function to print help
    echo -e "${CYAN}Usage:${RESET} ./build.sh [debug|release|run]"
    echo -e "  debug    Build with debugging symbols"
    echo -e "  release  Build optimized (default)"
    echo -e "  run      Build & immediately run"
}

# add simple parsing conditions (1 arg)
if [[ "$1" == "debug" ]]; then # if first arg is "debug"
    MODE="debug" # set mode to debug
elif [[ "$1" == "run" ]]; then # if it's run
    MODE="run" # set it to run mode, so it will run the executable once built
elif [[ "$1" == "help" || "$1" == "--help" ]]; then # if it's help
    print_help # call func to print help
    exit 0 # exit directly
fi

clear

START_TIME=$(date +%s%3N)
echo -e "${CYAN}==> Building project '${PROJECT}' in ${YELLOW}${MODE}${CYAN} mode...${RESET}"

mkdir -p "$OUT_DIR" # make a director for output built executable

#build project according to build mode
if [[ "$MODE" == "debug" ]]; then
    $CXX $CXXFLAGS $DEBUG_FLAGS "$SRC" -o "$OUT_DIR/$PROJECT" # build project
elif [[ "$MODE" == "release" || "$MODE" == "" ]]; then
    $CXX $CXXFLAGS $RELEASE_FLAGS "$SRC" -o "$OUT_DIR/$PROJECT"
elif [[ "$MODE" == "run" ]]; then
    $CXX $CXXFLAGS $RELEASE_FLAGS "$SRC" -o "$OUT_DIR/$PROJECT" || exit 1
    echo -e "${GREEN}==> Running...${RESET}" # if it's run mode, build then run
    echo ""
    "./$OUT_DIR/$PROJECT"
    exit 0
fi

END_TIME=$(date +%s%3N)        # record end time
DURATION=$((END_TIME - START_TIME))  # calculate duration in ms

# show a success output message
if [[ $? -eq 0 ]]; then
    echo -e "${GREEN}==> Build succeeded in ${YELLOW}${DURATION} ms${RESET}! Binary: ${CYAN}${OUT_DIR}/${PROJECT}${RESET}"
else
    echo -e "${RED}==> Build failed.${RESET}"
    exit 1
fi
