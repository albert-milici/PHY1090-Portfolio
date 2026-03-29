#!/bin/bash
# compiles, runs, and animates the improved string wave model

SRC="week5/src/string_wave_improved.c"
BIN="bin/string_wave_improved"
CSV="week5/output/string_wave_improved.csv"
VID="week5/output/string_wave_improved.mp4"
ANIM="week5/animate_line_file_video.py"

# parameters)
POINTS=${1:-500}
CYCLES=${2:-20}
SAMPLES=${3:-400}
K=${4:-2.0}
M=${5:-0.5}
DAMPING=${6:-0.5}
LENGTH=${7:-50.0}

# compiles if needed
if [ ! "$BIN" -nt "$SRC" ]; then
    echo "Compiling..."
    gcc "$SRC" -o "$BIN" -lm
fi

# runs the simulation
echo "Running: $POINTS points, $CYCLES cycles, $SAMPLES samples"
echo "Physics: k=$K m=$M damping=$DAMPING length=$LENGTH"
./$BIN $POINTS $CYCLES $SAMPLES "$CSV" $K $M $DAMPING $LENGTH

# generates the animation
echo "Generating animation..."
python3 "$ANIM" "$CSV" "$VID"