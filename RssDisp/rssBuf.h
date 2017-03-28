#ifndef _rss_buf_h_
#define _rss_buf_h_

#include <Arduino.h>

#define RssBufMax	(1024*8)
#define RssBufItemMax	100

class RssBuf {
 protected:
  String m_sTitle;	// site title
  int  m_iBuf;		// write index
  int  m_iItem;		// item index
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
  
};

#endif /* _rss_buf_h_ */

	  
