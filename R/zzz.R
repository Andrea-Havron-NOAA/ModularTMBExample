
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

setMethod("lapply", signature(X = "Rcpp_VariableVector", FUN = "sum"),
          function(X,FUN) {
            lappy(X$data,FUN)
          })

setMethod("length", signature(x = "Rcpp_VariableVector"),
          function(x) {
            return(x$size())
          })

setMethod("sum", signature(x = "Rcpp_VariableVector"),
          function(x) {
            ret<-new(Variable)
            tmp<-0.0
            for(i in 1:x$size()){
                tmp = tmp + x[i]$value
            }
            ret$value<-tmp
            return(ret)
          })

setMethod("dim", signature(x = "Rcpp_VariableVector"),
          function(x) {
            return(x$size())
          })

