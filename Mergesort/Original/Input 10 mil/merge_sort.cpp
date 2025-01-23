#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
NumericVector mergeSort(NumericVector arr) {
    int n = arr.size();
    if (n <= 1)
        return arr;

    // Find the midpoint and divide the array
    int mid = n / 2;
    NumericVector left(arr.begin(), arr.begin() + mid);
    NumericVector right(arr.begin() + mid, arr.end());

    // Recursively sort both halves
    left = mergeSort(left);
    right = mergeSort(right);

    // Merge the sorted halves back into the original array
    NumericVector result(n);
    int i = 0, j = 0, k = 0;

    while (i < left.size() && j < right.size()) {
        if (left[i] <= right[j]) {
            result[k++] = left[i++];
        }
        else {
            result[k++] = right[j++];
        }
    }

    // Copy remaining elements from the left array
    while (i < left.size()) {
        result[k++] = left[i++];
    }

    // Copy remaining elements from the right array
    while (j < right.size()) {
        result[k++] = right[j++];
    }

    return result;
}
