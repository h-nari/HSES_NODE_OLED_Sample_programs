#include "xmlParser.h"

class RssParser : public XmlParser {
 public:
  RssParser();

 protected:
  void process_tag_close(const char *tag, const char *text) override;

};
