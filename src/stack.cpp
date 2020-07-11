#include "stack.h"

#include <iostream>
#include <stdio.h>

Stack::Stack()
{}

void Stack::Create(Type type, std::string_view name)
{
  std::cout << "Creating new [" << type.Name() << ":" << BaseTypeToString(type.Base()) << "]. Name: " << name << std::endl;
}
