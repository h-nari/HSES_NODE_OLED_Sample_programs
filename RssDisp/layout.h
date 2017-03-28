#ifndef _layout_h_
#define _layout_h_

#define LayoutElemMax	6

struct LayoutElem {
  int16_t  x, y;
  uint16_t w, h;
  void (*disp_func)(struct LayoutElem *elem, bool bInit);
};

struct Layout {
  LayoutElem elem[LayoutElemMax];
};

void layout_set(Layout *layout);
void layout_update(Layout *layout);


#endif /* _layout_h_ */
