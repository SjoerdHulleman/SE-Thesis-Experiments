v1 <- 1:1000000
v2 <- 1000000:1

print("Created vectors")
start_run_time <- Sys.time()

library(Rcpp)

sourceCpp("multiply_vector.cpp")

start_func_time <- Sys.time()
result <- multiply_vectors(v1, v2)
end_func_time <- Sys.time()

end_run_time <- Sys.time()

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))

