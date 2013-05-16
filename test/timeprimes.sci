// Copyright 2013 Nedim Srndic
// 
// This file is part of rsa - the RSA implementation in C++.
//
// rsa is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// rsa is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with rsa.  If not, see <http://www.gnu.org/licenses/>.

// This code opens and reads data from a file named 'timeprimes.dat', 
// plots the data to a 2d graph and exports it as an image file 
// with the filename 'timeprimes.gif'.

// Open 'timeprimes.dat', an existing file.
// 'timeprimes.dat' contains execution data in the following format:
//
// n1   t(n1)
// n2   t(n2)
// ...
// nx   t(nx)
//

in = file('open', 'timeprimes.dat', 'old');
// Read all the rows and two columns from the file.
xy = read(in, -1, 2);
// x is the first, y the second column.
x = xy(:, 1);
y = xy(:, 2);
// Prepare the graphics and plot a 2d graph.
scf();
plot2d2(x, y);
xtitle("Prime generation duration");
axes = gca();
axes.x_label.text = "Prime length (decimal digits)";
axes.y_label.text = "Time (seconds)";
// Export the graph to the 'timeprimes.gif' image file.
xs2gif(0, 'timeprimes.gif');
