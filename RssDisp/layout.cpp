#include <Arduino.h>
#include "layout.h"

void layout_set(Layout *layout)
{
  for(int i=0; i<LayoutElemMax; i++){
    LayoutElem *elem = &layout->elem[i];
    if(elem->disp_func)
      (elem->disp_func)(elem, true);
  }
}

void layout_update(Layout *layout)
{
  for(int i=0; i<LayoutElemMax; i++){
    LayoutElem *elem = &layout->elem[i];
    if(elem->disp_func)
      (elem->disp_func)(elem, false);
  }
}


