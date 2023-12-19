#include <iostream>

using namespace std;

int mysteryfunc(int numbers[][5], int arr_size)

{

    int total = 0;

    for(int i = 0; i < arr_size; i++)

    {

        int b = numbers[i][0];

        for(int j = 0; j < 5; j++)

        {

            b = numbers[i][j];

        }

        total +=b;

    }

    return total;

}

int main() {
    int arr[3][5] = {{17,5,42,-3,0},{-10,31,-1,5,4},{6,9,10,6,5}};
    int x = mysteryfunc(arr, 3);
    cout << x << endl;
    return 0;
}