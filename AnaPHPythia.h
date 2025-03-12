#ifndef __ANAPHPYTHIA_H__
#define __ANAPHPYTHIA_H__

#include "SubsysReco.h"

class PHCompositeNode;
class PHPythiaHeader;
class PHPythiaContainer;

class AnaPHPythia: public SubsysReco
{
public:
  AnaPHPythia(const std::string &name = "AnaPHPythia");
  virtual ~AnaPHPythia();

  // Methods Derived from SubsysReco
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

protected:

  PHPythiaHeader *phpythiaheader;
  PHPythiaContainer *phpythia;

};

#endif	/* __ANAPHPYTHIA_H__ */

