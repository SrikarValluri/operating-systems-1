To create the executable file, simply use this command:

gcc -pthread -lm -o line_processor threads.c

followed by:

./line_processor 

or

./line_processor < input_file > output_file
