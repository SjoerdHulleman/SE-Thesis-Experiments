setwd('/app')

start_run_time <- Sys.time()

library(optparse)
library(jsonlite)

library(fields)
library(httr)
library(inline)
library(jsonlite)
# library(onemap)
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


require(Rcpp)
require(inline)
require(fields)
# require(Rcpp)
# sourceCpp("source.cpp")


dat<-as.matrix(read.table("mouse.txt"))
{# Convert each param to JSON format
start_api_time_1 <- Sys.time()

json_geno <- toJSON(as.matrix(dat))

list_rec <- list(
	geno = json_geno
)
json_rec <- toJSON(list_rec, auto_unbox=TRUE)
url_rec <- "http://localhost:8080/est_bc_source"

response_rec <- POST(
	url_rec,
	body = json_rec,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_api_time_1 <- Sys.time()

content_rec <- content(response_rec)

rec <- do.call(rbind, as.matrix(content_rec))
rec <- matrix(as.numeric(rec), nrow = nrow(rec), ncol = ncol(rec))
}# ---- END OF AUTO-GENERATED CODE ----- #
# image.plot(rec, col=rev(tim.colors()))

source("simulate_diploid_populations.R")
dat.bc<-sim.pop.bc(n.ind = 150, n.mrk = 1000, ch.len = 100, missing = 0, n.ch = 1, verbose = FALSE)
# dat.bc
dat.bc.t<-dat.bc$geno
system.time(
{# Convert each param to JSON format

start_api_time_2 <- Sys.time()

json_geno <- toJSON(as.matrix(dat.bc.t))

list_rec <- list(
	geno = json_geno
)
json_rec <- toJSON(list_rec, auto_unbox=TRUE)
url_rec <- "http://localhost:8080/est_bc_source"

response_rec <- POST(
	url_rec,
	body = json_rec,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)

end_api_time_2 <- Sys.time()

print("call content")
content_rec <- content(response_rec)

print("call as matrix and do.call")
rec <- do.call(rbind, as.matrix(content_rec))
print("Call matrix and as.numeric")
rec <- matrix(as.numeric(rec), nrow = nrow(rec), ncol = ncol(rec))
}# ---- END OF AUTO-GENERATED CODE ----- #
)
# choose(5000, 2)
# image.plot(rec, col=rev(tim.colors()))

end_run_time <- Sys.time()

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
api_time_1 <- as.numeric(end_api_time_1 - start_api_time_1, units = "secs")
api_time_2 <- as.numeric(end_api_time_2 - start_api_time_2, units = "secs")

print_run_time <- paste("Run time: ", run_time)
print_api_time_1 <- paste("Api time 1: ", api_time_1)
print_api_time_2 <- paste("Api time 2: ", api_time_2)

print(print_run_time)
print(print_api_time_1)
print(print_api_time_2)

print("Execution COMPLETED")
