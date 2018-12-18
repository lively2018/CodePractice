#include <iostream>
using namespace std;
int main(void)
{
	const int ArSize = 20;
	char name[ArSize];
	char dessert[ArSize];

	cout << "Enter your name:\n";
	cin.get(name, ArSize);
	cout << "Enter your favorite dessert:\n";
	cin.get(dessert, ArSize);
	cout << "I ahve some delicious " << dessert;
	cout << " for you, " << name << ".\n";

	return 0;
}
