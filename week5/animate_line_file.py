import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

import pandas as pd
import math as m
import random

from pathlib import Path
import sys


def execute_time_step(data, rope, i):
    """This function carries out the update process for the animation.  
    All points on the string 'follow the leader', except for point 0, 
    which is 'driven' by in a simple harmonic oscillator

    Args:
        rope (list of Line2D): the plot showing the data to be updated
        y_positions (1-d NumPy array): the underlying data to be ploted
        time (float): the current time in the animation
    """
    # updates the positions
    x_positions, y_positions = extract_position(data, i=i)

    # updates the plotted data in the animation
    rope.set_data(x_positions, y_positions)
    return i


def reset_plots(rope, y_positions):
    """This function resets the data in the plot when the plotting 
    loop is completed.

    Args:
        rope (list of Line2D): the plot showing the data to be updated
        y_positions (1-d NumPy array): the underlying data to be ploted
    """
    # Calls the initial function to set up the string
    x_positions, y_positions = configure_rope()

    # sets the data for the rope to the initial value
    rope.set_data(x_positions, y_positions)


def animate(time, data, rope):
    """This function carries out the animation steps and resets the 
    plots at the start of the loop if allowed to run.

    Args:
        time (float): current timestamp in the animation
        rope (list of Line2D): the plot showing the data to be updated
        y_positions (1-d NumPy array): the underlying data to be ploted

    Returns:
        rope, (tuple of (lists of Line2D)): iterable collection of the 
            plots which have been animated in this function. 
            Note that even if only one plot is included in the animation, 
            this must still be returned as an iterable, speciied by a 
            trailing comma. 
    """
    # updates the positions
    x_positions, y_positions = extract_position(data, i=time)

    # updates the plotted data in the animation
    rope.set_data(x_positions, y_positions)
    
    # return a tuple of plots (lists of Line2D)
    return rope,


def configure_animation(frame_count = 125, fps = 25):
    """This function sets up the variables reauored for the animations in
    Python. These are returned as a tuple which must be parsed by the 
    calling process.

    Keyword Args:
        frame_count (number): Intended number of frames the animation 
            will run for. After this time, the animation will loop
            Defaults to 125
        fps (number): Intended number of frames per second in the
            animation. This number is returned as well.
            Defaults to 25 

    Returns:
        times (np array of floats): the time stamps (in seconds) for each 
            step in the animation. In this program, these are directly 
            mapped to the actual time of the animation, but this is not 
            required by FuncAnimation().
        interval (int): the number of milliseconds between frames, to a 
            minimum of 1ms. Used to configure the animation. In this 
            program, this is matched to the framerate at which the 
            animation is saved, but this is not required by ani.save()
        fps (number): the number of frames per secdond to be used in the
            final version of the animation saved to file.
    """
    # calculates the total run time in the animation
    run_time = frame_count/fps 

    # calculates the interval in milliseconds between frames, minimum 1
    interval = max([int(1000 / fps), 1])

    # calculates the time step in seconds to be used between frames
    time_step = interval / 1000

    # creates a numpy array of the times in the animation
    times = np.linspace(0., run_time, int(frame_count))

    # returns a tuple with the times, interval and frames per second
    return times, interval, fps


def configure_rope(length=50, end_point=True, step_size=1, start = 0):
    """Configures the parameters of the string which is to be used in 
    the animation.

    Keyword Args:
        length (number): the length of the string to simulate
            Defaults to 50
        end_point (bool): whether to include a final point at 
            point_labels = length. 
            Defaults to True
        step_size (number): the distance between simulated points
            Defaults to 1
        start (number): the position of the start of the string.
            Defaults to 0

    Returns:
        x_positions (Numpy array of floats): initial values for the
            x-positions of the points on the string
        y_positions (Numpy array of floats): initial values for the 
            y-positions of the points on the string
    """
    # calculates the number of points to simulate
    points = int((length/step_size) + end_point)

    # calculates the position of the end of the string
    stop = start + length

    # calculates the x-positions of the points on the string
    x_positions = np.linspace(start=start, stop=stop, num=points, endpoint=end_point)

    # sets the initial positions of the simulated points to zero
    y_positions = np.zeros(points)

    # returns a tuple containing the positions of the points on the string
    return x_positions, y_positions


