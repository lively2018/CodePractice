#include <iostream>
#include <limits.h>
using namespace std;

#define ZERO 0

int main(void)
{
	int sam = INT_MAX;
	unsigned sue = sam;

	cout << "Sam has " << sam << " dollars and Sue has " << sue;
	cout << " dollars deposited.\nAdd $1 to eah account.\nNow ";
	sam = sam + 1;
	sue = sue + 1;
	cout << "Sam has " << sam << " dollars and Sue has " << sue;
	cout << " dollars depositied.\nPoor Sam!\n";
	sam = ZERO;
	sue = ZERO;
	cout << "Sam has " << sam << " dollars  and Sue has " << sue;
	cout << " dollars depositied.\n";
	cout << "Take $1 from each account.\nNow ";
	sam = sam - 1;
	sue = sue - 1;
	cout << "Sam has " << sam << " dollars and Sue has " << sue;
	cout << " dollars depositied.\nLucky Sue!\n";
	return 0;
}

