
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>
#include "yjUtility.h"
void Analyze(TMultiGraph* gg, int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, bool debug = true){
  char *sec_head = new char[10];
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  TFile *in_Full = new TFile(Form("%shealth_hist.root", save_dir_root.c_str()), "read");
  if (!in_Full){
    cout<<"No file for this here sector"<<endl;
    return;
  }
  TGraph *g_compareheights = (TGraph*) in_Full->Get(Form("%s_compare_heights", sec_head));
  if (!g_compareheights){
    cout<<"No file for this here sector"<<endl;
    return;
  }
  int colors16[16] = {kBlack, kRed+1, kBlue+1, kGreen-3, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};

  g_compareheights->SetMarkerSize(2);
  if (sector > 15) sector = sector - 16;
  g_compareheights->SetMarkerColor(colors16[sector]);
  g_compareheights->SetMarkerStyle(8);
  gg->Add(g_compareheights);

  return;
}

void Compare_Heights_Tiles(){
  TMultiGraph* gg = new TMultiGraph();
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  for (int s = 1; s < 31; s++){
    char *sec_head = new char[10];
    if (s < 10) sprintf(sec_head, "s0%d", s);
    else sprintf(sec_head,"s%d", s);

    const std::string data_dir = "../data/" + std::string(sec_head);
    const std::string save_dir_root = "../Results/"+std::string(sec_head)+"/root_hist/";
    const std::string save_dir_plot = "../Results/"+std::string(sec_head)+"/plots/";
    const std::string save_dir_raw = "../Results/"+std::string(sec_head)+"/root_raw/";

    // Now we have out file names
    bool debug = false;
    // analyze the data
    Analyze(gg, s, data_dir, save_dir_raw, save_dir_plot, save_dir_root, debug);
  }
  float xPos = 0.18;
  //float xPos = 0.85;
  float yPos = 0.82;
  float yPos2 = 0.2;
  float dy = 0.04;
  float dy2 = 0.05;
  float fontType = 43;
  float fontSize = 13;
  double max_y = 1.2;
  TCanvas *c = new TCanvas("c","", 1000, 600);
  gPad->SetGridy();
  gPad->SetGridx();
  double xs = 1; // grid step along X
  double ys = 1; // grid step along Y
  double xmin = 0,xmax = 32;
  auto aline = new TLine();

  gg->GetXaxis()->SetTitle("Tile");
  gg->GetYaxis()->SetTitle("I_{<signal>} - I_{<dark>}");
  gg->Draw("AP");
  for (double xg = xmin+xs; xg < xmax; xg=xg+xs) aline->PaintLine(xg,0,xg,max_y);

  drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("sEPD Tile Maximum Currents",xPos,yPos-dy2,0, 1, fontSize, fontType);

return;
}
