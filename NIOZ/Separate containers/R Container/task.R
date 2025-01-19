setwd('/app')

start_run_time <- Sys.time()

library(optparse)
library(jsonlite)

library(httr)
library(jsonlite)
library(plot3D)
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

options(width = 120)
require(plot3D)
palette("Dark2")


load(file = "./input_data/Spatio_temporal_settings.rda")
load(file = "./input_data/WKd_Ems.rda")
load(file = "./input_data/Irrad_Ems.rda")
load(file = "./input_data/WAlpha_Ems.rda")
load(file = "./input_data/WEopt_Ems.rda")
load(file = "./input_data/WPs_Ems.rda")
load(file = "./input_data/WHeight_Ems.rda")
load(file = "./input_data/Sediment_Ems.rda")
load(file = "./input_data/BAlpha_Ems.rda")
load(file = "./input_data/BEopt_Ems.rda")
load(file = "./input_data/BPs_Ems.rda")


#require(Rcpp)
#sourceCpp("intPP2D.cpp")  # compiles the C++ code and loads the functions


system.time(
{# Convert each param to JSON format
start_api_time_1 <- Sys.time()

json_zmax <- toJSON(as.vector(Bat_xyv$depth))
json_kd <- toJSON(as.matrix(as.matrix(WKd_Ems[,-1])))
json_par <- toJSON(as.matrix(as.matrix(Irrad_Ems[,-1])))
json_alfa <- toJSON(as.matrix(as.matrix(WAlpha_Ems[,-1])))
json_eopt <- toJSON(as.matrix(as.matrix(WEopt_Ems[,-1])))
json_pmax <- toJSON(as.matrix(as.matrix(WPs_Ems[,-1])))
json_height <- toJSON(as.matrix(as.matrix(WHeight_Ems[,-1])))

list_ppPel <- list(
	zmax = json_zmax,
	kd = json_kd,
	par = json_par,
	alfa = json_alfa,
	eopt = json_eopt,
	pmax = json_pmax,
	height = json_height
)
json_ppPel <- toJSON(list_ppPel, auto_unbox=TRUE)
url_ppPel <- "http://localhost:8080/intPP_mixed"

response_ppPel <- POST(
	url_ppPel,
	body = json_ppPel,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_api_time_1 <- Sys.time()

content_ppPel <- content(response_ppPel)

ppPel <- do.call(rbind, as.matrix(content_ppPel))
ppPel <- matrix(as.numeric(ppPel), nrow = nrow(ppPel), ncol = ncol(ppPel))
}# ---- END OF AUTO-GENERATED CODE ----- #
)

system.time(
{# Convert each param to JSON format
start_api_time_2 <- Sys.time()

json_zmax <- toJSON(as.vector(Bat_xyv$depth))
json_kd <- toJSON(as.matrix(as.matrix(WKd_Ems[,-1])))
json_par <- toJSON(as.matrix(as.matrix(Irrad_Ems[,-1])))
json_height <- toJSON(as.matrix(as.matrix(WHeight_Ems[,-1])))

list_Rad <- list(
	zmax = json_zmax,
	kd = json_kd,
	par = json_par,
	height = json_height
)
json_Rad <- toJSON(list_Rad, auto_unbox=TRUE)
url_Rad <- "http://localhost:8080/rad_bot"

response_Rad <- POST(
	url_Rad,
	body = json_Rad,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_api_time_2 <- Sys.time()

content_Rad <- content(response_Rad)

Rad <- do.call(rbind, as.matrix(content_Rad))
Rad <- matrix(as.numeric(Rad), nrow = nrow(Rad), ncol = ncol(Rad))
}# ---- END OF AUTO-GENERATED CODE ----- #
)

zn <- 0.002  # depth of chlorophyll layer

system.time(
{# Convert each param to JSON format
start_api_time_3 <- Sys.time()

json_zmax <- toJSON(as.vector(as.vector(rep(zn,times=nrow(Bat_xyv)))))
json_kd <- toJSON(as.vector(as.vector(Sediment_Ems$Kd)))
json_pMud <- toJSON(as.vector(as.vector(Sediment_Ems$silt/100)))
json_par <- toJSON(as.matrix(as.matrix(Rad)))
json_alfa <- toJSON(as.matrix(as.matrix(BAlpha_Ems[,-1])))
json_eopt <- toJSON(as.matrix(as.matrix(BEopt_Ems[,-1])))
json_pmax <- toJSON(as.matrix(as.matrix(BPs_Ems[,-1])))

list_ppBen <- list(
	zmax = json_zmax,
	kd = json_kd,
	pMud = json_pMud,
	par = json_par,
	alfa = json_alfa,
	eopt = json_eopt,
	pmax = json_pmax
)
json_ppBen <- toJSON(list_ppBen, auto_unbox=TRUE)
url_ppBen <- "http://localhost:8080/intPP_exp"

response_ppBen <- POST(
	url_ppBen,
	body = json_ppBen,
	encode = "raw",
	add_headers("Content-Type" = "application/json")
)
end_api_time_3 <- Sys.time()

content_ppBen <- content(response_ppBen)

ppBen <- do.call(rbind, as.matrix(content_ppBen))
ppBen <- matrix(as.numeric(ppBen), nrow = nrow(ppBen), ncol = ncol(ppBen))
}# ---- END OF AUTO-GENERATED CODE ----- #
)



Pelagic_t <- apply(ppPel, MARGIN = 1, FUN = mean)
Benthic_t <- apply(ppBen, MARGIN = 1, FUN = mean)

Pelagic_xy <- data.frame(Bat_xyv, # longitude, latitude, depth
                         ppPel = apply(ppPel, MARGIN = 2, FUN = mean))
Benthic_xy <- data.frame(Bat_xyv, 
                         ppBen = apply(ppBen, MARGIN = 2, FUN = mean))
Rad_bottom <- data.frame(Bat_xyv, rad = colMeans(Rad))


#save(file = "../output_data/Pelagic_t.rda", Pelagic_t)
#save(file = "../output_data/Benthic_t.rda", Benthic_t)
#save(file = "../output_data/Pelagic_xy.rda", Pelagic_xy)
#save(file = "../output_data/Benthic_xy.rda", Benthic_xy)


par(mfrow=c(2,3))
with(Pelagic_xy, 
   points2D(longitude, latitude, colvar=depth, 
         main = "water depth", clab= "m",
         asp=1.8, pch=".", cex=4))

with(Pelagic_xy, 
   points2D(longitude, latitude, colvar=ppPel, 
         main = "Pelagic Photosynthesis", clab="mgC/m2/h",
         asp=1.8, pch=".", cex=4))

with(Benthic_xy, 
   points2D(longitude, latitude, colvar=ppBen, 
         main = "Benthic Photosynthesis", clab="mgC/m2/h", 
         asp=1.8, pch=".", cex=4))

with(Rad_bottom, 
   points2D(longitude, latitude, colvar=rad, 
         main = "Radiation at bottom", clab="uE/m2/s",
         asp=1.8, pch=".", cex=4))

with(Sediment_Ems, 
   points2D(longitude, latitude, colvar=Kd, 
         main = "Sediment extinction", clab="/m",
         asp=1.8, pch=".", cex=4))

end_run_time <- Sys.time()

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
api_time_1 <- as.numeric(end_api_time_1 - start_api_time_1, units = "secs")
api_time_2 <- as.numeric(end_api_time_2 - start_api_time_2, units = "secs")
api_time_3 <- as.numeric(end_api_time_3 - start_api_time_3, units = "secs")

print(paste("Run time: ", run_time))
print(paste("API time 1: ", api_time_1))
print(paste("API time 2: ", api_time_2))
print(paste("API time 3: ", api_time_3))

print("Execution COMPLETED")