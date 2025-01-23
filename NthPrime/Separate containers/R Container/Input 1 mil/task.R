setwd('/app')
library(optparse)
library(jsonlite)

library(httr)
library(jsonlite)
# library(Rcpp)
library(readr)



print('option_list')
option_list = list(

)


opt = parse_args(OptionParser(option_list=option_list))

var_serialization <- function(var){
    if (is.null(var)){
        print("Variable is null")
        exit(1)
    }
    tryCatch(
        {
            var <- fromJSON(var)
            print("Variable deserialized")
            return(var)
        },
        error=function(e) {
            print("Error while deserializing the variable")
            print(var)
            var <- gsub("'", '"', var)
            var <- fromJSON(var)
            print("Variable deserialized")
            return(var)
        },
        warning=function(w) {
            print("Warning while deserializing the variable")
            var <- gsub("'", '"', var)
            var <- fromJSON(var)
            print("Variable deserialized")
            return(var)
        }
    )
}



print("Running the cell")
library(jsonlite)
library(httr)
library(readr)

start_run_time <- Sys.time()
# library(Rcpp)
# sourceCpp("nthprime.cpp")


x <- 1000000

{# Convert each param to JSON format
start_func_time <- Sys.time()
json_n <- toJSON(x)

list_nthPrime <- list(
	n = json_n
)
json_nthPrime <- toJSON(list_nthPrime, auto_unbox=TRUE)
url_nthPrime <- "http://localhost:8080/findNthPrime"

response_nthPrime <- POST(
	url_nthPrime,
	body = json_nthPrime,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_func_time <- Sys.time()


content_nthPrime <- content(response_nthPrime)

nthPrime <- content_nthPrime
nthPrime
}# ---- END OF AUTO-GENERATED CODE ----- #

end_run_time <- Sys.time()

print(paste("Nth prime: ", nthPrime))


run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))
