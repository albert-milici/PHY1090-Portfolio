import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import pandas as pd
import sys


# target fps for playback
TARGET_FPS = 50


def configure_plot(x_positions, y_positions,y_peak):
    """Sets up the figure and line artist for the animation.

    Uses a larger figure and higher dpi than matplotlib's defaults, so the
    saved video is crisp on modern displays instead of 640x480 pixelated.

    Args:
        x_positions (np.ndarray): x-positions of the points on the string
        y_positions (np.ndarray): initial y-positions for frame 0

    Returns:
        fig (Figure): the figure to be animated
        rope (Line2D): the line artist whose y-data we update each frame
    """
    # figsize * dpi gives output pixel dimensions
    # (12, 6.75) * 150 = 1800 x 1012 — close to 1080p, no more blur
    fig, ax1 = plt.subplots(1, 1, figsize=(12, 6.75), dpi=150)

    fig.suptitle('Simulation of Vibrations in a string')

    # lock y limits (set_ylim already does this) AND x limits explicitly
    # so blit doesn't waste time re-checking autoscale bounds every frame
    # ax1.set_ylim(-1.1, 1.1)
    ax1.set_ylim(-y_peak, y_peak)
    ax1.set_xlim(x_positions[0], x_positions[-1])

    rope, = ax1.plot(x_positions, y_positions, "-", markersize=2,
                     color="green", label="points on string")

    ax1.legend(handles=[rope], bbox_to_anchor=(0, 0), loc='upper left')

    return fig, rope


def get_data(filename, other=2):
    """Reads the CSV file into a dataframe and reports its dimensions.

    Args:
        filename (str): path to the input CSV

    Keyword Args:
        other (int): number of non-position columns at the start
            (typically [#, time] = 2)

    Returns:
        data (DataFrame): the raw CSV data
        num_positions (int): number of spatial points on the string
        num_times (int): number of time samples (rows)
    """
    try:
        data = pd.read_csv(filename, skipinitialspace=True)
    except FileNotFoundError:
        print("The file you have specified, {} does not exist. "
              "Have you given the correct path to the file?".format(filename))
        exit(-1)

    num_positions = len(data.columns) - other
    num_times = len(data)

    return data, num_positions, num_times


def check_args():
    """Parses command line arguments.

    Usage:
        python3 script.py INPUT_CSV [OUTPUT_FILE] [STRIDE]

    STRIDE is optional — if omitted, it's auto-computed to hit TARGET_FPS
    over the physical duration stored in the CSV. Set STRIDE=1 to render
    every single row (slow, rarely useful).

    Returns:
        input_file (str): path to input CSV
        output_file (str): path to output video (default: same basename .mp4)
        stride (int or None): manual stride override, or None for auto
    """
    stride = None

    if len(sys.argv) == 4:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        try:
            stride = int(sys.argv[3])
            if stride < 1:
                raise ValueError
        except ValueError:
            print("STRIDE must be a positive integer.")
            exit(-1)
    elif len(sys.argv) == 3:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
    elif len(sys.argv) == 2:
        input_file = sys.argv[1]
        output_file = input_file.replace('.csv', '.mp4')
    else:
        print("ERROR: Incorrect number of arguments!")
        print("Correct use: python3 {} [INPUT_CSV] [OUTPUT_FILE] [STRIDE]".format(sys.argv[0]))
        print("Example: python3 {} week5/output/string_wave.csv "
              "week5/output/string_wave.mp4".format(sys.argv[0]))
        print("  (STRIDE optional; auto-computed if omitted)")
        exit(-1)

    if not input_file.endswith('.csv'):
        print("The file you have specified, {} does not appear to be "
              "a csv file.".format(input_file))
        exit(-1)

    return input_file, output_file, stride


def compute_stride(data, num_times, user_stride):
    """Picks a stride that keeps rendered frames ~= physical_duration * TARGET_FPS.

    If the CSV stores e.g. 10 time-units of simulation sampled at 1000 Hz,
    that's 10,001 rows — but only 500 are needed for a smooth 50 fps replay
    at 1:1 speed. Rendering the extra 9,500 is pure waste.

    Args:
        data (DataFrame): the full CSV
        num_times (int): number of rows
        user_stride (int or None): manual override; None triggers auto

    Returns:
        stride (int): take every Nth row
        physical_duration (float): from the CSV's time column
    """
    # column index 1 is the time column (matches C code's "#, time, y[0], ..." header)
    time_col = data.iloc[:, 1].to_numpy()
    physical_duration = float(time_col[-1] - time_col[0])

    if user_stride is not None:
        return user_stride, physical_duration

    # we want roughly (physical_duration * TARGET_FPS) frames rendered
    desired_frames = max(1, int(physical_duration * TARGET_FPS))
    stride = max(1, num_times // desired_frames)
    return stride, physical_duration


def main():
    input_file, output_file, user_stride = check_args()

    data, num_positions, num_times = get_data(input_file)

    # decide how many rows to actually render
    stride, physical_duration = compute_stride(data, num_times, user_stride)

    # pre-extract all y-positions as a 2D numpy array (frames x positions)
    # with stride applied — this is where the big speedup comes from
    y_all = data.iloc[::stride, 2:].to_numpy()
    # x_all = np.arange(num_positions)
    x_all = np.linspace(0, 50.0, num_positions)
    num_frames = len(y_all)

    print("CSV: {} rows, physical duration {:.3f} time units.".format(
        num_times, physical_duration))
    print("Stride: {} -> rendering {} frames at {} fps "
          "({:.2f}s of video).".format(
              stride, num_frames, TARGET_FPS, num_frames / TARGET_FPS))

    interval = max(int(1000 / TARGET_FPS), 1)

    y_peak = np.max(np.abs(y_all)) * 1.1

    fig, rope = configure_plot(x_all, y_all[0], y_peak)


    # set_ydata is marginally faster than set_data since x_all never changes —
    # one less array assignment per frame. Over thousands of frames it adds up.
    def animate(i):
        rope.set_ydata(y_all[i])
        return rope,

    ani = animation.FuncAnimation(fig, animate, num_frames,
                                  interval=interval, blit=True)

    # write the file — use ffmpeg for mp4 (fast + high quality),
    # pillow for gif (slow but matches legacy workflows)
    if output_file.endswith('.mp4'):
        # -crf 18 is "visually lossless" h.264; default is 23
        # yuv420p ensures playback in every browser and player
        ani.save(filename=output_file, writer="ffmpeg", fps=TARGET_FPS,
                 extra_args=['-vcodec', 'libx264', '-pix_fmt', 'yuv420p',
                             '-crf', '18'])
    elif output_file.endswith('.gif'):
        ani.save(filename=output_file, writer="pillow", fps=TARGET_FPS)
    else:
        ani.save(filename=output_file, fps=TARGET_FPS)

    print("Animation saved to {}".format(output_file))


if __name__ == "__main__":
    main()