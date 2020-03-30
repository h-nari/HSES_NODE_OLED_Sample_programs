#ifndef _layout_h_
#define _layout_h_

#define LayoutElemMax	6

#include <stdio.h>
#include <stdint.h>

typedef void (*layout_disp_f)(struct LayoutElem *elem, bool bInit);

struct LayoutElem {
  int16_t  x, y;
  uint16_t w, h;
  layout_disp_f disp_func;
};

struct Layout {
  LayoutElem elem[LayoutElemMax];
};

void layout_set(Layout *layout);
void layout_update(Layout *layout, layout_disp_f except = NULL);


#endif /* _layout_h_ */
