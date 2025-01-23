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

v1 <- 1:10000000
v2 <- 10000000:1

print("Created vectors")
start_run_time <- Sys.time()

# library(Rcpp)

# sourceCpp("multiply_vector.cpp")

{# Convert each param to JSON format
start_func_time <- Sys.time()
json_v1 <- toJSON(as.vector(v1))
json_v2 <- toJSON(as.vector(v2))

list_result <- list(
	v1 = json_v1,
	v2 = json_v2
)
json_result <- toJSON(list_result, auto_unbox=TRUE)
url_result <- "http://localhost:8080/multiply_vectors"

response_result <- POST(
	url_result,
	body = json_result,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_func_time <- Sys.time()

content_result <- content(response_result)

result <- as.vector(content_result)
result <- as.vector(as.numeric(result))
}# ---- END OF AUTO-GENERATED CODE ----- #

end_run_time <- Sys.time()

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func_time <- as.numeric(end_func_time - start_func_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func time: ", func_time))
