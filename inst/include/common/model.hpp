
#ifndef MODEL_HPP
#define MODEL_HPP

#include "def.hpp"
#include "../pop_dy/population.hpp"

#include "../pop_dy/von_bertalanffy.hpp"
#include "../common/data.hpp"

template<typename Type>
class Model{
    public:

    std::vector<Type> predicted;
    std::shared_ptr<Population<Type> >  pop;
    std::shared_ptr< VonBertalanffy<Type> > vb;
    std::shared_ptr< ObsData<Type> > obsdata;

    std::vector<Type*> parameters;

    Model(){
        this->pop = std::make_shared<Population<Type> >();
        this->vb = std::make_shared<VonBertalanffy<Type> >();
        this->obsdata = std::make_shared<ObsData<Type> >();
    }


    //singleton instance based on Type
  static std::shared_ptr<Model<Type> > instance;
  
  /**
   * Returns the sigleton instance of VonBertalanffyModel
   * of type Type.
   */
  static std::shared_ptr<Model<Type> > getInstance(){
    return Model<Type>::instance; //TODO: make sure not null and create if null
  }

     /**
   * Objective function to compute least squares
   * of observed and predicted length.
   */
  Type evaluate(){
    Type norm2 = 0.0;
    pop->vb = this->vb;
    pop -> evaluate();
    for(int i =0; i < pop -> ages.size(); i++){
        this->predicted[i] = pop->length[i];
        norm2+=(this->predicted[i]-obsdata -> data[i])*
          (this->predicted[i]-obsdata -> data[i]);
    }
    return norm2;
  }
  
  /**
   * clears the estimated parameter list
   */
  void clear(){
    this->parameters.clear();
  }
  
  };

template<typename Type>
std::shared_ptr<Model<Type> > Model<Type>::instance = std::make_shared<Model<Type> >();

#endif