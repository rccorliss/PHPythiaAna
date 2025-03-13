#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdlib>
#include <ctime>
//#include <cmath>

#include <TPythia6.h>

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,8) 
#include <TMCParticle.h>
#else
#include <TMCParticle6.h>
#endif

#include <TLorentzVector.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>

#include <PHIODataNode.h>
#include <PHObject.h>
#include <PHCompositeNode.h>
#include <PHNodeIterator.h>
#include <PHNodeReset.h>
#include <PHTimeStamp.h>
#include <Fun4AllReturnCodes.h>
#include <getClass.h>

#include <PHPythiaHeader.h>
#include <PHPythiaContainer.h>
#include <RccAnaPHPythia.h>

using namespace std;

RccAnaPHPythia::RccAnaPHPythia(const std::string &name): SubsysReco(name)
{
  phpythia = 0;		// array of pythia particles
  phpythiaheader = 0;	// pythia header
}

RccAnaPHPythia::~RccAnaPHPythia()
{
}

// Done at the beginning of processing
int RccAnaPHPythia::InitRun(PHCompositeNode *topNode)
{
  Double_t pt_low = 0.1;
  Double_t pt_high = 5.;


  nProcessed=0;
 h_ptpi             = new TH1D("h_ptpi","pt",100,pt_low,pt_high);
  h_pt_gpi           = new TH1D("h_pt_gpi","pt_g",100,pt_low,pt_high);
  h_eta_gpi           = new TH1D("h_eta_gpi","eta_g",100,0,8);
  h_masspi           = new TH1D("hmasspi","mass",300,0.,4.);
  h_massfakepi           = new TH1D("hmassfakepi","mass",300,0.,4.);
  h_massmpcpi           = new TH1D("hmassmpcpi","mass",300,0.,4.);

  h_pt_gg = new TH2D("h_pt_gg","pt_gg",200,pt_low,pt_high,200,pt_low,pt_high);
  h_pt_fakegg = new TH2D("h_pt_fakegg","pt_fakegg",200,pt_low,pt_high,200,pt_low,pt_high);
  h_pt_fakeggcut = new TH2D("h_pt_fakeggcut","pt_fakeggcut",200,pt_low,pt_high,200,pt_low,pt_high);
  

  return EVENT_OK;
}

// Done at the end of processing
int RccAnaPHPythia::End(PHCompositeNode *topNode)
{
  //write an output file with all the histograms:
  TFile *f = new TFile("rccOutput.root","RECREATE");
  h_ptpi->Write();
  h_pt_gpi->Write();
  h_eta_gpi->Write();
  h_masspi->Write();
  h_massfakepi->Write();
  h_massmpcpi->Write();
  h_pt_gg->Write();
  h_pt_fakegg->Write();
  h_pt_fakeggcut->Write();
  f->Close();

  return EVENT_OK;
}

// Process each event
int RccAnaPHPythia::process_event(PHCompositeNode *topNode)
{
  // Get PYTHIA Header
  phpythiaheader = findNode::getClass<PHPythiaHeader>(topNode,"PHPythiaHeader");
  if (!phpythiaheader)
    {
      cout << PHWHERE << "Unable to get PHPythiaHeader, is Node missing?" << endl;
      return ABORTEVENT;
    }

  // Get PYTHIA Particles
  phpythia = findNode::getClass<PHPythiaContainer>(topNode,"PHPythia");
  if (!phpythia)
    {
      cout << PHWHERE << "Unable to get PHPythia, is Node missing?" << endl;
      return ABORTEVENT;
    }

  nProcessed++;
  if (nProcessed<10 || nProcessed%100==0) {
  
    // Print some header information
    cout << "Event: " << phpythiaheader->GetEvt() << "\t" << phpythiaheader->GetNpart() << endl;
    
    cout << "KS\tKF\tpid\tname\thistory" << endl;
    // Print information from each 
    int npart = phpythia->size();
    for (int ipart=0; ipart<npart; ipart++)
      {
        TMCParticle *part = phpythia->getParticle(ipart);
        //TMCParticle *parent = phpythia->getParent(part);
        cout << setw(4) << phpythia->getLineNumber(part)
            << "\t" << part->GetKS() 
            << setw(8)  <<  part->GetKF() // << "\t" << part->GetEnergy() 
            << setw(12) << part->GetName()
            //<< "\t" << ipart 
            //<< "\t" << part->GetParent()
            //<< "\t" << part->GetFirstChild()
            //<< "\t" << part->GetLastChild()
            //<< "\t" << phpythia->getChildNumber(part)
            << "\t" << phpythia->getHistoryString(part)
            << endl;
      }
  }
  //holders for the two current photons, and a counter for how many we have found in this event:
  TLorentzVector photon1(0,0,0,0);
  TLorentzVector photon2(0,0,0,0);
  int nPhotons = 0;

  //loop over particles to find the two with name "Photon"
  for (unsigned int ipart=0; ipart<phpythia->size(); ipart++)
    {
      TMCParticle *part = phpythia->getParticle(ipart);
      if (strcmp(part->GetName(),"gamma")==0)
        {
      TLorentzVector v;
      v.SetPxPyPzE(part->GetPx(), part->GetPy(), part->GetPz(), part->GetEnergy());
          if (nPhotons==0) {
            photon1 = v;
            nPhotons++;
          } else if (nPhotons==1) {
            photon2 = v;
            nPhotons++;
          }
          else {
            cout << "Found more than 2 photons in this event, skipping the rest" << endl;
            break;
          }
        }
    }

  //if we found two photons, histogram their pt and the invariant mass:
  if (nPhotons==2) {
    TLorentzVector pion=photon1+photon2;
    h_ptpi->Fill(pion.Pt());
    h_pt_gpi->Fill(photon1.Pt());
    h_pt_gpi->Fill(photon2.Pt());
    h_eta_gpi->Fill(photon1.Eta());
    h_eta_gpi->Fill(photon2.Eta());
    h_pt_gg->Fill(photon1.Pt(),photon2.Pt());

    h_masspi->Fill(pion.M());

  }

  //mix each photon with the last N photons:
  for (unsigned int i=0; i<previousPhoton.size(); i++) {
    if(nPhotons>0) {
      TLorentzVector fake=photon1+previousPhoton[i];
      h_massfakepi->Fill(fake.M());
      h_pt_fakegg->Fill(photon1.Pt(),previousPhoton[i].Pt());
      if (nPhotons>1) {
        fake=photon2+previousPhoton[i];
        h_massfakepi->Fill(fake.M());
        h_pt_fakegg->Fill(photon2.Pt(),previousPhoton[i].Pt());
      }
    }
  }
  //add our photons to the list of previous photons:
  if (nPhotons>0) {
    previousPhoton.push_back(photon1);
    if (previousPhoton.size()>maxPhotons)
      {
        previousPhoton.erase(previousPhoton.begin());
      }
  }
  if (nPhotons>1) {
    previousPhoton.push_back(photon2);
    if (previousPhoton.size()>maxPhotons)
      {
        previousPhoton.erase(previousPhoton.begin());
      }
  }

  return EVENT_OK;
}

