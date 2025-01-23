#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
NumericVector multiply_vectors(NumericVector v1, NumericVector v2) {
  int size = v1.size();
  NumericVector result(size);
  
  for (int i=0; i < size; i++) {
    result[i] = v1[i] * v2[i];
  }
  
  return result;
}