def configure_plot(x_positions, y_positions):
    """This function sets up the plot for the animation.  It sets the
    initial x and y values, the title and the limits for the axes.

    Args: 
        x_positions (Numpy array of floats): initial values for the
            x-positions of the points on the string
        y_positions (Numpy array of floats): initial values for the
            y-positions of the points on the string

    Returns:
        fig (matplotlib.figure.Figure): the figure to be animated
        rope (list of Line2D): the plot showing the data to be animated
    """
    # creates the figure and the axis to be plotted
    fig, (ax1) = plt.subplots(1, 1)

    # sets the overall title of the plot
    fig.suptitle('Simulation of Vibrations in a string')

    # sets the x and y limits to be plotted
    ax1.set_ylim(-1.1, 1.1)

    # creates the plot which will be animated
    rope, = ax1.plot(x_positions, y_positions, "o", markersize=5, color="green", label="points on string")

    # creates the legend entry
    ax1.legend(handles=[rope], bbox_to_anchor=(0, 0), loc='upper left')

    # returns the figure and the plot to be animated
    return fig, rope


def get_data(filename, other=2):
    """This function reads the data from the csv file and returns it,
    together with data properties which are used elsewhere in the 
    program. 

    Args:
        filename (string): the path to the file containing the data

    Keword Arguments:
        other (int): the number of columns in the dataframe that
            represent data other than the position measurements
            Defaults to 2

    Returns:
        data (Pandas Dataframe): a dataframe with rows for each point
            in time and a column for every point on the string that
            we are modelling
        num_positions (int): the number of x-positions on the line
            that are being modelled
        num_times (int): the number of samples of the positions in 
            the supplied data.  Will be used as frame count later.

    """
    # tries to read the data from the file into a pandas dataframe
    try:
        data = pd.read_csv(filename, skipinitialspace=True)
    # if that file is not found
    except FileNotFoundError:
        # explains what has gone wrong
        print("The file you have specified, {} does not exist. Have you given the correct path to the file?".format(filename))
        # exits with error status
        exit(-1)

    # calculates how many positions have been measured
    num_positions = len(data.columns)-other

    # calculates how many samples were taken
    num_times = len(data)

    # returns the data and meta data
    return data, num_positions, num_times


def extract_position(data, i=0, other=2):
    """This function extracts the position of the points on the string
    from a dataframe for a given row.  The user can specify the number
    of other columns that are present in the dataframe

    Args:
        data (Pandas Dataframe): a dataframe with rows for each point
            in time and a column for every point on the string that
            we are modelling

    Keyword Arguments:
        i (int): the index representing the row of the dataframe
            Defaults to 0
        other (int): the number of columns in the dataframe that 
            represent data other than the position measurements
            Defaults to 2
    """
    # converts a row of the table into a value that can be parsed
    row = data.iloc[i]

    # gets the y positions from elements other to end of the row
    # flattens this into a numpy array and flattens it to be 
    # suitable for use with the plotting routines
    y_positions = np.array(row[other:]).flatten()

    # makes the x positions from the length of the row
    x_positions = np.arange(len(row)-other)

    # returns the x and y positions
    return x_positions, y_positions


def check_args():
    """This function reads and validates the command line arguments.
    Expects an input CSV file and an output GIF path.

    Returns:
        input_file (string): path to the input CSV file
        output_file (string): path to save the output GIF
    """
    # checks for the correct number of arguments
    if len(sys.argv) == 3:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
    elif len(sys.argv) == 2:
        # if only input is given, default output to same directory
        input_file = sys.argv[1]
        output_file = input_file.replace('.csv', '.gif')
    else:
        print("ERROR: Incorrect number of arguments!")
        print("Correct use: python3 {} [INPUT_CSV] [OUTPUT_GIF]".format(sys.argv[0]))
        print("Example: python3 {} week5/output/string_wave.csv week5/output/string_wave.gif".format(sys.argv[0]))
        exit(-1)

    # checks the input file has csv extension
    if not input_file.endswith('.csv'):
        print("The file you have specified, {} does not appear to be a csv file.".format(input_file))
        exit(-1)

    return input_file, output_file


def main():
    """This is the main function that executes the rest of the program
    Using a main function instead of executing directly in the global 
    namespace allows for local variables and better control of scope.
    """
    # gets the input and output paths from the command line
    input_file, output_file = check_args()

    # gets the data and its dimensions from the file
    data, num_positions, num_times = get_data(input_file)

    # sets up the variables to manage the animation
    times, interval, fps = configure_animation(frame_count=num_times)

    # sets up the points on the string 
    x_positions, y_positions = extract_position(data)

    # configures the initial state of the plot, including x and y positions of points on the string
    fig, rope = configure_plot(x_positions, y_positions)

    # initialises the python animation 
    ani = animation.FuncAnimation(fig, animate, num_times, interval=interval, blit=True,
                                  fargs=(data, rope))

    # saves the animation to the user specified output path
    ani.save(filename=output_file, writer="pillow", fps=fps)
    print("Animation saved to {}".format(output_file))


# we use this convention to ensure that if we import functions from this script, it is not executed
if __name__ == "__main__":
    main()