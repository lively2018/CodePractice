#include <iostream>
using namespace std;

int main(void)
{
	char c = 'M';
	int i = c;
	cout << "The ASCII code for " << c << " is " << i << "\n";

	cout << "Add one o the character code:\n";
	c = c + 1;
	i = c;

	cout << "The ASCII code fore " << c << " is " << i << '\n';

	cout << "Displaying char c using cout.put(c): ";
	cout.put(c);

	cout.put('!');

	cout << "\nDone\n";
	return 0;
}

