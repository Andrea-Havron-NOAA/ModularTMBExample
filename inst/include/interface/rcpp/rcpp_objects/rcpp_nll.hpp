#ifndef RCPP_NORMAL_NLL_HPP
#define  RCPP_NORMAL_NLL_HPP


#include "../../../common/model.hpp"
#include "rcpp_interface_base.hpp"
#include "../../../nll/normal_nll.hpp"
#include "../../../nll/mvnorm_nll.hpp"
#include "../rcpp_interface.hpp"

/****************************************************************
 * NLL Rcpp interface                                   *
 ***************************************************************/
/**
 * @brief Rcpp interface that serves as the parent class for
 * Rcpp nll interfaces. This type should be inherited and not
 * called from R directly.
 *
 */
class NLLInterface : public RcppInterfaceBase {
public:
  static uint32_t id_g; /**< static id of the NormalNLLInterface object */
    uint32_t id;          /**< local id of the NormalNLLInterface object */
    static std::map<uint32_t, NLLInterface*> nll_objects; /**<
 map relating the ID of the UnivariateNLLInterface to the objects
 objects */
  uint32_t module_id;

NLLInterface() {
  this->id = NLLInterface::id_g++;
  NLLInterface::nll_objects[this->id] = this;
  RcppInterfaceBase::interface_objects.push_back(this);
}

virtual ~NLLInterface() {}

virtual uint32_t get_id() = 0;
};

uint32_t NLLInterface::id_g = 1;
std::map<uint32_t, NLLInterface*> NLLInterface::nll_objects;


class NormalNLLInterface : public NLLInterface{

public:
    VariableVector observed_value;
    VariableVector expected_value;
    VariableVector log_sd;
    std::vector<double> nll_vec;
    std::string nll_type;
    std::vector<std::string> key;

    bool simulate_flag = false;
    
