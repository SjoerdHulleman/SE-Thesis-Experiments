## ----setup, include=FALSE-----------------------------------------------------
run_time_start <- Sys.time()
options(width = 120)
require(plot3D)
palette("Dark2")


## ----fig.width=8, fig.height=6------------------------------------------------
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


## -----------------------------------------------------------------------------
require(Rcpp)
sourceCpp("intPP2D.cpp")  # compiles the C++ code and loads the functions


## -----------------------------------------------------------------------------
# Pelagic primary production, assuming mixed water column 
system.time({
func_1_start <- Sys.time()
  ppPel <- intPP_mixed(Bat_xyv$depth, 
                 as.matrix(WKd_Ems    [, -1]), 
                 as.matrix(Irrad_Ems  [, -1]), 
                 as.matrix(WAlpha_Ems [, -1]), 
                 as.matrix(WEopt_Ems  [, -1]), 
                 as.matrix(WPs_Ems    [, -1]), 
                 as.matrix(WHeight_Ems[, -1]))
func_1_end <- Sys.time()
})

# radiation at the bottom   
system.time({
func_2_start <- Sys.time()
  Rad <- rad_bot(Bat_xyv$depth, 
                 as.matrix(WKd_Ems     [, -1]), 
                 as.matrix(Irrad_Ems  [, -1]), 
                 as.matrix(WHeight_Ems[, -1]))
func_2_end <- Sys.time()
})

zn <- 0.002  # depth of chlorophyll layer

# Benthic primary production, 
# exponentially declining chlorophyll concentration, a function of silt fraction
system.time({
func_3_start <- Sys.time()
  ppBen <- intPP_exp(as.vector(rep(zn, times = nrow(Bat_xyv))), 
                    as.vector(Sediment_Ems$Kd), 
                    as.vector(Sediment_Ems$silt/100),
                    as.matrix(Rad), 
                    as.matrix(BAlpha_Ems      [, -1]), 
                    as.matrix(BEopt_Ems       [, -1]), 
                    as.matrix(BPs_Ems         [, -1]))
func_3_end <- Sys.time()
})



## -----------------------------------------------------------------------------
Pelagic_t <- apply(ppPel, MARGIN = 1, FUN = mean)
Benthic_t <- apply(ppBen, MARGIN = 1, FUN = mean)

Pelagic_xy <- data.frame(Bat_xyv, # longitude, latitude, depth
                         ppPel = apply(ppPel, MARGIN = 2, FUN = mean))
Benthic_xy <- data.frame(Bat_xyv, 
                         ppBen = apply(ppBen, MARGIN = 2, FUN = mean))
Rad_bottom <- data.frame(Bat_xyv, rad = colMeans(Rad))


## -----------------------------------------------------------------------------
#save(file = "../output_data/Pelagic_t.rda", Pelagic_t)
#save(file = "../output_data/Benthic_t.rda", Benthic_t)
#save(file = "../output_data/Pelagic_xy.rda", Pelagic_xy)
#save(file = "../output_data/Benthic_xy.rda", Benthic_xy)


## ----fig.width=10, fig.height=6-----------------------------------------------
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

run_time_end <- Sys.time()

run_time <- as.numeric(run_time_end - run_time_start, units = "secs")
func_1_time <- as.numeric(func_1_end - func_1_start, units = "secs")
func_2_time <- as.numeric(func_2_end - func_2_start, units = "secs")
func_3_time <- as.numeric(func_3_end - func_3_start, units = "secs")

print("Execution COMPLETED")


print(paste("Run time: ", run_time))
print(paste("Func time 1: ", func_1_time))
print(paste("Func time 2: ", func_2_time))
print(paste("Func time 3: ", func_3_time))



