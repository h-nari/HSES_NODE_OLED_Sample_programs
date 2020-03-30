#include "layout.h"

#include <Arduino.h>

void layout_set(Layout *layout) {
  for (int i = 0; i < LayoutElemMax; i++) {
    LayoutElem *elem = &layout->elem[i];
    if (elem->disp_func) (elem->disp_func)(elem, true);
  }
}

void layout_update(Layout *layout, layout_disp_f except) {
  for (int i = 0; i < LayoutElemMax; i++) {
    LayoutElem *elem = &layout->elem[i];
    if (elem->disp_func && elem->disp_func != except)
      (elem->disp_func)(elem, false);
  }
}