    NormalNLLInterface() : NLLInterface(){}
    
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
        normal->simulate_flag = this->simulate_flag;
        if(this->nll_type == "data"){
            normal->osa_flag = true;
        } else {
            normal->osa_flag = false;
        }
        normal->observed_value.resize(this->observed_value.size());
        for(size_t i=0; i<this->observed_value.size(); i++){
            normal->observed_value[i] = this->observed_value[i].value;
            if(this->observed_value[i].estimable){
                model->parameters.push_back(&(normal)->observed_value[i]);
            }
        }
        normal->expected_value.resize(this->expected_value.size());
        for(size_t i=0; i<this->expected_value.size(); i++){
            normal->expected_value[i] = this->expected_value[i].value;
            if(this->expected_value[i].estimable){
                model->parameters.push_back(&(normal)->expected_value[i]);
            }
        }
        normal->log_sd.resize(this->log_sd.size());
        for(size_t i=0; i<this->log_sd.size(); i++){
            normal->log_sd[i] = this->log_sd[i].value;
            if(this->log_sd[i].estimable){
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

    /**
     * Update the model parameter values and finalize. Sets the parameter values and evaluates the
     * portable model once and transfers values back to the Rcpp interface.
     */
     
    void finalize(Rcpp::NumericVector v) {
        std::shared_ptr< Model<double> > model = Model<double>::getInstance();
        std::shared_ptr<NLLBase<double> > nll_base = model->nll_models[this->id];
        NormalNLL<double>* norm = (NormalNLL<double>*) nll_base.get();    

        for (int i = 0; i < v.size(); i++) {
            (*model->parameters[i]) = v[i];
        }

        double f = model->evaluate();
        /*
        this->observed_value.resize(norm->observed_value.size());
        for(int i=0; i<norm->observed_value.size(); i++){
            this->observed_value[i].value = norm->observed_value[i];
        }
        this->expected_value.resize(norm->expected_value.size());
        for(int i=0; i<norm->expected_value.size(); i++){
            this->expected_value[i].value = norm->expected_value[i];
        }
        this->log_sd.resize(norm->log_sd.size());
        Rcout << "log sd size after is: " << this->log_sd.size() << std::endl;
        for(int i=0; i<norm->log_sd.size(); i++){
            this->log_sd[i].value = norm->log_sd[i];
        }
        */
        this->nll_vec.resize(norm->nll_vec.size());
        for(int i=0; i<norm->nll_vec.size(); i++){
            this->nll_vec[i] = norm->nll_vec[i];
        }


    }

};


class MVNormNLLInterface : public NLLInterface{

public:
    VariableVector observed_value;
    VariableVector expected_value;
    VariableVector log_sd;
    VariableVector logit_phi;
    std::vector<double> nll_vec;
    std::string nll_type;
    std::vector<std::string> key;

    bool simulate_flag = false;
    
    MVNormNLLInterface() : NLLInterface(){}
    
    virtual ~MVNormNLLInterface() {}
    virtual uint32_t get_id() { return this->id; }
    
    virtual std::string get_module_name(){
        return "MVNormNLLInterface";
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
     std::shared_ptr<MVNormNLL<Type> > mvnorm =
        std::make_shared<MVNormNLL<Type> >();

        std::shared_ptr<Model<Type> > model = Model<Type>::getInstance();
        mvnorm->nll_type = this->nll_type;
        mvnorm->id = this->id;
        mvnorm->key.resize(this->key.size());
        for(int i=0; i<key.size(); i++){
            mvnorm->key[i] = this-> key[i];
        }
        mvnorm->simulate_flag = this->simulate_flag;
        if(this->nll_type == "data"){
           mvnorm->osa_flag = true;
        } else {
            mvnorm->osa_flag = false;
        }
         //initialize x and mu : how do I differentiate this from the SetX and SetMu functions above? flags?
        mvnorm->observed_value.resize(this->observed_value.size());
        for(size_t i=0; i<this->observed_value.size(); i++){
            mvnorm->observed_value[i] = this->observed_value[i].value;
            if(this->observed_value[i].estimable){
                model->parameters.push_back(&(mvnorm)->observed_value[i]);
            }
        }
        mvnorm->expected_value.resize(this->expected_value.size());
        for(size_t i=0; i<this->expected_value.size(); i++){
            mvnorm->expected_value[i] = this->expected_value[i].value;
            if(this->expected_value[i].estimable){
                model->parameters.push_back(&(mvnorm)->expected_value[i]);
            }
        }
        mvnorm->log_sd.resize(this->log_sd.size());
        for(size_t i=0; i<this->log_sd.size(); i++){
            mvnorm->log_sd[i] = this->log_sd[i].value;
            if(this->log_sd[i].estimable){
                model->parameters.push_back(&(mvnorm)->log_sd[i]);
                model->pnames.push_back("log_sd");
            }
        }

        mvnorm->logit_phi.resize(this->logit_phi.size());
        for(size_t i=0; i<this->logit_phi.size(); i++){
            mvnorm->logit_phi[i] = this->logit_phi[i].value;
            if(this->logit_phi[i].estimable){
                model->parameters.push_back(&(mvnorm)->logit_phi[i]);
                model->pnames.push_back("logit_phi");
            }
        }
    
        
        model->nll_models[mvnorm->id] = mvnorm;
        info->nll_models[mvnorm->id] = mvnorm;
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

    /**
     * Update the model parameter values and finalize. Sets the parameter values and evaluates the
     * portable model once and transfers values back to the Rcpp interface.
     */
    void finalize(Rcpp::NumericVector v) {
        
        std::shared_ptr< Model<double> > model = Model<double>::getInstance();
        std::shared_ptr<NLLBase<double> > nll_base = model->nll_models[this->id];
        MVNormNLL<double>* mvnorm = (MVNormNLL<double>*) nll_base.get();  

        for (int i = 0; i < v.size(); i++) {
            (*model->parameters[i]) = v[i];
        }

        double f = model->evaluate();

/*
        for(int i=0; i<mvnorm->observed_value.size(); i++){
            this->observed_value[i].value = mvnorm->observed_value[i];
        }
        for(int i=0; i<mvnorm->expected_value.size(); i++){
            this->expected_value[i].value = mvnorm->expected_value[i];
        }
        for(int i=0; i<mvnorm->Sigma.col(0).size(); i++){
            for(int j=0; j<mvnorm->Sigma.row(0).size(); j++){
                this->Sigma[i,j] = mvnorm->Sigma(i,j);
            }
        }
        */
        this->nll_vec.resize(mvnorm->nll_vec.size());
        for(int i=0; i<mvnorm->nll_vec.size(); i++){
            this->nll_vec[i] = mvnorm->nll_vec[i];
        }

    }

};




#endif