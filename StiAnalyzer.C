#define StiAnalyzer_cxx
#include "StiAnalyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void StiAnalyzer::Loop()
{
//   In a ROOT session, you can do:
//      root> .L StiAnalyzer.C
//      root> StiAnalyzer t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   
   // If there's a chain and entires, create the hists and canvii.
   BookHistograms();
   BookCanvas();

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      errorAcc->Fill(AcceptedHit_errorMag);
      errorRej->Fill(RejectedHit_errorMag);
      errorAny->Fill(AnyHit_errorMag);
   }

   makeErrorSumTest();
}

void StiAnalyzer::makeErrorSumTest()
{
   errorSumTest->cd();
   errorAny->SetLineColor(kBlack);
   errorAcc->SetLineColor(kBlue);
   errorRej->SetLineColor(kRed);
   TH1F* sum = (TH1F*)errorAcc->Clone();
   sum->Add(errorRej);
   sum->SetLineColor(kMagenta);
   sum->SetMarkerColor(kMagenta);
   sum->SetMarkerStyle(20);
   errorAny->Draw();
   errorAcc->Draw("same");
   errorRej->Draw("same");
   sum->Draw("same P");
   TLegend* leg = new TLegend(.55,.55,.87,.85);
   leg->AddEntry(errorAny,"Any Hit","l");
   leg->AddEntry(errorAcc,"Accept Hit","l");
   leg->AddEntry(errorRej,"Reject Hit","l");
   leg->AddEntry(sum,"Accept+Reject","p");
   leg->Draw("same");
}

void StiAnalyzer::BookCanvas()
{
  errorSumTest = new TCanvas("errorSumTest","Error Sum Test",0,0,1050,1050);
}
 
void StiAnalyzer::BookHistograms()
{
  errorAcc = new TH1F("errorAcc","errorAcc",200,0,2);
  errorRej = new TH1F("errorRej","errorRej",200,0,2);
  errorAny = new TH1F("errorAny","Projection Errors;Error Magnitude(#sigma);Counts",200,0,2);
}