start_run_time <- Sys.time()
library(Rcpp)


sourceCpp("base_line.cpp")

x <- 1
start_func_time <- Sys.time()
result <- doNothing(x)
end_func_time <- Sys.time()

end_run_time <- Sys.time()

print(result)

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))