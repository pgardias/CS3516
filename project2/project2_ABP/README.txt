The procedure for running each program is the same, therefore this readme is the same for both programs. Any 
commands mentioned in this readme are in quotes, but should be typed in and executed without the quotes.

1. Compile the program by opening terminal/command prompt in the folder with the .c, .h, and makefile files and 
entering "make all". To clean the compiled program and any related object files, do the same thing, and enter 
"make clean".

2. To run the program, after it has been compiled, type "./p2" in the terminal/command prompt, and then follow 
the instructions. The program will request you to enter the number of messages to simulate, packet loss probability,
packet corruption probability, packet out-of-order probability, average time between messages, level of tracing, and 
whether or not you would like the actions to be randomized. The last input required will ask if you would like to run 
with bidirectionality, even though this program does not support it. This field must be set to 0.

----

Notes:

1. Sometimes the program continuously hung up mid-run or failed mid-run. This may have been related to a memory problem, 
as restarting my virtual machine allowed me to run the program without any problems. If the problem is related to memory, 
this should not be too big of an issue unless the program is being run many times. However, please try to run the program 
multiple times or restarting your machine if there are any problems.

2. There was no mention of what the output trace should actaully consist of in the assignment description, so I included 
the initial setup phase where input was requested by the program, and the result phase where the program printed how many 
messages were received correctly, incorrectly, etc. I chose to do this rather than include the entirety of the program 
output because the output for the Go Back N version was very long, and rather unnecessary to include the entirety of. 
However, if this is what is in fact required, it should still be accessible by running the program with the parameters 
shown in the attached .pdf file, and should produce similar results.