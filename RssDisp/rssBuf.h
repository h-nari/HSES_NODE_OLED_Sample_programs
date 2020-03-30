#ifndef _rss_buf_h_
#define _rss_buf_h_

#include <Arduino.h>

#define RssBufMax	(1024*8)
#define RssBufItemMax	100

class RssBuf {
 protected:
  String m_sTitle;	// site title
  int  m_iBuf;		  // write index
  int  m_iItem;		  // item index
  int  m_iItemMax;  // 記憶する記事の上限、 -1なら無制限
  char m_sBuf[RssBufMax];
  char *m_aItem[RssBufItemMax];

 public:
  RssBuf();
  void clear();
  void setTitle(const char *title) { m_sTitle = title;}
  void addItem(const char *item);
  const char *getTitle() { return m_sTitle.c_str();}
  int getItemCount() {return m_iItem;}
  const char* getItem(int idx);
  void setItemMax(int v){ m_iItemMax = v;}
  bool isFull() { return m_iItemMax > 0 && m_iItem >= m_iItemMax;}
  
};

#endif /* _rss_buf_h_ */

	  
