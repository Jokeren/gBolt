BEGIN {
	total = 0;
	visit = 0;
}

{
	if (($1 == "v") && ($3 == "5")) 
	{
		print "here";
	}
}

END {

	if (visit == 1) {
		++total;
	}
	print total;

}
