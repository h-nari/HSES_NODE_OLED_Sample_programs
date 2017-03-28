#define TAG_NAME_MAX	20
#define TEXT_MAX	80*5
#define PATH_MAX	256

class XmlParser {
 protected:
  enum ParserMode{
    pm_text,
    pm_bracket,
    pm_tag_name,
    pm_tag_rest,
    pm_tag_slash,
    pm_attr_name,
    pm_attr_name2,
    pm_attr_value1,
    pm_attr_value2,
    pm_cdata_begin,
    pm_cdata,
    pm_cdata_end1,
    pm_cdata_end2,
  };
  ParserMode m_mode;
  char m_sTagName[TAG_NAME_MAX];
  int  m_iTagName;
  bool m_bTagOpen;
  bool m_bOut;
  const char *m_sMatching;
  char m_sPath[PATH_MAX];
  char m_sText[TEXT_MAX];
  int  m_iText;
  bool m_bError;
  
 public:
  XmlParser();
  void init();
  void put(char c);

 protected:
  void xml_tag(void);
  
  void virtual process_text(char c);
  void virtual process_tag_open(const char *tag, const char *text);
  void virtual process_tag_close(const char *tag, const char *text);
  void virtual process_error(const char *fmt, ...);
};
