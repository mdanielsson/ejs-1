function x(a)
{
	return a;
}

var i;
for (i = 0; i < 15000000; i = i + 1) {
	j = x(i);
}
print(j);
