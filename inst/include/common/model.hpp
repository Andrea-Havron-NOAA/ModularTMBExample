
#ifndef MODEL_HPP
#define MODEL_HPP

#include "def.hpp"
#include "information.hpp"
#include "../pop_dy/population.hpp"
#include "../pop_dy/von_bertalanffy.hpp"
#include "../nll/normal_nll.hpp"

template<typename Type>
class Model{
    public:

    std::vector<Type> predicted;
    std::shared_ptr<Population<Type> >  pop;
    std::shared_ptr< VonBertalanffy<Type> > vb;
    std::shared_ptr< NormalNLL<Type> > normal;


     std::map<uint32_t, std::shared_ptr<NLLBase<Type> > >
      nll_models;
    typedef typename std::map<
      uint32_t, std::shared_ptr<NLLBase<Type> > >::iterator
      nll_iterator;
      

    std::vector<Type*> parameters;
    static std::shared_ptr<Model<Type> > model;
    std::shared_ptr<Information<Type> > info;
    

    Model(){
        this->pop = std::make_shared<Population<Type> >();
        this->vb = std::make_shared<VonBertalanffy<Type> >();
        this->normal = std::make_shared<NormalNLL<Type> >();
    }


    //singleton instance based on Type
 //static std::shared_ptr<Model<Type> > instance;
  
  /**
   * Returns the sigleton instance of VonBertalanffyModel
   * of type Type.
   */
  static std::shared_ptr<Model<Type> > getInstance() {
    if (Model<Type>::model == nullptr) {
      Model<Type>::model = std::make_shared<Model<Type> >();
      Model<Type>::model->info = Information<Type>::getInstance();
    }
    return Model<Type>::model;
  }

  

     /**
   * Objective function to compute least squares
   * of observed and predicted length.
   */
  Type evaluate(){
    Type jnll = 0.0;
    
    //maybe here, setup functions can take a simulate flag and simulation can be controlled from model
    //setup pointers for priors
    this->info->setup_priors();
     for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type != "priors"){
        jnll += n->evaluate();
      }
    }

    /*
    //setup pointers for random effects
    //info->setup_random_effects();
    //evaluate nlls for priors and random effects
     for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type != "random_effects"){
        jnll += n->evaluate();
      }
    }
    */
    pop->vb = this->vb;
    pop->evaluate();
    this->info->setup_data();
    for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "data"){
        Rcout << "data nll key is: " << n->key << std::endl;
    //    jnll += n->evaluate();
      }
    }
    return jnll;

  }
  
  /**
   * clears the estimated parameter list
   */
  void clear(){
    this->parameters.clear();
  }
  
  };

template <typename Type>
std::shared_ptr<Model<Type> > Model<Type>::model = nullptr;  // singleton instance

#endif