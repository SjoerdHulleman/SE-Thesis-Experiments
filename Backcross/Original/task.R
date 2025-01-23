start_run_time <- Sys.time()
require(Rcpp)
require(inline)
require(fields)
require(Rcpp)
sourceCpp("source.cpp")


dat<-as.matrix(read.table("mouse.txt"))

start_func1_time <- Sys.time()
rec<-est_bc_source(dat)
end_func1_time <- Sys.time()

# image.plot(rec, col=rev(tim.colors()))

##More markers
source("simulate_diploid_populations.R")
dat.bc<-sim.pop.bc(n.ind = 150, n.mrk = 1000, ch.len = 100, missing = 0, n.ch = 1, verbose = FALSE)
# dat.bc
dat.bc.t<-dat.bc$geno
system.time({
    start_func2_time <- Sys.time()
    rec<-est_bc_source(dat.bc.t)
    end_func2_time <- Sys.time()
})
choose(5000, 2)
# image.plot(rec, col=rev(tim.colors()))

end_run_time <- Sys.time()

run_time <- as.numeric(end_run_time - start_run_time, units = "secs")
func1_time <- as.numeric(end_func1_time - start_func1_time, units = "secs")
func2_time <- as.numeric(end_func2_time - start_func2_time, units = "secs")

print(paste("Run time: ", run_time))
print(paste("Func1 time: ", func1_time))
print(paste("Func2 time: ", func2_time))

print("Execution COMPLETED")