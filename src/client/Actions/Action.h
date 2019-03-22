//
// Created by j42-king on 22/03/2019.
//

#ifndef PROJECT_ACTION_H
#  define PROJECT_ACTION_H

#endif // PROJECT_ACTION_H

class Action
{
 public:
  virtual ~Action() {} // Declare a virtual destuctor in
  // the base class. All derived types
  // will inherit a virtual destructor.

  virtual void execute() = 0;
};