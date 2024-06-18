#ifndef RCPP_INTERFACE_HPP
#define  RCPP_INTERFACE_HPP



#include "rcpp_objects/rcpp_growth.hpp"
#include "rcpp_objects/rcpp_population.hpp"
#include "rcpp_objects/rcpp_distributions.hpp"
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
 * Clears parameters
 */
template <typename Type>
void clear_internal() {
  std::shared_ptr<Model<Type> > model = 
    Model<Type>::getInstance();
  model->parameters.clear();
}

/**
 * Clears memory from model.
 */
void clear() {
  RcppInterfaceBase::interface_objects.clear();
  Variable::parameters.clear();
  
  // rcpp_growth
  GrowthInterfaceBase::id_g = 1;
  GrowthInterfaceBase::growth_objects.clear();
  
  //rcpp_distributions
  DensityComponentsInterface::id_g = 1;
  DensityComponentsInterface::density_components_objects.clear();
  
  //rcpp_population
  PopulationInterfaceBase::id_g = 1;
  PopulationInterfaceBase::interface_objects.clear();
  
  
  clear_internal<TMB_FIMS_REAL_TYPE>();
  clear_internal<TMB_FIMS_FIRST_ORDER>();
  clear_internal<TMB_FIMS_SECOND_ORDER>();
  clear_internal<TMB_FIMS_THIRD_ORDER>();
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
      .constructor<Rcpp::NumericVector, size_t>()
      .field("data", &VariableVector::storage_m)
      .method("at", &VariableVector::at)
      .method("size", &VariableVector::size)
      .method("resize", &VariableVector::resize)
      .method("set_all_estimable", &VariableVector::set_all_estimable)
      .method("fill", &VariableVector::fill);
    Rcpp::class_<PopulationInterface>("Population")
    .constructor()
    .field("ages", &PopulationInterface::ages)
    .method("get_id", &PopulationInterface::get_id)
    .method("set_growth", &PopulationInterface::SetGrowth)
    .method("get_module_name", &PopulationInterface::get_module_name);
    Rcpp::class_<NormalLPDFInterface>("NormalLPDF")
    .constructor()
    .method("finalize", &NormalLPDFInterface::finalize)
    .field("observed_value", &NormalLPDFInterface::observed_value)
    .field("expected_value", &NormalLPDFInterface::expected_value)
    .field("log_sd", &NormalLPDFInterface::log_sd)
    .field("input_type", &NormalLPDFInterface::input_type)
    .field("log_likelihood_vec", &NormalLPDFInterface::log_likelihood_vec)
    .field("simulate_flag", &NormalLPDFInterface::simulate_flag)
    .method("get_id", &NormalLPDFInterface::get_id)
    .method("set_distribution_links", &NormalLPDFInterface::SetDistributionLinks);
    Rcpp::class_<MVNormLPDFInterface>("MVNormLPDF")
    .constructor()
    .method("finalize", &MVNormLPDFInterface::finalize)
    .field("observed_value", &MVNormLPDFInterface::observed_value)
    .field("expected_value", &MVNormLPDFInterface::expected_value)
    .field("log_sd", &MVNormLPDFInterface::log_sd)
    .field("logit_phi", &MVNormLPDFInterface::logit_phi)
    .field("input_type", &MVNormLPDFInterface::input_type)
    .field("log_likelihood_vec", &MVNormLPDFInterface::log_likelihood_vec)
    .field("simulate_flag", &MVNormLPDFInterface::simulate_flag)
    .method("get_id", &MVNormLPDFInterface::get_id)
    .method("set_distribution_links", &MVNormLPDFInterface::SetDistributionLinks);
    Rcpp::class_<vonBertalanffyInterface>("vonBertalanffy")
    .constructor()
    .method("finalize", &vonBertalanffyInterface::finalize)
    .field("logk", &vonBertalanffyInterface::logk)
    .field("l_inf", &vonBertalanffyInterface::l_inf)
    .field("a_min", &vonBertalanffyInterface::a_min)
    .field("alpha", &vonBertalanffyInterface::alpha)
    .field("beta", &vonBertalanffyInterface::beta)
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
