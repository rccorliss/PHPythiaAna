#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <TROOT.h>
#include <TPythia6.h>
#include <TMCParticle.h>
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
#include <AnaPHPyBBar.h>

#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include "header.h"
#include <TStyle.h>
#include <TLegend.h>
#include <TRandom.h>

using namespace std;

AnaPHPyBBar::AnaPHPyBBar(const std::string &name): SubsysReco(name)
{
  phpythia = 0;		// array of pythia particles
  phpythiaheader = 0;	// pythia header
  filename = "anaphpythia.root";
  _verbosity = false;
  nevents = 0;
  _pt_cut =0; //by default no pt cut
}
AnaPHPyBBar::~AnaPHPyBBar()
{
  delete invmass_like;
  delete invmass_like_muarm;
  delete invmass_like_ptcut;
  delete invmass_like_muarm_ptcut;
  
  delete invmass_primary;
  delete invmass_primary_muarm;
  delete invmass_primary_ptcut;
  delete invmass_primary_muarm_ptcut;
  
  delete invmass_osc;
  delete invmass_osc_muarm;
  delete invmass_osc_ptcut;
  delete invmass_osc_muarm_ptcut;
  
  delete invmass_decay;
  delete invmass_decay_ptcut;
  delete invmass_decay_muarm;
  delete invmass_decay_muarm_ptcut;
  
  delete invmass_pm;
  delete invmass_pm_ptcut;
  delete invmass_pm_muarm;
  delete invmass_pm_muarm_ptcut;
  
}

// Done at the beginning of processing
int AnaPHPyBBar::InitRun(PHCompositeNode *topNode)
{ 
  outfile = new TFile( filename.c_str(), "RECREATE" );
  
  invmass_pm = new TH1F("invmass_pm","all like-sign pairs in 4pi acc", 160, 0, 16);
  invmass_pm_ptcut = new TH1F("invmass_pm_ptcut","all like-sign pairs with pT>1  in 4pi acc", 160, 0, 16);
  invmass_pm_muarm = new TH1F("invmass_pm_muarm","all like-sign pairs in Muon Arm", 160, 0, 16);
  invmass_pm_muarm_ptcut = new TH1F("invmass_pm_muarm_ptcut","all like-sign pairs with pT>1 in Muon Arm", 160, 0, 16);

  invmass_like = new TH1F("invmass_like","all like-sign pairs in 4pi acc", 160, 0, 16);
  invmass_like_ptcut = new TH1F("invmass_like_ptcut","all like-sign pairs with pT>1  in 4pi acc", 160, 0, 16);
  invmass_like_muarm = new TH1F("invmass_like_muarm","all like-sign pairs in Muon Arm", 160, 0, 16);
  invmass_like_muarm_ptcut = new TH1F("invmass_like_muarm_ptcut","all like-sign pairs with pT>1 in Muon Arm", 160, 0, 16);
  
  invmass_primary = new TH1F("invmass_primary","all like-sign pairs in 4pi acc", 160, 0, 16);
  invmass_primary_ptcut = new TH1F("invmass_primary_ptcut","all like-sign pairs with pT>1  in 4pi acc", 160, 0, 16);
  invmass_primary_muarm = new TH1F("invmass_primary_muarm","all like-sign pairs in Muon Arm", 160, 0, 16);
  invmass_primary_muarm_ptcut = new TH1F("invmass_primary_muarm_ptcut","all like-sign pairs with pT>1 in Muon Arm", 160, 0, 16);
  
  invmass_osc = new TH1F("invmass_like_osc","oscillation like-sign pairs in 4pi acc", 160, 0, 16);
  invmass_osc_ptcut = new TH1F("invmass_like_osc_ptcut","oscillation like-sign pairs with pT>1 in 4pi acc", 160, 0, 16);
  invmass_osc_muarm = new TH1F("invmass_like_osc_muarm","oscillation like-sign pairs in Muon Arm", 160, 0, 16);
  invmass_osc_muarm_ptcut = new TH1F("invmass_like_osc_muarm_ptcut","oscillation like-sign pairs with pT>1 in Muon Arm", 160, 0, 16);
  
  invmass_decay = new TH1F("invmass_like_decay","Decay like-sign pairs in 4pi acc", 160, 0, 16);
  invmass_decay_muarm = new TH1F("invmass_like_decay_muarm","Decay like-sign pairs in Muon Arm", 160, 0, 16);
  invmass_decay_ptcut = new TH1F("invmass_like_decay_ptcut","Decay like-sign pairs with pT>1 in 4pi acc", 160, 0, 16);
  invmass_decay_muarm_ptcut = new TH1F("invmass_like_decay_muarm_ptcut","Decay like-sign pairs with pT>1 in Muon Arm", 160, 0, 16);

  cout << "SINGLE Muon PT cut:" << _pt_cut << endl;
  cout << "Verbosity: " << _verbosity << endl;
  cout << "Outputfile: " << filename << endl;
  return EVENT_OK;
}
 
