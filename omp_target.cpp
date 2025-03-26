#include <iostream>
#include <omp.h>
#include <bits/stdc++.h>

using namespace std;

#pragma omp requires reverse_offload

class test {
    public:
        string testing; 
};

int main() {
    int x = 10;  // Initialize variable

    test *c = new test;
    c->testing = "check";

    std::cout << "Before offload, x = " << x << std::endl;

    for(int i = 0 ; i < 2 ; i++){
        #pragma omp target firstprivate(c) map(tofrom: x)// map(from: x)
        {
            //for(int i = 0 ; i < 2 ; i++){
                cout<<"Value of x in target is: "<<x<<endl;
                c->testing = "hello";
                x += 32;  // Modify value on the device
            
                
            //}
        
        }

        cout<<x<<endl;

        #pragma omp target device(ancestor:1) map(tofrom: x) //map(to: x)
        {
            cout<<"Value of x in host is: "<<x<<endl;
            c->testing = "hello2";
            x += 2;  // Further modify x on the host
            //std::cout<<"testing execution"<<std::endl;
        }
    }

    std::cout << "After offload, x = " << x << std::endl;

    return 0;
}
