#include <iostream>
#include <climits>
using namespace std;

int main()
{
    double prr;
    prr = 1/static_cast<double>(ULLONG_MAX);
    cout << prr << endl;
    return 0;

}