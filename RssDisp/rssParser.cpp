#include <Arduino.h>
#include "rssParser.h"
#include "rssBuf.h"

extern RssBuf rssBuf;

RssParser::RssParser()
{
}


void RssParser::process_tag_close(const char *tag, const char *text)
{
  if(strcmp(m_sPath,"/rdf:RDF/item/title")==0 ||
     strcmp(m_sPath,"/rss/channel/item/title") == 0){
    rssBuf.addItem(text);
  }
  else if(strcmp(m_sPath,"/rdf:RDF/channel/title")==0 ||
	  strcmp(m_sPath,"/rss/channel/title")==0){
    rssBuf.setTitle(text);
  }
}
