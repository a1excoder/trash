import csv, sys

if len(sys.argv) < 2:
	print("Enter csv file name after .py")
	sys.exit(1)

fname = sys.argv[1]

try:
	csv_file = open(fname, 'r')
	csv_file_reader = csv.DictReader(csv_file)

	for row in csv_file_reader:
		print(f"{row['url']} | {row['username']} | {row['password']}")
	
	csv_file.close()
	
except FileNotFoundError:
	print(f"file '{fname}' not found")
	sys.exit(2)
except KeyError:
	print(f"file '{fname}' is not firefox password format file")
	sys.exit(3)
