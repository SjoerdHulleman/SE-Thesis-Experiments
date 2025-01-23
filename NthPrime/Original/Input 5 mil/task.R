start_run_time <- Sys.time()
library(Rcpp)
sourceCpp("nthprime.cpp")


x <- 5000000

start_func_time <- Sys.time()
nthPrime <- findNthPrime(x)
end_func_time <- Sys.time()


end_run_time <- Sys.time()

print(paste("Nth prime: ", nthPrime))


run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))
