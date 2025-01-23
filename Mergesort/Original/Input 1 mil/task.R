
start_run_time <- Sys.time()
library(Rcpp)

Rcpp::sourceCpp("merge_sort.cpp")

print("Loading array")
arr <- load("one_million_array.rda")
print("Loaded array")
head(int_arr)

start_func_time <- Sys.time()
sorted_arr <- mergeSort(int_arr)
end_func_time <- Sys.time()

end_run_time <- Sys.time()

print("Execution COMPLETED")

head(sorted_arr)

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))