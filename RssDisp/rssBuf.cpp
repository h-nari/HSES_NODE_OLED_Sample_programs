#include "rssBuf.h"

RssBuf::RssBuf()
{
  clear();
}

void RssBuf::clear()
{
  m_sTitle = "";
  m_iBuf = 0;
  m_iItem = 0;
}

void RssBuf::addItem(const char *str)
{
  if(m_iItem >= RssBufItemMax - 1){
    Serial.printf("RssBuf ItemBuf overflow: %d\n", m_iItem);
    return;
  }
  
  size_t len = strlen(str);
  if( m_iBuf + len >= RssBufMax - 1){
    Serial.printf("RssBuf sBuf overflow: %d\n", m_iBuf + len);
    return;
  }

  m_aItem[m_iItem++] = m_sBuf + m_iBuf;
  strcpy(m_sBuf + m_iBuf, str);
  m_iBuf += len + 1;
}

const char *RssBuf::getItem(int idx)
{
  if(idx >= 0 && idx < m_iItem)
    return m_aItem[idx];
  else
    return NULL;
}

    
