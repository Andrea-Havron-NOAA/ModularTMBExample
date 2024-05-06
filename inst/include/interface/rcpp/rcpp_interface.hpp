#ifndef RCPP_INTERFACE_HPP
#define  RCPP_INTERFACE_HPP



#include "rcpp_objects/rcpp_growth.hpp"
#include "rcpp_objects/rcpp_data.hpp"

bool CreateModel(){
    
  FIMS_LOG("CreateModel: "+ fims::to_string(RcppInterfaceBase::interface_objects.size())+ " instantiated model objects.")
  for (size_t i = 0; i < RcppInterfaceBase::interface_objects.size();
       i++) {
    RcppInterfaceBase::interface_objects[i]->prepare();
  }
  return true;
}

/**
 * Exposes the Variable and vonBertalanffyInterface classes to R.
 */
RCPP_EXPOSED_CLASS(Variable)
RCPP_EXPOSED_CLASS(vonBertalanffyInterface)
RCPP_EXPOSED_CLASS(ObsDataInterface)

/**
 * Returns the initial values for the parameter set
 */
Rcpp::NumericVector get_parameter_vector(){
    Rcpp::NumericVector p;
    
    for(int i =0; i < Variable::parameters.size(); i++){
        if(Variable::parameters[i]->estimable){
            Variable::estimated_parameters.push_back(Variable::parameters[i]);
            p.push_back(Variable::parameters[i]->value);
        }
    }
    return p;
}
/**
 * Clears the vector of independent variables.
 */
void clear(){
    Variable::parameters.clear();
}

/**
 * Returns the entire log as a string in JSON format.
 */
std::string GetLog() {
  return FIMSLog::fims_log->get_log();
}
/**
 * Returns only error entries from log as a string in JSON format.
 */
std::string GetLogErrors() {
  return FIMSLog::fims_log->get_errors();
}

/**
 * Returns only warning entries from log as a string in JSON format.
 */
std::string GetLogWarnings() {
  return FIMSLog::fims_log->get_warnings();
}

/**
 * Returns only info entries from log as a string in JSON format.
 */
std::string GetLogInfo() {
  return FIMSLog::fims_log->get_info();
}

/**
 * Returns  log entries by module as a string in JSON format.
 */
std::string GetLogModule(const std::string& module) {
  return FIMSLog::fims_log->get_module(module);
}

/**
 * If true, writes the log on exit .
 */
void WriteLog(bool write) {
  FIMS_LOG("Setting FIMS write log: "+ fims::to_string(write))
  FIMSLog::fims_log->write_on_exit = write;
}

/**
 * Sets the path for the log file to written.
 */
void SetLogPath(const std::string& path) {
  FIMS_LOG("Setting FIMS log path: "+ path)
  FIMSLog::fims_log->set_path(path);
}

/**
 * If true, throws a  runtime exception when an error is logged .
 */
void SetLogThrowOnError(bool throw_on_error) {
    FIMSLog::fims_log->throw_on_error = throw_on_error;
}

/**
 * Initializes the logging syste, Sets all signal handling. 
 */
void InitLogging(){
  FIMS_LOG("Initializing FIMS logging system.")
  std::signal(SIGSEGV, &WriteAtExit);
  std::signal(SIGINT, &WriteAtExit);
  std::signal(SIGABRT, &WriteAtExit);
  std::signal(SIGFPE, &WriteAtExit);
  std::signal(SIGILL, &WriteAtExit);
  std::signal(SIGTERM, &WriteAtExit);
}

/**
 * Define the Rcpp module.
 */
RCPP_MODULE(growth) {
    Rcpp::class_<Variable>("Variable")
    .constructor()
    .field("value", &Variable::value)
    .field("estimable",&Variable::estimable);
    Rcpp::class_<ObsDataInterface>("ObsData")
    .constructor()
    .method("finalize", &ObsDataInterface::finalize)
    .field("Data", &ObsDataInterface::data)
    .field("ages", &ObsDataInterface::ages);
    Rcpp::class_<vonBertalanffyInterface>("vonBertalanffy")
    .constructor()
    .method("finalize", &vonBertalanffyInterface::finalize)
    .field("k", &vonBertalanffyInterface::k)
    .field("l_inf", &vonBertalanffyInterface::l_inf)
    .field("a_min", &vonBertalanffyInterface::a_min)
    .field("alpha", &vonBertalanffyInterface::alpha)
    .field("beta", &vonBertalanffyInterface::beta);
    Rcpp::function("get_parameter_vector", get_parameter_vector);
    Rcpp::function("clear", clear);
    Rcpp::function("CreateModel", CreateModel);
    Rcpp::function("GetLog", GetLog);
    Rcpp::function("GetLogErrors", GetLogErrors);
    Rcpp::function("GetLogWarnings", GetLogWarnings);
    Rcpp::function("GetLogInfo", GetLogInfo);
    Rcpp::function("GetLogModule", GetLogModule);
    Rcpp::function("WriteLog", WriteLog);
    Rcpp::function("SetLogPath", SetLogPath);
    Rcpp::function("InitLogging", InitLogging);
};

#endif
