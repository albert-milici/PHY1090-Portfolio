#!/bin/bash
# compiles, runs, and animates the improved string wave model
SRC="week5/src/string_wave_improved.c"
BIN="bin/string_wave_improved"
CSV="week5/output/string_wave_improved.csv"
MP4="week5/output/string_wave_improved.mp4"
GIF="week5/output/string_wave_improved.gif"
ANIM="week5/animate_line_file_video.py"

# simulation parameters
POINTS=${1:-500}
CYCLES=${2:-20}
SAMPLES=${3:-100}
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

# generate MP4 (fast via ffmpeg inside matplotlib)
echo "Generating MP4..."
python3 "$ANIM" "$CSV" "$MP4"

# GIF conversion parameters
GIF_SPEEDUP=4       # playback speed multiplier (2 = 2x faster, 4 = 4x faster, etc)
GIF_FPS=25          # frames per second in the final GIF
GIF_WIDTH=1000       # output width in pixels (height auto-scales)

# convert MP4 to GIF via two-pass palette (good quality, small file)
# setpts is the inverse of speedup: 2x faster = 0.5*PTS, 4x faster = 0.25*PTS
PTS_FACTOR=$(echo "scale=4; 1/$GIF_SPEEDUP" | bc)
echo "Converting to GIF (${GIF_SPEEDUP}x speed, ${GIF_FPS} fps, ${GIF_WIDTH}px wide)..."
ffmpeg -y -i "$MP4" \
    -vf "setpts=${PTS_FACTOR}*PTS,fps=${GIF_FPS},scale=${GIF_WIDTH}:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" \
    -loop 0 "$GIF"

echo "Done: $GIF"