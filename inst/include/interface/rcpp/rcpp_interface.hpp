#ifndef RCPP_INTERFACE_HPP
#define  RCPP_INTERFACE_HPP



#include "rcpp_objects/rcpp_growth.hpp"
#include "rcpp_objects/rcpp_population.hpp"
#include "rcpp_objects/rcpp_nll.hpp"
#include "../../common/def.hpp"

bool CreateModel(){
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
RCPP_EXPOSED_CLASS(VariableVector)
RCPP_EXPOSED_CLASS(vonBertalanffyInterface)
RCPP_EXPOSED_CLASS(ObsDataInterface)

/**
 * Returns the initial values for the parameter set
 */
Rcpp::NumericVector get_parameter_vector() {
    Rcpp::NumericVector p;
    std::shared_ptr<Model<double> > model = Model<double>::getInstance();


    for (int i = 0; i < model->parameters.size(); i++) {
        p.push_back(*model->parameters[i]);
    }

    return p;
}

/**
 * Returns the initial values for the parameter set
 */
Rcpp::StringVector get_parameter_names_vector() {
    Rcpp::StringVector pnames;
    std::shared_ptr<Model<double> > model = Model<double>::getInstance();

    for (int i = 0; i < model->pnames.size(); i++) {
        pnames.push_back(model->pnames[i]);
    }

    return pnames;
}



/**
 * Clears the vector of independent variables.
 */
void clear(){
    Variable::parameters.clear();
}


/**
 * Define the Rcpp module.
 */
RCPP_MODULE(growth) {
    Rcpp::class_<Variable>("Variable")
    .constructor()
    .constructor<double>()
    .field("value", &Variable::value)
    .field("estimable",&Variable::estimable)
    .field("id",&Variable::id);
    Rcpp::class_<VariableVector>("VariableVector")
      .constructor()
      .constructor<size_t>()
      .field("data", &VariableVector::storage_m)
      .method("at", &VariableVector::at)
      .method("size", &VariableVector::size)
      .method("resize", &VariableVector::resize)
      .method("set_all_estimable", &VariableVector::set_all_estimable);
    Rcpp::class_<PopulationInterface>("Population")
    .constructor()
    .field("ages", &PopulationInterface::ages)
    .method("get_id", &PopulationInterface::get_id)
    .method("get_module_name", &PopulationInterface::get_module_name);
    Rcpp::class_<NormalNLLInterface>("NormalNLL")
    .constructor()
    .field("observed_value", &NormalNLLInterface::observed_value)
    .field("expected_value", &NormalNLLInterface::expected_value)
    .field("log_sd", &NormalNLLInterface::log_sd)
    .field("estimate_observed_value", &NormalNLLInterface::estimate_observed_value)
    .field("estimate_expected_value", &NormalNLLInterface::estimate_expected_value)
    .field("estimate_log_sd", &NormalNLLInterface::estimate_log_sd)
    .field("nll_type", &NormalNLLInterface::nll_type)
    //.field("module_name", &NormalNLLInterface::module_name)
    //.field("module_id", &NormalNLLInterface::module_id)
    //.field("member_name", &NormalNLLInterface::member_name)
    .method("get_id", &NormalNLLInterface::get_id)
    .method("set_nll_links", &NormalNLLInterface::SetNLLLinks);
    //.method("get_module_id", &NormalNLLInterface::get_module_id)
   // .method("SetX", &NormalNLLInterface::SetX)
   // .method("SetMu", &NormalNLLInterface::SetMu);
    Rcpp::class_<vonBertalanffyInterface>("vonBertalanffy")
    .constructor()
   // .method("finalize", &vonBertalanffyInterface::finalize)
    .field("k", &vonBertalanffyInterface::k)
    .field("l_inf", &vonBertalanffyInterface::l_inf)
    .field("a_min", &vonBertalanffyInterface::a_min)
    .field("alpha", &vonBertalanffyInterface::alpha)
    .field("beta", &vonBertalanffyInterface::beta)
    //.field("id", &vonBertalanffyInterface::id)
    .method("get_id", &vonBertalanffyInterface::get_id)
    .method("get_module_name", &vonBertalanffyInterface::get_module_name);
  //  .method("show", &vonBertalanffyInterface::show);
    Rcpp::function("get_parameter_vector", get_parameter_vector);
    Rcpp::function("get_parameter_names_vector", get_parameter_names_vector);
    Rcpp::function("clear", clear);
    Rcpp::function("CreateModel", CreateModel);
    Rcpp::function("CreateVector", CreateVector);
};

#endif
