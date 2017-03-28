#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include "XmlParser.h"

static const char cdata_begin_str[] = "![CDATA[";
static const char cdata_end_str[]   = "]]>";

XmlParser::XmlParser()
{
  init();
}

void XmlParser::init()
{
  m_mode = pm_text;
  m_sPath[0] = 0;
  m_bError = false;
  m_iText = 0;
}

void XmlParser::put(char c)
{
  switch(m_mode){
  case pm_text:
    if(c == '<')
      m_mode = pm_bracket;
    else
      process_text(c);
    break;
    
  case pm_bracket:
    if(c == '/'){
      m_bTagOpen = false;
      m_iTagName = 0;
      m_mode = pm_tag_name;
    }
    else if(c == cdata_begin_str[0]){
      m_mode = pm_cdata_begin;
      m_sMatching = cdata_begin_str + 1;
    }
    else if(isspace(c)){
      process_text('<');
      m_mode = pm_text;
    }
    else {
      m_bTagOpen = true;
      m_sTagName[0] = c;
      m_iTagName = 1;
      m_mode = pm_tag_name;
    }
    break;
    
  case pm_tag_name:
    if(c == '>'){
      xml_tag();
      m_mode = pm_text;
    }
    else if(c == '/'){
      xml_tag();
      m_bTagOpen = false;
      xml_tag();
      m_mode = pm_tag_slash;
    }
    else if(isspace(c)){
      m_mode = pm_tag_rest;
    }
    else {
      if(m_iTagName < TAG_NAME_MAX - 1)
	m_sTagName[m_iTagName++] = c;
    }
    break;

  case pm_tag_slash:
    if(c == '>') {
      m_mode = pm_text;
    }
    else {
      process_error("Unexpected charactor(%c) after /", c);
      m_mode = pm_text;
    }
    break;
    
  case pm_tag_rest:
    if(c == '>'){
      xml_tag();
      m_mode = pm_text;
    }
    else if(c == '/'){
      xml_tag();
      m_bTagOpen = false;
      xml_tag();
      m_mode = pm_tag_slash;
    }
    else if(isalpha(c))
      m_mode = pm_attr_name;
    else if(!isspace(c) && c != '?'){
      process_error("Unexpected charactor(%c) in tag",c);
      m_mode = pm_text;
    }
    break;

  case pm_attr_name:
    if(isalnum(c) || c == ':'){
      /* attr_name */
    }
    else if(c == '=')
      m_mode = pm_attr_value1;
    else if(isspace(c))
      m_mode = pm_attr_name2;
    else
      process_error("Unexpected charactor(%c) in attr_name",c);
    break;

  case pm_attr_name2:
    if(c == '=')
      m_mode = pm_attr_value1;
    else if(c == '>'){
      xml_tag();
      m_mode = pm_text;
    }
    else if(c == '/'){
      xml_tag();
      m_bTagOpen = false;
      xml_tag();
      m_mode = pm_tag_slash;
    }
    else if(isalpha(c))
      m_mode = pm_attr_name;
    else if(!isspace(c)){
      process_error("Unexpected charactor(%c) in tag",c);
      m_mode = pm_text;
    }
    break;

  case pm_attr_value1:
    if(c == '"')
      m_mode = pm_attr_value2;
    else if(!isspace(c)){
      process_error("Unexpected charactor(%c) in attr_value",c);
      m_mode = pm_text;
    }
    break;

  case pm_attr_value2:
    if(c == '"')
      m_mode = pm_tag_rest;
    break;

  case pm_cdata_begin:
    if(c != *m_sMatching++){
      const char *p = cdata_begin_str;
      while(p < m_sMatching)
	process_text(*p);
      m_mode = pm_text;
    } else if(*m_sMatching == 0)
      m_mode = pm_cdata;
    break;
      
  case pm_cdata:
    if(c == *cdata_end_str){
      m_mode = pm_cdata_end1;
      m_sMatching = cdata_end_str + 1;
    } else
      process_text(c);
    break;
    
  case pm_cdata_end1:
    if(c == ']')
      m_mode = pm_cdata_end2;
    else {
      process_text(']');
      process_text(c);
      m_mode = pm_cdata;
    }
    break;

  case pm_cdata_end2:
    if(c == '>')
      m_mode = pm_text;
    else if(c == ']'){
      process_text(']');
    } else {
      process_text(']');
      process_text(']');
      m_mode = pm_cdata;
    }
    break;
    
  default:
    process_error("Bad parse mode:%d\n", m_mode);
    m_mode = pm_text;
    break;
  }
}

void XmlParser::xml_tag(void)
{
  if(m_iTagName < TAG_NAME_MAX)
    m_sTagName[m_iTagName] = 0;
  else {
    process_error("Too big m_iTagName:%d >= %d\n", m_iTagName, TAG_NAME_MAX);
    m_sTagName[TAG_NAME_MAX-1] = 0;
  }
  if(m_bTagOpen) {
    if(m_sTagName[0] != '?'){
      int len1 = strlen(m_sPath);
      int len2 = strlen(m_sTagName);
      if(len1 + len2 + 2 >= PATH_MAX)
	process_error("Too long path:%s + %s", m_sPath, m_sTagName);
      else {
	m_sPath[len1] = '/';
	strcpy(m_sPath + len1 + 1, m_sTagName);
      }
      if(m_iText > 0){
	process_tag_open(m_sTagName, m_sText);
	m_iText = 0;
	m_sText[0] = 0;
      }
    }
  } else {
    if(m_iText > 0){
      process_tag_close(m_sTagName, m_sText);
      m_iText = 0;
      m_sText[0] = 0;
    }
    
    char *p = strrchr(m_sPath,'/');
    if(p && strcmp(p+1, m_sTagName)==0)
      *p = 0;
    else
      process_error("close tag for %s not found.", m_sTagName);
  }
}



void XmlParser::process_text(char c)
{
  if(m_iText > 0 || !isspace(c)){
    if(m_iText < TEXT_MAX-1){
      m_sText[m_iText++] = c;
      m_sText[m_iText] = 0;
    }
  }
}

void XmlParser::process_tag_open(const char *name, const char *text)
{
}

void XmlParser::process_tag_close(const char *name, const char *text)
{
}

void XmlParser::process_error(const char *fmt, ...)
{
  if(!m_bError){
#if 0
    FILE *ofp = stdout;
    va_list ap;

    fprintf(ofp,"XmlParser Error:\n");
    va_start(ap,fmt);
    vfprintf(ofp, fmt, ap);
    va_end(ap);
    fprintf(ofp,"\n");
#endif
    m_bError = true;
  }
}
