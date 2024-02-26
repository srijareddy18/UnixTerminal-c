In this project I have implemented mytree, mymtimes, mytime, mytimeout as external commands and they can be invoked by calling as follows:

  “mytree [cmd]”
  “mytime cmd”
  “mymtimes [cmd]”
  “mytimeout snds cmd”

As these are inside my project directory if we navigate to another folder and try to execute any of these they may not execute.

When mytoolkit encounters any special characters it may behave differently, as they are not handled in my project.

The input for the mytoolkit should be properly spaced as I am parsing the input based on the space character. Sample inputs which will work are as follows:
	mytree test
	mymtimes test
	ls | grep print | wc
	ls > sample.txt
	wc < sample.txt
	wc > sample.txt > sample2.txt

While generating the executables there are few warnings but the program main functionality is not affected and there are no errors generated.
