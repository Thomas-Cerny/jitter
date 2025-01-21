<h3>This is a program for sorting ping values into 12 intervals and the calculation of packet-to-packet jitter, mean and dispersion of packets.</h3>

<h4>Use:</h4>
<p>
It is to be used with a file containing ping messages and called input.txt. It has to be in the same directory as the build program.
The program uses the ping time values to calculate 12 intervals in which it then sorts the ping times and outputs the number of packets in each interval,
after which it lets the user delete a few additional small or big values, if forming way too many empty intervals.
It also calculates the jitter, mean and dispersion.
</p>

<h4>Additional notes:</h4>
<p>
The .cpp file has to be compiled with a C++ compiler supporting at least C++11.
Since the data type "short" is used to avoid redundant calculation of ping value count, the maximum number of ping messages is 65535.
The calculated intervals, mean and dispersion are rounded to 2 decimal places. The jitter is not rounded since it is often way too small.
</p>
