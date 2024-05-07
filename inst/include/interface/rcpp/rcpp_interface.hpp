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
 * Add log info entry from R.
 */
void LogInfo(std::string log_entry){
    FIMSLog::fims_log->message(log_entry, -999, "R_env", "R_script_entry");
}

/**
 * Add log warning entry from R.
 */
void LogWarning(std::string log_entry){
    FIMSLog::fims_log->warning_message(log_entry, -999, "R_env", "R_script_entry");
}

/**
 * Add log error entry from R.
 */
void LogError(std::string log_entry){
  
    std::stringstream ss;
                ss << "capture.output(traceback(4))";
    SEXP expression, result;
                ParseStatus status;

                PROTECT(expression = R_ParseVector(Rf_mkString(ss.str().c_str()), 1, &status, R_NilValue));
                if (status != PARSE_OK) {
                    std::cout << "Error parsing expression" << std::endl;
                    UNPROTECT(1);
                }
    Rcpp::Rcout<<"before call.";
                PROTECT(result = Rf_eval(VECTOR_ELT(expression, 0), R_GlobalEnv));
    Rcpp::Rcout<<"after call.";
    UNPROTECT(2);
    std::stringstream ss_ret;
    ss_ret<<"\n";
    for (int j = 0; j < LENGTH(result); j++) {
        std::string str(CHAR(STRING_ELT(result, j)));
        ss_ret<<str<<"\n";
    }
    
    std::string ret = ss_ret.str();//"find error";//Rcpp::as<std::string>(result);

    
//    Rcpp::Environment base = Rcpp::Environment::global_env();
//    Rcpp::Function f  = base["traceback"];
//    std::string ret = Rcpp::as<std::string>(f());
    FIMSLog::fims_log->error_message(log_entry, -999,  "R_env", ret.c_str());
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
    Rcpp::function("SetLogThrowOnError", SetLogThrowOnError);
    Rcpp::function("LogInfo", LogInfo);
    Rcpp::function("LogWarning", LogWarning);
    Rcpp::function("LogError", LogError);
    
};

#endif
