#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
int findNthPrime(int n) {
  int count = 0;
  int num = 1;
  
  while (count < n) {
    ++num;
    // Check if the number is prime
    bool isPrime = true;
    if (num <= 1) {
      isPrime = false;
    } else {
      for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
          isPrime = false;
          break;
        }
      }
    }
    
    if (isPrime) {
      ++count;
    }
  }
  
  return num;
}