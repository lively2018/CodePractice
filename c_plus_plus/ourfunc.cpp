#include <iostream>
using namespace std;

void simon(int n)
{
	cout << "Simon says touc your toes " << n << " times.\n";
}

int main(void)
{
	simon(3);
	cout << "Pick an integer: ";
	int count;
	cin >> count;
	simon(count);

	return 0;
}
