import sys
def main(): 
	output = 0

	# checks if there are the right number of arguments
	try:

	# converts the first two arguments to integers
	
		in_arg = int(sys.argv[1])
		multiple = int(sys.argv[2])

	except: # (argc != 2)
	# raises an error
		raise Exception("Incorrect arguments.\nUsage: python repeat_adder.py [NUM] [NUM]\ne.g.\npython repeat_adder.py 2 3")
		
	# iterates over all numbers up to the input
	for i in range(in_arg):
		# adds the index to the output
		output = output + multiple
	
	# prints the output
	print("{}\n".format(output))

if __name__ == "__main__":
        main()
