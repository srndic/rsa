#! /bin/sh
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

rm timekeys.dat;

exit 0;
