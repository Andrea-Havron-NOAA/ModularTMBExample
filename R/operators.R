

setMethod("Ops", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"),
           function(e1, e2) callGeneric(e1, e2))
 
 setMethod("Ops", signature(e1 = "Rcpp_Variable", e2 = "numeric"),
           function(e1, e2) callGeneric(e1, e2))
 
 setMethod("Ops", signature(e1 = "numeric", e2 = "Rcpp_Variable"),
           function(e1, e2) callGeneric(e1, e2))

#Variable
setMethod("acos", signature(x = "Rcpp_Variable"), function (x) {new(Variable,acos(x$value))})
  setMethod("asin", signature(x = "Rcpp_Variable"), function (x) {new(Variable,asin(x$value))})
  setMethod("atan", signature(x = "Rcpp_Variable"), function (x) {new(Variable,atan(x$value))})
  setMethod("cos", signature(x = "Rcpp_Variable"), function (x) {new(Variable,cos(x$value))})
  setMethod("cosh", signature(x = "Rcpp_Variable"), function (x) {new(Variable,cosh(x$value))})
  setMethod("sin", signature(x = "Rcpp_Variable"), function (x) {new(Variable,sin(x$value))})
  setMethod("sinh", signature(x = "Rcpp_Variable"), function (x) {new(Variable,sinh(x$value))})
  setMethod("tan", signature(x = "Rcpp_Variable"), function (x) {new(Variable,tan(x$value))})
  setMethod("tanh", signature(x = "Rcpp_Variable"), function (x) {new(Variable,tanh(x$value))})
  setMethod("exp", signature(x = "Rcpp_Variable"), function (x) {new(Variable,exp(x$value))})
  setMethod("log10", signature(x = "Rcpp_Variable"), function (x) {new(Variable,log10(x$value))})
  setMethod("sqrt", signature(x = "Rcpp_Variable"), function (x) {new(Variable, (x$value^0.5))})
  setMethod("log", signature(x = "Rcpp_Variable"), function (x, base=exp(1)){return(new(Variable,log(x$value)))})

  setMethod("^", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
    (e1$value^e2$value)})
  setMethod("^", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
    ((e1$value^ e2))})
  setMethod("^", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
    (e1^ e2$value)})
  
  #+
  setMethod("+", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1$value + e2$value))})
  setMethod("+", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
    return(new(Variable,e1$value + e2))})
  setMethod("+", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1 + e2$value))})
  #-
  setMethod("-", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1$value - e2$value))})
  setMethod("-", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
    return (new(Variable,e1 - e2$value))})
  setMethod("-", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1 - e2$value))})
  
  #*
  setMethod("*", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1$value * e2$value))})
  setMethod("*", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
    return(new(Variable,e1$value * e2))})
  setMethod("*", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1 * e2$value))})
  
  #/
  setMethod("/", signature(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1$value / e2$value))})
  setMethod("/", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
    return(new(Variable,e1$value / e2))})
  setMethod("/", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
    return(new(Variable,e1 / e2$value))})


# -------------------------------------------------------------------------

#setMethod("<-", c(e1 = "Rcpp_Variable", e2 = "Rcpp_Variable"), function (e1, e2){
 # (e1$value<- e2$value)})

#setMethod("=", c(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
 # (e1$value<- e2)})

setMethod("fill", signature(e1 = "Rcpp_VariableVector", e2 = "numeric"), function (e1, e2){

    for(i in 1:e1$size()){
        e1[i]$value <- e2
    }
    
     })

setMethod("+", signature(e1 = "Rcpp_VariableVector", e2 = "Rcpp_VariableVector"), function (e1, e2){
    
    if(e1$size() != e$size()){
        stop("Call to operator \"+\", vectors not equal length")
    }
    ret<-new(VariableVector, e1$size())
    for(i in 1:e1$size()){
        ret<-e1[i]+e2[i]
    }
    return(ret)
     })
setMethod("^", signature(e1 = "Rcpp_Variable", e2 = "numeric"), function (e1, e2){
  ((e1$value^ e2))})
setMethod("^", signature(e1 = "numeric", e2 = "Rcpp_Variable"), function (e1, e2){
  (e1^ e2$value)})
