#include <iostream>
using namespace std;

int arr[] = {3, 5, 7, 8, 2, 6, 9, 4};   // doit etre nombre d'élements exponentiel: 2, 4, 8, 16 etc.
int len = sizeof(arr) / sizeof(arr[0]); // longueur de l'array

int reductionSumRecursif(int tableau[], int a) // arr1 divise les array en deux avec a comme length pour récursivité
{
    if (a == 1)
    {
        return tableau[0]; // si le array est juste un element on le retourne
    }

    int arr1[a / 2], arr2[a / 2], k;
    for (k = 0; k < (a / 2); ++k)
    {
        arr1[k] = tableau[k];
    }
    for (; k < a; ++k)
    {
        arr2[k - (a / 2)] = tableau[k];
    }

    return reductionSumRecursif(arr1, a / 2) + reductionSumRecursif(arr2, a / 2);
}

int main()
{
    for (int i = 0; i < len; i++)
    {
        cout << arr[i];
    }

    cout << "\n";

    int sum = reductionSumRecursif(arr, len);
    cout << "The array sum is " << sum;

    cout << "\n";

    return 0;
}