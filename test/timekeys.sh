#! /bin/sh

# Copyright 2013 Nedim Srndic
# 
# This file is part of rsa - the RSA implementation in C++.
#
# rsa is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# rsa is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with rsa.  If not, see <http://www.gnu.org/licenses/>.

## rsa testing script

if test $# -eq 0
then ## There are no params, default values are used.
	a=8;
	b=20;
elif test $# -eq 2 && test $1 -lt $2
then ## There are two params.
	a=$1;
	b=$2;
else ## Discard params and quit.
	echo "test.sh: Zero or two numeric parameters required." 1>&2;
	exit 1;
fi
	
if test -e timekeys.dat
then
	rm timekeys.dat;
fi

if test -e keys.dat
then
	rm keys.dat;
fi

while test $a -le $b
do
	/bin/echo -n "$a " 1>>timekeys.dat;
	/usr/bin/time --append --output=timekeys.dat --format="%U" ../Release/rsa genkey $a 1>>keys.dat;
	sleep 1;
	/bin/echo -n "$a ";
	a=$((a+1));
done

## End file with newline to make sure Scilab can read the last line.
echo >> timekeys.dat;

## Display a graph of results.
scilab -f timekeys.sci;

exit 0;
