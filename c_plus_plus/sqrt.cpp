#include <iostream>
#include <math.h>
using namespace std;

int main(void)
{
	double cover;
	cout << "Now many square feet of sheets do you have?\n";
	cin >> cover;
	double side;
	side = sqrt(cover);
	cout << "You can cover a square with sides of " << side;
	cout << " feet\nwith your sheets.\n";
	return 0;
}