// Done at the end of processing
int AnaPHPyBBar::End(PHCompositeNode *topNode)
{
  if( outfile ) 
  {
   outfile->Write();
   outfile->Close();
  }
  return EVENT_OK;
}


////////////////////////////////////////////////////////////////////
// Process each event
int AnaPHPyBBar::process_event(PHCompositeNode *topNode)
{
  nevents++;
  if(nevents%10000==0)
   {
     cout << "AnaPHPyBBar:: Processing Event " << nevents << endl;
   } 
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

 int npart = phpythia->size();

 //Lets not get bothered by single muon only events
  int num_muons(0);
  for( int ipart=0; ipart<npart; ipart++)
  {
   if( (abs( phpythia->getParticle(ipart)->GetKF() )==13) ) num_muons++;
      // (sqrt( square(phpythia->getParticle(ipart)->GetPx()) + square(phpythia->getParticle(ipart)->GetPy()))> _pt_cut) )
     // num_muons++ ;
  }
   if( num_muons < 2 ) return ABORTEVENT;

 //define vectors to be filled with particle objects
  Particle_mm.clear();
  Particle_pp.clear();
  primary_BB.clear();



  // Get Particle Information
  if( _verbosity )
  {
    cout << endl;
    cout << "Event number: " << nevents << endl;
  }

  for (int ipart=0; ipart<npart; ipart++)
    {
      TMCParticle *part = phpythia->getParticle(ipart);
      if(abs(part->GetKF()) != 13)continue;
      bool Anc = phpythia->hasAncestorAbs(part,5);   // 5 means b or bbar quark
      if(!Anc) continue;
      //if(sqrt( square(phpythia->getParticle(ipart)->GetPx()) + square(phpythia->getParticle(ipart)->GetPy()))<_pt_cut) continue;
      if( _verbosity )
      {
       cout << phpythia->getHistoryString(part) << endl;
       cout << "mu info: E: " << part->GetEnergy() << " Px: " << part->GetPx() << " Py: " << part->GetPy()
	   << " Pz: " << part->GetPz() << " Pt: " << sqrt( square(phpythia->getParticle(ipart)->GetPx()) + square(phpythia->getParticle(ipart)->GetPy())) 
		<<endl;
      }

  
      if(part->GetKF()==-13)Particle_pp.push_back(*part); //fill vector with only + charge particles
      if(part->GetKF()==13)Particle_mm.push_back(*part); // fill vector with only - charged particles
      
      if(  (abs( phpythia->getParent(part )->GetKF()) == 511) ||
           (abs( phpythia->getParent(part )->GetKF()) == 521) ||
           (abs( phpythia->getParent(part )->GetKF()) == 531) ||
           (abs( phpythia->getParent(part )->GetKF()) == 541) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5122) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5132) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5142) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5232) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5242) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5332) ||
           (abs( phpythia->getParent(part )->GetKF()) == 5342) )
       primary_BB.push_back(*part);
      
    }
 
  
  //for two likesign pairs ++, --
  std::vector<TMCParticle> likesign_container;
  for(int i=0; i<2; i++)
  {
   if(i==0)
   {
     likesign_container.clear();
     likesign_container = Particle_pp ;
   }else{
     likesign_container.clear();
     likesign_container = Particle_mm ;
   }
  	
   //check if both muons are primary 
   for( vector<TMCParticle>::iterator ptrk1 = primary_BB.begin(); ptrk1!= primary_BB.end(); ptrk1++)
     {
       for( vector<TMCParticle>::iterator ptrk2 = ptrk1+1; ptrk2!= primary_BB.end(); ptrk2++)
	 {
	   double pinvmass  =    std::sqrt( square( ptrk1->GetEnergy() + ptrk2->GetEnergy() ) - 
					    (square( ptrk1->GetPx() + ptrk2->GetPx() ) +
					     square( ptrk1->GetPy() + ptrk2->GetPy() ) +
					     square( ptrk1->GetPz() + ptrk2->GetPz() ) ) );
	   
	   invmass_primary->Fill(pinvmass);
	   //cout << "Two primary: " << endl;
	   //cout <<  phpythia->getHistoryString( &*ptrk1 ) << endl;
	   //cout <<  phpythia->getHistoryString( &*ptrk2 ) << endl;
	   //cout << endl;
	   /*
	   if( ptrk1->GetKF() * ptrk2->GetKF() <0 )
	     {
	       h_primary_unlike->Fill( pinvmass );
	     } else{
	     h_primary_like->Fill( pinvmass );
	     if( phpythia->getParent(&*ptrk1)->GetKS()!=12 && phpythia->getParent(&*ptrk2)->GetKS()!=12 )
	       {
		 //technically you should never see this
		 cout << "Neither osscillated" << endl;
		 cout <<  phpythia->getHistoryString( &*ptrk1 ) << " KS: " << phpythia->getParent( &*ptrk1 )->GetKS() << endl;
		 cout <<  phpythia->getHistoryString( &*ptrk2 ) << " KS: " << phpythia->getParent( &*ptrk2 )->GetKS() << endl;
	       }
	   }*/ 


	   if( both_muon_in_muon_arm( &*ptrk1, &*ptrk2)) invmass_primary_muarm->Fill(pinvmass);
	   if( (sqrt(square(ptrk1->GetPx()) + square(ptrk1->GetPy()))>_pt_cut) &&
	       (sqrt(square(ptrk2->GetPx()) + square(ptrk2->GetPy()))>_pt_cut)  ) invmass_primary_ptcut->Fill(pinvmass);
	   if( both_muon_in_muon_arm( &*ptrk1, &*ptrk2) &&
	       (sqrt(square(ptrk1->GetPx()) + square(ptrk1->GetPy()))>_pt_cut) &&
	       (sqrt(square(ptrk2->GetPx()) + square(ptrk2->GetPy()))>_pt_cut)  ) invmass_primary_muarm_ptcut->Fill(pinvmass);
	   
	 }
     }    



	
  for( vector<TMCParticle>::iterator trk1 = likesign_container.begin(); trk1!= likesign_container.end(); trk1++)
   {
      for( vector<TMCParticle>::iterator trk2 = trk1+1; trk2!= likesign_container.end(); trk2++)
       {
        double invmass  =    std::sqrt( square( trk1->GetEnergy() + trk2->GetEnergy() ) - (
		                        square( trk1->GetPx() + trk2->GetPx() ) +
 		                        square( trk1->GetPy() + trk2->GetPy() ) +
		                        square( trk1->GetPz() + trk2->GetPz() ) ) );
        invmass_like->Fill(invmass); 
	if( (sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
            (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut)  ) invmass_like_ptcut->Fill(invmass);

        if( both_muon_in_muon_arm( &*trk1, &*trk2))invmass_like_muarm->Fill(invmass);

	if( both_muon_in_muon_arm( &*trk1, &*trk2) &&
            (sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
            (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut)  )  invmass_like_muarm_ptcut->Fill(invmass);       
        
        if( _verbosity)
        {
         cout << "Inv. Mass: " << invmass << endl;
        }

       //check oscilation
       if( (phpythia->getParent( &*trk1 )->GetKS()==12) || (phpythia->getParent( &*trk2 )->GetKS()==12) ) 
        {
          //check the non-oscillating one 
          TMCParticle* non_oss_part = (phpythia->getParent( &*trk1 )->GetKS()==12)? 
			 &*trk2 : &*trk1 ;
          //TMCParticle* oss_part = (phpythia->getParent( &*trk1 )->GetKS()==12)?
          //               &*trk1 : &*trk2 ;
            if( (abs( phpythia->getParent( non_oss_part )->GetKF()) == 511) || 
                (abs( phpythia->getParent( non_oss_part )->GetKF()) == 521) ||
                (abs( phpythia->getParent( non_oss_part )->GetKF()) == 531) || 
                (abs( phpythia->getParent( non_oss_part )->GetKF()) == 541) ||
                (abs( phpythia->getParent( non_oss_part )->GetKF()) == 5122)||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5132) ||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5142) ||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5232) ||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5242) ||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5332) ||
		(abs( phpythia->getParent( non_oss_part )->GetKF()) == 5342)  )
             {
               invmass_osc->Fill(invmass);
	       if((sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
		  (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut)) invmass_osc_ptcut->Fill(invmass);

	       if( both_muon_in_muon_arm( &*trk1, &*trk2))invmass_osc_muarm->Fill(invmass);

               if( both_muon_in_muon_arm( &*trk1, &*trk2) &&
		   (sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
                   (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut)) invmass_osc_muarm_ptcut->Fill(invmass);
	     }
	    
	}
      

       //check B-decay chain
        int kf1 = abs( phpythia->getParent( &*trk1 )->GetKF() );
        int kf2 = abs( phpythia->getParent( &*trk2 )->GetKF() );
        if( (kf1==411 || kf1==421 || kf1==431 || kf1==413 || kf1==423 || kf1==4433) ||
	    (kf2==411 || kf2==421 || kf2==431 || kf2==413 || kf2==423 || kf2==4433) )
        {
          invmass_decay->Fill(invmass);
	  if( both_muon_in_muon_arm( &*trk1, &*trk2)) invmass_decay_muarm->Fill(invmass);
	  
	  if((sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
	     (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut) ) invmass_decay_ptcut->Fill(invmass);
	  
	  if( both_muon_in_muon_arm( &*trk1, &*trk2) &&
	       (sqrt(square(trk1->GetPx()) + square(trk1->GetPy()))>_pt_cut) &&
               (sqrt(square(trk2->GetPx()) + square(trk2->GetPy()))>_pt_cut) ) invmass_decay_muarm_ptcut->Fill(invmass);

          //cout <<  phpythia->getHistoryString( &*trk1 ) << endl;
          //cout <<  phpythia->getHistoryString( &*trk2 ) << endl;
          //cout << endl;
        }



      } //1st muon look
  }  //2nd muon loop

  } //for ++ and --
  
 
  // make pairs for all +- 
  unsigned int l;
  unsigned int ll;
 for(l=0; l<Particle_mm.size(); l++){  //loop over first mu- in the event
   double pT1 = sqrt(Particle_mm[l].GetPx()*Particle_mm[l].GetPx() + Particle_mm[l].GetPy()*Particle_mm[l].GetPy()); //calculate pT from mu-
   double y1=0.5*log((Particle_mm[l].GetEnergy() + Particle_mm[l].GetPz())/((Particle_mm[l].GetEnergy() - Particle_mm[l].GetPz())));
   
   for(ll=0; ll<Particle_pp.size(); ll++){  //loop over first mu+ in the event
     double pT2 = sqrt(Particle_pp[ll].GetPx()*Particle_pp[ll].GetPx() + Particle_pp[ll].GetPy()*Particle_pp[ll].GetPy()); //calculate pT from mu+
     double y2=0.5*log((Particle_pp[ll].GetEnergy() + Particle_pp[ll].GetPz())/((Particle_pp[ll].GetEnergy() - Particle_pp[ll].GetPz())));
     
     double invmass_PM = sqrt(2.0*(mu*mu + Particle_mm[l].GetEnergy()*Particle_pp[ll].GetEnergy() - Particle_mm[l].GetPx()*Particle_pp[ll].GetPx() -Particle_mm[l].GetPy()*Particle_pp[ll].GetPy() - Particle_mm[l].GetPz()*Particle_pp[ll].GetPz()));
     invmass_pm->Fill(invmass_PM);
     if(pT1>1 && pT2>1)invmass_pm_ptcut->Fill(invmass_PM);
     
     if((y1 < 2.2) && (y1 > 1.2) && (y2 < 2.2) && (y2 > 1.2)) {
       invmass_pm_muarm->Fill(invmass_PM); // north arm
       if(pT1>1 && pT2>1)invmass_pm_muarm_ptcut->Fill(invmass_PM); //north arm with min pT cut
     }
     else if((y1 > -2.2) && (y1 < -1.2) && (y2 > -2.2) && (y2 < -1.2)){
       invmass_pm_muarm->Fill(invmass_PM); //south arm
       if(pT1>1 && pT2>1)invmass_pm_muarm_ptcut->Fill(invmass_PM); //south arm with min pT cut
     }
     
   }
 }

 //cout << endl;
  return EVENT_OK;
  
}


bool AnaPHPyBBar::both_muon_in_muon_arm(TMCParticle* part1, TMCParticle* part2)
{
  double E1 = part1->GetEnergy();
  double pz1 = part1->GetPz();
  double rapidity1 = 0.5*std::log((E1+pz1)/(E1-pz1));
  
  double E2 = part2->GetEnergy();
  double pz2 = part2->GetPz();
  double rapidity2 = 0.5*std::log((E2+pz2)/(E2-pz2));

 //if( _verbosity) cout << "Rapidity1 : " << rapidity1 << " Rapidity2: " << rapidity2 << endl;

  if( ((rapidity1>1.2 && rapidity1<2.2) && (rapidity2>1.2 && rapidity2<2.2)) ||
    ((rapidity1>-2.2 && rapidity1<-1.2) && (rapidity2>-2.2 && rapidity2<-1.2)) )  return true;
 
  //test back-to-back
  //if( (fabs(rapidity1)>1.2 && fabs(rapidity1)<2.2) &&
    //(fabs(rapidity2)>1.2 && fabs(rapidity2)<2.2) )  return true;

   return false;
}


  
      
