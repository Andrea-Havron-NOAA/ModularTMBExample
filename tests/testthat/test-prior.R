# A simple example showing how to use portable models
# with Rcpp and TMB


# #Get parameters from FishLife
# #install FishLife using: remotes::install_github("James-Thorson-NOAA/FishLife") 
# library(FishLife)
# params <- matrix(c('Loo', 'K'), ncol=2)
# x <- Search_species(Genus="Hippoglossoides")$match_taxonomy
# y <- Plot_taxa(x, params=params)
library(mvtnorm)

# multivariate normal in log space for two growth parameters
mu <- c(Linf = 3.848605, K = -1.984452) #y[[1]]$Mean_pred[params]
Sigma <- rbind(c( 0.1545170, -0.1147763),
               c( -0.1147763,  0.1579867)) #y[[1]]$Cov_pred[params, params]
row.names(Sigma) <- c('Linf', 'K')
colnames(Sigma) <- c('Linf', 'K')


#simulate data
set.seed(123)
sim.parms <- mvtnorm::rmvnorm(1, mu, Sigma)
l_inf<- sim.parms[1]
a_min<- 0.1
k<- exp(sim.parms[2])
ages<-c(0.1, 1,2,3,4,5,6,7,8)
Length<-replicate(length(ages), 0.0)

for(i in 1:length(ages)){
  Length[i] = (l_inf * (1.0 - exp(-k * (ages[i] - a_min))))
}
set.seed(234)
length.data <- Length + rnorm(length(ages), 0, .1)

#clear the parameter list, if there already is one
clear()

#create a von Bertalanffy object
vonB<-new(vonBertalanffy)

#initialize k
vonB$logk$value<-log(.05)
vonB$logk$estimable<-TRUE

#initialize a_min
vonB$a_min$value<-.1
vonB$a_min$estimable<-FALSE

#initialize l_inf
vonB$l_inf$value<-7
vonB$l_inf$estimable<-TRUE

#set data
Pop <- new(Population) 
#set ages 
Pop$ages<-ages
Pop$set_growth(vonB$get_id())

DataNLL <- new(NormalNLL)

DataNLL$observed_value <- new(VariableVector, length.data, length(length.data))
DataNLL$log_sd <- new(VariableVector, 1)
DataNLL$log_sd[1]$value <- 0
DataNLL$nll_type = "data"
DataNLL$estimate_log_sd <- TRUE
DataNLL$set_nll_links("data", Pop$get_id(), Pop$get_module_name(), "length")

GrowthKPrior <- new(NormalNLL)
GrowthKPrior$expected_value <- new(VariableVector, mu[2], 1)
GrowthKPrior$nll_type = "prior"
GrowthKPrior$set_nll_links( "prior", vonB$get_id(), vonB$get_module_name(), "logk")

#prepare for interfacing with TMB
CreateModel()


#create an empty data list (data set above)
Data <- list()

#create a parameter list
Parameters <- list(
  p = get_parameter_vector()
)

obj <- MakeADFun(Data, Parameters, DLL="ModularTMBExample")
newtonOption(obj, smartsearch=FALSE)

print(obj$gr(obj$par))

## Fit model
opt <- nlminb(obj$par, obj$fn, obj$gr)
sdr <- sdreport(obj)

mean.sdr <- as.list(sdr, "Est")$p
std.sdr <- as.list(sdr, "Std")$p
ci <- list()
for(i in seq_along(mean.sdr)){
  ci[[i]] <- mean.sdr[i] + c(-1,1)*qnorm(.975)*std.sdr[i]
}

test_that("test single prior",{
  expect_equal( log(k) > ci[[1]][1] & log(k) < ci[[1]][2], TRUE)
  expect_equal( l_inf > ci[[2]][1] & l_inf < ci[[2]][2], TRUE)
  expect_equal( log(.1) > ci[[3]][1] & log(.1) < ci[[3]][2], TRUE)
})

test_that( "test_tmbstan", {
skip_on_ci(library(tmbstan))
skip_on_ci(fit <- tmbstan(obj))
skip_on_ci(library(shinystan))
skip_on_ci(library(ggplot2))
skip_on_ci(launch_shinystan(fit))
})

clear()
# #update the von Bertalanffy object with updated parameters
# vonB$finalize(rep$par.fixed)

# #show results
# vonB$show()

# obj$report()

# #show final gradient
# print("final gradient:")
# print(rep$gradient.fixed)
