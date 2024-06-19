# A simple example showing how to use portable models
# with Rcpp and TMB

#simulate data
l_inf<- 5
a_min<- 0.1
k<- .5
ages<-c(a_min, 1,2,3,4,5,6,7,8)
#data<-c(replicate(length(ages), 0.0), 0.0)
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
vonB$logk$value<-log(k)
vonB$logk$estimable<-TRUE

#initialize a_min
vonB$a_min$value<-a_min
vonB$a_min$estimable<-FALSE

#initialize l_inf
vonB$l_inf$value<-max(length.data)
vonB$l_inf$estimable<-TRUE

#set data
Pop <- new(Population) 
#set ages 
Pop$ages<-ages
Pop$set_growth(vonB$get_id())

DataLL <- new(NormalLPDF)

DataLL$observed_value <- new(VariableVector, length.data, length(length.data))

DataLL$log_sd <- new(VariableVector, 1)
DataLL$log_sd[1]$value <- 0
DataLL$log_sd[1]$estimable <- TRUE
DataLL$input_type <- "data"
DataLL$simulate_flag <- TRUE 
paste0(Pop$get_module_name(), "_", Pop$get_id(), "_length")
DataLL$set_distribution_links("data", Pop$get_id(), Pop$get_module_name(), "length")


#prepare for interfacing with TMB
CreateModel()


#create an empty data list (data set above)
Data <- list()

#create a parameter list
Parameters <- list(
  p = get_parameter_vector()
)

obj <- TMB::MakeADFun(Data, Parameters, DLL="ModularTMBExample", trace = TRUE)
#newtonOption(obj, smartsearch=FALSE)

print(obj$gr(obj$par))

## Fit model
opt <- nlminb(obj$par, obj$fn, obj$gr)
sdr <- TMB::sdreport(obj)

mean.sdr <- as.list(sdr, "Est")$p
std.sdr <- as.list(sdr, "Std")$p
ci <- list()
for(i in seq_along(mean.sdr)){
  ci[[i]] <- mean.sdr[i] + c(-1,1)*qnorm(.975)*std.sdr[i]
}

test_that("test single nll",{
  expect_equal( log(k) > ci[[1]][1] & log(k) < ci[[1]][2], TRUE)
  expect_equal( l_inf > ci[[2]][1] & l_inf < ci[[2]][2], TRUE)
  expect_equal( log(.1) > ci[[3]][1] & log(.1) < ci[[3]][2], TRUE)
})

#access output from Rcpp object
vonB$finalize(opt$par)
DataLL$finalize(opt$par)
#print optimzed values from RCPP
test_that("test rcpp output", {
  expect_equal(unname(opt$par[1]), vonB$logk$value)
  expect_equal(unname(opt$par[2]), vonB$l_inf$value)
})

# currently fails due to bug in finalize functions
# expect_equal(opt$objective, sum(DataLL$log_likelihood_vec))

clear()
