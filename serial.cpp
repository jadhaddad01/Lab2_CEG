#include <iostream>
using namespace std;

int arr[] = {1, 2, 3, 4, 5, 6, 7, 8};   // doit etre nombre d'élements exponentiel: 2, 4, 8, 16 etc.
int len = sizeof(arr) / sizeof(arr[0]); // longueur de l'array

int reductionSumRecursif(int tableau[], int a) // arr1 divise les array en deux avec a comme length pour récursivité
{
    // tableau divisé a un élement on retourne l'élément
    if (a == 1)
    {
        return tableau[0]; // si le array est juste un element on le retourne
    }

    // sinon on divise le tableau en deux encore une fois pour que ca devient plus petit
    int arr1[a / 2], arr2[a / 2], k;
    for (k = 0; k < (a / 2); ++k)
    {
        arr1[k] = tableau[k];
    }
    for (; k < a; ++k)
    {
        arr2[k - (a / 2)] = tableau[k];
    }

    // on ajoute les tableaux divisés ensemble pour avoir la somme
    return reductionSumRecursif(arr1, a / 2) + reductionSumRecursif(arr2, a / 2);
}

int main()
{
    // on imprime le tableau initial
    for (int i = 0; i < len; i++)
    {
        cout << arr[i] << " ";
    }

    cout << "\n";

    // on utilise la fonction récursive pour faire la réduction parrallele
    int sum = reductionSumRecursif(arr, len);
    cout << "Somme des éléments: " << sum;

    cout << "\n";

    return 0;
}