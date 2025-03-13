#ifndef __RCCANAPHPYTHIA_H__
#define __RCCANAPHPYTHIA_H__

#include "SubsysReco.h"
#include <TLorentzVector.h>

class PHCompositeNode;
class PHPythiaHeader;
class PHPythiaContainer;

class TH1D;
class TH2D;
//class TLorentzVector;

class RccAnaPHPythia: public SubsysReco
{
public:
  RccAnaPHPythia(const std::string &name = "RccAnaPHPythia");
  virtual ~RccAnaPHPythia();

  // Methods Derived from SubsysReco
  int InitRun(PHCompositeNode *topNode);
  int process_event(PHCompositeNode *topNode);
  int End(PHCompositeNode *topNode);

  //std object to hold the last N photons (TMCParticle):
  std::vector<TLorentzVector> previousPhoton;
  int nProcessed;

  //settings:
  static const  int maxPhotons=10;


  //histograms to hold info about real and fake photons:
  TH1D *h_ptpi;
  TH1D *h_pt_gpi;
  TH1D *h_eta_gpi;
  TH1D *h_masspi;
  TH1D *h_massfakepi;
  TH1D *h_massmpcpi;

  TH2D *h_pt_gg;
  TH2D *h_pt_fakegg;
  TH2D *h_pt_fakeggcut;
  
protected:

  PHPythiaHeader *phpythiaheader;
  PHPythiaContainer *phpythia;
};

#endif	/* __ANAPHPYTHIA_H__ */

