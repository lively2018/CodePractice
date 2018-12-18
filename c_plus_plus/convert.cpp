#include <iostream>
using namespace std;

int stonetolb(int);
int main(void)
{
	int stone;
	cout << "Enter the weight in stone: ";
	cin >> stone;
	int pounds = stonetolb(stone);
	cout << stone << " stone are ";
	cout << pounds << " pounds.\n";
	return 0;
}
int stonetolb(int sts)
{
	return 14 * sts;
}
