#ifndef RCPP_INTERFACE_BASE_HPP
#define  RCPP_INTERFACE_BASE_HPP

#include "../../../common/def.hpp"



#define RCPP_NO_SUGAR
#include <Rcpp.h>



/**
 * Rcpp representation of a variable
 * interface between R and cpp.
 */
class Variable{
public:
    static std::vector<Variable*> parameters;
    static std::vector<Variable*> estimated_parameters;
    bool estimable = FALSE;
    double value = 0;
    
    Variable(){
        Variable::parameters.push_back(this);
    }
    
};


/**
 *@brief Base class for all interface objects
 */
class RcppInterfaceBase {
 public:
  static uint32_t module_id_g; /**< static id of the GrowthInterfaceBase object */
uint32_t module_id;          /**< local id of the GrowthInterfaceBase object */
  /**< FIMS interface object vectors */
  static std::vector<RcppInterfaceBase *> interface_objects;

  RcppInterfaceBase(){
    this->module_id = RcppInterfaceBase::module_id_g++;
  }

  /** @brief virtual method to inherit to add objects to the TMB model */
  virtual bool prepare() {
    return false;
  }


};

uint32_t RcppInterfaceBase::module_id_g = 1;
std::vector<RcppInterfaceBase *>
    RcppInterfaceBase::interface_objects;
std::vector<Variable*> Variable::parameters;
std::vector<Variable*> Variable::estimated_parameters;

#endif