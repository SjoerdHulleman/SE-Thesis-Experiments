#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
NumericVector doNothing(NumericVector x) {
  return x;
}
