# A simple example showing how to use portable models
# with Rcpp and TMB

library(TMB)
library(Rcpp)
library(ModularTMBExample)

#Get parameters from FishLife
#install FishLife using: remotes::install_github("James-Thorson-NOAA/FishLife") 
library(FishLife)
library(mvtnorm)
params <- matrix(c('Loo', 'K'), ncol=2)
x <- Search_species(Genus="Hippoglossoides")$match_taxonomy
y <- Plot_taxa(x, params=params)

# multivariate normal in log space for two growth parameters
mu <- c(Linf = 3.848605, K = exp(-1.984452)) #y[[1]]$Mean_pred[params]
Sigma <- rbind(c( 0.1545170, -0.1147763),
               c( -0.1147763,  0.1579867)) #y[[1]]$Cov_pred[params, params]
row.names(Sigma) <- c('Linf', 'K')
colnames(Sigma) <- c('Linf', 'K')


#simulate data
set.seed(123)
sim.parms <- mvtnorm::rmvnorm(2, y[[1]]$Mean_pred[params], 
                                  y[[1]]$Cov_pred[params, params])
l_inf<- sim.parms[,1]
a_min<- 0.1
k<- exp(sim.parms[,2])
ages<-c(0.1, 1,2,3,4,5,6,7,8)
#data<-c(replicate(length(ages), 0.0), 0.0)
Length1 <- Length2 <- replicate(length(ages), 0.0)

for(i in 1:length(ages)){
  Length1[i] = (l_inf[1] * (1.0 - exp(-k[1] * (ages[i] - a_min))))
  Length2[i] = (l_inf[2] * (1.0 - exp(-k[2] * (ages[i] - a_min))))
}
set.seed(234)
length.data1 <- Length1 + rnorm(length(ages), 0, .1)
set.seed(345)
length.data2 <- Length2 + rnorm(length(ages), 0, .1)

#clear the parameter list, if there already is one
clear()

#create first von Bertalanffy object
vonB1<-new(vonBertalanffy)

#initialize k
vonB1$k$value<-.05
vonB1$k$estimable<-TRUE

#initialize a_min
vonB1$a_min$value<-.01
vonB1$a_min$estimable<-FALSE

#initialize l_inf
vonB1$l_inf$value<-7
vonB1$l_inf$estimable<-TRUE

#create second von Bertalanffy object
vonB2<-new(vonBertalanffy)

#initialize k
vonB2$k$value<-.05
vonB2$k$estimable<-TRUE

#initialize a_min
vonB2$a_min$value<-.01
vonB2$a_min$estimable<-FALSE

#initialize l_inf
vonB2$l_inf$value<-7
vonB2$l_inf$estimable<-TRUE

#set data
Pop1 <- new(Population) 
#set ages 
Pop1$ages<-ages
Pop2 <- new(Population) 
#set ages 
Pop2$ages<-ages

DataNLL1 <- new(NormalNLL)
DataNLL1$observed_value <- 
  new(VariableVector, length.data1, length(length.data1))

DataNLL1$expected_value <- new(VariableVector, length(length.data1))
for(i in 1:length(length.data1)){
  DataNLL1$expected_value[i]$value <- 0
}

DataNLL1$log_sd <- new(VariableVector, 1)
DataNLL1$log_sd[1]$value <- 0
DataNLL1$nll_type = "data"
DataNLL1$estimate_log_sd <- TRUE
DataNLL1$set_nll_links("data", Pop1$get_id(), Pop1$get_module_name(), "length")

DataNLL2 <- new(NormalNLL)
DataNLL2$observed_value <- 
  new(VariableVector, length.data2, length(length.data2))

DataNLL2$expected_value <- new(VariableVector, length(length.data2))
for(i in 1:length(length.data2)){
  DataNLL2$expected_value[i]$value <- 0
}

DataNLL2$log_sd <- new(VariableVector, 1)
DataNLL2$log_sd[1]$value <- 0
DataNLL2$nll_type = "data"
DataNLL2$estimate_log_sd <- TRUE
DataNLL2$set_nll_links("data", Pop2$get_id(), Pop2$get_module_name(), "length")


# GrowthKPrior <- new(NormalNLL)
# GrowthKPrior$expected_value <- new(VariableVector, mu[2], 1)
# GrowthKPrior$nll_type = "prior"
# GrowthKPrior$set_nll_links( "prior", vonB$get_id(), vonB$get_module_name(), "k")

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
rep <- sdreport(obj)

rep

#update the von Bertalanffy object with updated parameters
vonB$finalize(rep$par.fixed)

#show results
vonB$show()

obj$report()

#show final gradient
print("final gradient:")
print(rep$gradient.fixed)
