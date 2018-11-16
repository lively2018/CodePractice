#include <iostream>
#include <limits.h>
using namespace std;

int main(void)
{
	int n_int = INT_MAX;
	short n_short = SHRT_MAX;
	long n_long = LONG_MAX;

	cout << " int is " << sizeof(int) << " bytes.\n";
	cout << " short is " << sizeof(short) << " bytes.\n";
	cout << " long is " << sizeof(long) << " bytes.\n";

	cout << " Maximum values:\n";
	cout << " int: " << n_int << "\n";
	cout << " short: " << n_short << "\n";
	cout << " long: " << n_long << "\n";

	cout << "Minimum int value = " << INT_MIN << "\n";

	return 0;
}
