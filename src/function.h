#ifndef FUNCTIONBASEMENT_H
#define FUNCTIONBASEMENT_H

#include <string>
#include <vector>
#include <memory>

#include "parsers.h"
#include "trigger.h"

enum class FunctionType {
  Base,
  Physical,
  Standart,
  Trigger
};

enum class FunctionStatus {
  NotActivated,
  Running,
  BodyCompleted,
  // TODO think if we need state "whait for tail fin"
  finished // all tail functions of that function recursevely completed
};

class FunctionBasement
{
 public:
  using Functions = std::vector<std::shared_ptr<FunctionBasement>>;

  using Events = std::vector<EventSignature>;
  FunctionBasement(FunctionType type, FunctionBasement* parent_ = nullptr);
  virtual void FunctionCalled();
  /// should be called befor adding to pool of active functions
  virtual bool check_state() { return true; }
  virtual void Init(UnificatedArguments&& target_components);
  Events ReceiveEvents();
  bool need_to_delete() const { return need_to_delete_; }
  FunctionType type() const { return type_; }
  FunctionStatus status() const { return status_; }
  bool is_initialized() const {return is_initialized_; }
 protected:
  virtual void ChildFunctionStatusUpdated(const FunctionBasement *child_func);
  void DeleteFunction();
  FunctionStatus status_;
  Events event_pool_;
  bool is_initialized_ = false;
 private:
  FunctionType type_;
  int id_;
  static int id_setter_;
  std::shared_ptr<FunctionBasement> parent_ = nullptr;
  bool need_to_delete_ = false;
};

class PhysicalFunction : public FunctionBasement {
 public:
  PhysicalFunction(FunctionBasement* parent = nullptr)
    : FunctionBasement(FunctionType::Physical, parent),
      function_signature_("") {}
  PhysicalFunction(UnificatedArguments&& target_components,
                   FunctionBasement* parent = nullptr);
  virtual void Init(UnificatedArguments&& target_components) override final;
  virtual void FunctionCalled() override final;
  const std::string& target_component() const { return target_component_; }
  FunctionSignature& function_signature() { return function_signature_; }
 private:
  std::string target_component_; // first arg
  int left_lifetime_; // second arg
  FunctionSignature function_signature_; // all that left

};

class StandartFunction : public FunctionBasement {
 public:
  virtual bool check_state() override;
  StandartFunction(FunctionSignature trigger_signature,
                   std::vector<FunctionSignature> body_signatures,
                   std::vector<FunctionSignature> tail_signatures) : FunctionBasement(FunctionType::Standart) {}
  virtual void FunctionCalled() override;

 private:
  /// checks state onse when called
  StaticTrigger static_trigger_;
  Functions body_funcs_;
  Functions tail_funcs_;
};


//sets some triggers
class TriggerFunction : public FunctionBasement {
 public:
  TriggerFunction() : FunctionBasement(FunctionType::Trigger) {}
};

#endif // FUNCTIONBASEMENT_H
