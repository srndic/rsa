#! /bin/sh
## rsa testing script

if test $# -eq 0
then ## There are no params, default values are used.
	a=3;
	b=9;
elif test $# -eq 2 && test $1 -lt $2
then ## There are two params.
	a=$1;
	b=$2;
else ## Discard params and quit.
	echo "test.sh: Zero or two numeric parameters required." 1>&2;
	exit 1;
fi
	
if test -e timeprimes.dat
then
	rm timeprimes.dat;
fi

if test -e primes.dat
then
	rm primes.dat;
fi

while test $a -le $b
do
	/bin/echo -n "$a " 1>>timeprimes.dat;
	/usr/bin/time --append --output=timeprimes.dat --format="%U" ../Release/rsa genprime $a 1>>primes.dat;
	## Need to wait for 1 second because the prime generator could otherwise 
	## generate the same numbers.
	sleep 1;
	/bin/echo -n "$a ";
	a=$((a+1));
done

## End file with newline to make sure Scilab can read the last line.
echo >> timeprimes.dat;

## Display a graph of results.
scilab -f timeprimes.sci;

exit 0;
