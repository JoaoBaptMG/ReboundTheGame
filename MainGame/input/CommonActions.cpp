#include "CommonActions.hpp"
#include <iostream>

void ButtonActionBase::update()
{
    last2 = last1; last1 = cur;
}
