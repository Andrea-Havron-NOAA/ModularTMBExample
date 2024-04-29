#ifndef RCPP_NORMAL_NLL_HPP
#define  RCPP_NORMAL_NLL_HPP


#include "../../../common/model.hpp"
#include "rcpp_interface_base.hpp"
#include "../../../nll/normal_nll.hpp"
#include "../rcpp_interface.hpp"

/****************************************************************
 * Univariate NLL Rcpp interface                                   *
 ***************************************************************/
/**
 * @brief Rcpp interface that serves as the parent class for
 * Rcpp nll interfaces. This type should be inherited and not
 * called from R directly.
 *
 */
class UnivariateNLLInterface : public RcppInterfaceBase {
public:
  static uint32_t id_g; /**< static id of the NormalNLLInterface object */
    uint32_t id;          /**< local id of the NormalNLLInterface object */
    static std::map<uint32_t, UnivariateNLLInterface*> univariate_nll_objects; /**<
 map relating the ID of the UnivariateNLLInterface to the objects
 objects */
  uint32_t module_id;

UnivariateNLLInterface() {
  this->id = UnivariateNLLInterface::id_g++;
  UnivariateNLLInterface::univariate_nll_objects[this->id] = this;
  RcppInterfaceBase::interface_objects.push_back(this);
}

virtual ~UnivariateNLLInterface() {}

virtual uint32_t get_id() = 0;
};

uint32_t UnivariateNLLInterface::id_g = 1;
std::map<uint32_t, UnivariateNLLInterface*> UnivariateNLLInterface::univariate_nll_objects;


class NormalNLLInterface : public UnivariateNLLInterface{

public:
    VariableVector observed_value;
    VariableVector expected_value;
    VariableVector log_sd;
    std::string nll_type;
    std::vector<std::string> key;

    bool estimate_observed_value = false;
    bool estimate_expected_value = false;
    bool estimate_log_sd = false;
    bool simulate_prior_flag = false;
    bool simulate_data_flag = false;
    
    NormalNLLInterface() : UnivariateNLLInterface(){}
    
    virtual ~NormalNLLInterface() {}
    virtual uint32_t get_id() { return this->id; }
    
    virtual std::string get_module_name(){
        return "NormalNLLInterface";
    }
    
    void SetNLLLinks(std::string nll_type, Rcpp::IntegerVector module_id, 
        Rcpp::StringVector module_name, Rcpp::StringVector name){
        this->nll_type = nll_type;

        std::stringstream ss;
        this->key.resize(module_id.size());
        for(int i=0; i<module_id.size(); i++){
            ss << module_name[i] << "_" << module_id[i] << "_" << name[i];
            this->key[i] = ss.str();
            ss.str("");
        }
    }

    template<typename Type>
    bool prepare_local() {
    std::shared_ptr<Information<Type> > info =
        Information<Type>::getInstance();
     std::shared_ptr<NormalNLL<Type> > normal =
        std::make_shared<NormalNLL<Type> >();

        std::shared_ptr<Model<Type> > model = Model<Type>::getInstance();
        normal->nll_type = this->nll_type;
        normal->id = this->id;
        normal->key.resize(this->key.size());
        for(int i=0; i<key.size(); i++){
            normal->key[i] = this-> key[i];
        }
        normal->simulate_prior_flag = this->simulate_prior_flag;
        normal->simulate_data_flag = this->simulate_data_flag;
        if(this->nll_type == "data"){
            normal->osa_flag = true;
        } else {
            normal->osa_flag = false;
        }
         //initialize x and mu : how do I differentiate this from the SetX and SetMu functions above? flags?
        normal->observed_value.resize(this->observed_value.size());
        for(size_t i=0; i<this->observed_value.size(); i++){
            normal->observed_value[i] = this->observed_value[i].value;
            if(this ->estimate_observed_value){
                model->parameters.push_back(&(normal)->observed_value[i]);
            }
        }
        normal->expected_value.resize(this->expected_value.size());
        for(size_t i=0; i<this->expected_value.size(); i++){
            normal->expected_value[i] = this->expected_value[i].value;
            if(this ->estimate_expected_value){
                model->parameters.push_back(&(normal)->expected_value[i]);
            }
        }
        normal->log_sd.resize(this->log_sd.size());
        for(size_t i=0; i<this->log_sd.size(); i++){
            normal->log_sd[i] = this->log_sd[i].value;
            if(this ->estimate_log_sd){
                model->parameters.push_back(&(normal)->log_sd[i]);
                model->pnames.push_back("log_sd");
            }
        }
        
        model->nll_models[normal->id] = normal;
        info->nll_models[normal->id] = normal;
        return true;
    }

    
 /**
     * Prepares the model to work with TMB.
     */
    virtual bool prepare() {
       
#ifdef TMB_MODEL
        this->prepare_local<TMB_FIMS_REAL_TYPE>();
        this->prepare_local<TMB_FIMS_FIRST_ORDER>();
        this->prepare_local<TMB_FIMS_SECOND_ORDER>();
        this->prepare_local<TMB_FIMS_THIRD_ORDER>();
        
#endif
        return true;

    }
};

#endif