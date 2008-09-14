// This code opens and reads data from a file named 'timekeys.dat', 
// plots the data to a 2d graph and exports it as an image file 
// with the filename 'timekeys.gif'.

// Open 'timekeys.dat', an existing file.
// 'timekeys.dat' contains execution data in the following format:
//
// n1   t(n1)
// n2   t(n2)
// ...
// nx   t(nx)
//

in = file('open', 'timekeys.dat', 'old');
// Read all the rows and two columns from the file.
xy = read(in, -1, 2);
// x is the first, y the second column.
x = xy(:, 1);
y = xy(:, 2);
// Prepare the graphics and plot a 2d graph.
scf();
plot2d2(x, y);
xtitle("Key generation duration");
axes = gca();
axes.x_label.text = "Key length (decimal digits)";
axes.y_label.text = "Time (seconds)";
// Export the graph to the 'timekeys.gif' image file.
xs2gif(0, 'timekeys.gif');
