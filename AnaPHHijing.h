#ifndef __ANAPHHIJING_H__
#define __ANAPHHIJING_H__

#include <SubsysReco.h>

class PHCompositeNode;
class PHHijingHeader;
class PHPythiaContainer;

class AnaPHHijing: public SubsysReco
{
public:
  AnaPHHijing(const std::string &name = "AnaPHHijing");
  virtual ~AnaPHHijing();

  // Methods Derived from SubsysReco
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  PHHijingHeader *phhijingheader;
  PHPythiaContainer *phpythia;

};

#endif	/* __ANAPHHIJING_H__ */

