#ifndef EORESETTABLE_H
#define EORESETTABLE_H

/**
Base class for objects that be reset

  @ingroup Core
 */
class eoResettable
{
 public:
  /// Virtual dtor. They are needed in virtual class hierarchies.
  virtual ~eoResettable() {}

  /**
   * Reset whatever values might be reinitialized
   */
  virtual void reset()
  {
    std::cout << "reset in eoEvalFuncCounter" << std::endl;            
  };
};

#endif