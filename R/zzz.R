
Rcpp::loadModule(module = "growth", what = TRUE)



setMethod("[<-", signature(x = "Rcpp_VariableVector", i = "numeric"),
          function(x, i) {
            (x$at(i))
             return(x)
          })

setMethod("[", signature(x = "Rcpp_VariableVector", i = "numeric"),
          function(x, i) {
            return(x$at(i))
          })

setMethod("length", signature(x = "Rcpp_VariableVector"),
          function(x) {
            return(x$size())
          })

