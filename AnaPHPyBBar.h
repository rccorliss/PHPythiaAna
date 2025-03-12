#ifndef __ANAPHPYBBAR_H__
#define __ANAPHPYBBAR_H__

#include "SubsysReco.h"
#include "TH1F.h"
#include "TFile.h"
#include "getClass.h"
#include "TChain.h"
#include "TMCParticle.h"
#include <vector>
#include <iostream>
#include <iterator>
#include <stdlib.h>


class PHCompositeNode;
class PHPythiaHeader;
class PHPythiaContainer;

//const int MAXMUONS = 10;
const double mu = 0.1056583;
const double pi = 4.0*atan(1.0);

//Histograms cut to PHENIX acceptance
const double eta_min = 1.2;
const double eta_max = 2.2;
//const double eta_min = 1.0948;		//Robert used 1.15423;
//const double eta_max = 2.5421;		//Robert used 2.38719;
const double p_min = 1.76;

class AnaPHPyBBar: public SubsysReco
{
 public:
  // constructor
  AnaPHPyBBar(const std::string &name = "AnaPHPyBBar");
  // destructor
  virtual ~AnaPHPyBBar();
  
  // Methods Derived from SubsysReco
  int InitRun(PHCompositeNode *topNode);         // initialize run
  int process_event(PHCompositeNode *topNode);   // process event (event method)
  int End(PHCompositeNode *topNode);            // end of job
  void SetOutputFile(char* fname){ filename = fname; }   
  void SetVerbosity(bool verbose){ _verbosity = verbose; }
  double square( double x ){ return x*x; }
  //TH1F*& get_invmass_like(){ return invmass_like; }
  //TH1F*& get_invmass_N(){ return invmass_like_N; }
  //TH1F*& get_invmass_S(){ return invmass_like_S; }
  void SetPtCut( double pt ){ _pt_cut = pt; }
  bool both_muon_in_muon_arm(TMCParticle*,TMCParticle*);

 protected:
  
  PHPythiaHeader *phpythiaheader;
  PHPythiaContainer *phpythia;
  std::string filename;
  TFile* outfile;
  bool _verbosity;
  int nevents;
  double _pt_cut;

  std::vector<TMCParticle> Particle_mm;
  std::vector<TMCParticle> Particle_pp;
  std::vector<TMCParticle> primary_BB;

  
  // in 4pi space
  TH1F *invmass_pm_ptcut;
  TH1F *invmass_pm;
  TH1F *invmass_like;
  TH1F *invmass_like_ptcut;
  TH1F *invmass_osc;  
  TH1F *invmass_osc_ptcut;  
  TH1F *invmass_decay;
  TH1F *invmass_decay_ptcut;
  TH1F *invmass_primary;
  TH1F *invmass_primary_ptcut;

  //Muon Arm
  TH1F *invmass_pm_muarm;
  TH1F *invmass_pm_muarm_ptcut;
  TH1F *invmass_like_muarm;
  TH1F *invmass_like_muarm_ptcut;
  TH1F *invmass_osc_muarm;
  TH1F *invmass_osc_muarm_ptcut;
  TH1F *invmass_decay_muarm;
  TH1F *invmass_decay_muarm_ptcut;
  TH1F *invmass_primary_muarm;
  TH1F *invmass_primary_muarm_ptcut;

};

#endif	/* __ANAPHPYBBAR_H__ */

