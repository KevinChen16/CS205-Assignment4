#include <iostream>
#include "matrix.h"

using namespace std;

int main() {
    Matrix a;
    cout<<"matrix a:"<<endl;
    cin>>a;
    cout<<"input scalar p: ";
    float p;
    cin>>p;
    Matrix a1;
    a1 = a*p;
    cout<<"a * p = "<<endl<<a1;
    a1 = p*a;
    cout<<"p * a = "<<endl<<a1;
    Matrix b; Matrix c;
    cout<<"matrix b:"<<endl;
    cin>>b;
    c=a*b;
    cout<<"a * b = "<<endl<<c;
    delete &a;
    delete &a1;
    delete &b;
    delete &c;
    return 0;
}
