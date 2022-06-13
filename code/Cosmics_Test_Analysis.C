#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
//#include "draw_scan.C"
#include "dlUtility.h"
#include "sEPD_functions.C"
#include "sEPD_Line_Functions.C"
#include "sEPD_Cosmics_Functions.C"

void Analyze(std::vector<std::string> &filenames, const int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, int ch1, int ch2, bool debug = false){
  // How many files are we looking at?
  int size = filenames.size();
  std::vector<int> channels;

  // sector add on for the directories and files
  char *sector_addon = new char[10];
  if (sector == 0) sprintf(sector_addon, "STAR EPD");
  else if (sector < 10) sprintf(sector_addon, "sEPD s0%d", sector);
  else sprintf(sector_addon,"sEPD s%d", sector);

  char *sector_addon_n = new char[10];
  if (sector == 0) sprintf(sector_addon_n, "s00");
  else if (sector < 10) sprintf(sector_addon_n, "s0%d", sector);
  else sprintf(sector_addon_n,"s%d", sector);

  if (debug) cout<< "Destination directory for all the stuff: "<<save_dir_raw<<endl;

  TFile *out_hist_file = new TFile(Form("%sCosmics_Test_hists.root", save_dir_root.c_str()), "recreate");

  for (int ff = 0; ff < size; ff++){


    if (debug) cout<<"In Directory: "<<filenames.at(ff)<<endl;

    if (debug) cout<<"Cosmics Test "<<ff<<"...."<<endl;
    // Make the root file and save it to save_dir_raw
    GetChannels(filenames.at(ff), channels, debug);

    // Get the channels that were run over in the test
    int ch1 = channels.at(0);

    char *tile_addon = new char[10];
    printf(tile_addon, "%d", ch1);



    std::string fname;
    // Make the root file for the txt file that is being read in,
    fname = MakeRootFile_Cosmics(filenames.at(ff), data_dir, save_dir_raw, n_channels, debug);
    if (debug){
      cout<<"Made Root File"<<endl;
    }

    char *root_path = new char[100];
    sprintf(root_path, "%s%s.root", save_dir_raw.c_str(), fname.c_str());
    TFile* fin = new TFile(root_path, "read");
    if(!fin){
      cout<<"No File Found here"<<endl;
      continue;
    }

    // Getting Tree and brnaches from this file
    TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
    if(!inTree_p){
      cout<<"No TTree found here..."<<endl;
      continue;
    }
    vector<Int_t>* ch=nullptr;
    vector<Int_t>* tile=nullptr;
    vector<Int_t>* trial=nullptr;
    vector<Int_t>* device=nullptr;
    vector<Float_t>* vmin=nullptr;

    inTree_p->SetBranchAddress("device", &device);
    inTree_p->SetBranchAddress("trial", &trial);
    inTree_p->SetBranchAddress("channel", &ch);
    inTree_p->SetBranchAddress("tile", &tile);
    inTree_p->SetBranchAddress("vmin", &vmin);

    TH1D *h_cosmic_dist[31];
    for (int i = 0; i < 31;i++){
      h_cosmic_dist = new TH1D(Form("h_cosmic_dist_%s_tile%d", sector_addon_n, i), ";Signal [V];", 100, 0, 2);
    }
    int size_of_tree = intTree_p->GetEntries();

    for (int i = 0; i < size_of_tree; i++){
      inTree_p->GetEntry(i);
      for (int j = 0; j < tile->size();j++){
        h_cosmic_dist[tile->at(j)-1]->Fill(vmin->at(j));
      }
    }

    float xPos = 0.65;
    float yPos = 0.86;
    float yPos2 = 0.2;
    float dy = 0.04;
    float dy2 = 0.05;
    float fontType = 43;
    float fontSize = 13;

    TCanvas *cc = new TCanvas("cc","cc");
    SetyjPadStyle();
    for (int i = 0; i < 31; i++){
      for (int j = 0 ; j < channels.size();j++){
        if (i+1 == channels.at(j)) {
          h_cosmic_dist[i]->SetLineColor(kBlue+2);
          h_cosmic_dist[i]->SetLineWidth(2);
          h_cosmic_dist[i]->SetFillColorAlpha(kBlue - 2, 0.3);
          h_cosmic_dist[i]->Draw("hist");

          drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
          drawText(Form("%s Cosmics Test - Tile %d", sector_addon, i+ 1),xPos,yPos-dy2,0, 1, fontSize, fontType);
          cc->SaveAs(Form("%s%s_cosmic_dist_tile_%d.png", save_dir_plot.c_str(), sector_addon_n, i+1));
          cc->SaveAs(Form("%s%s_cosmic_dist_tile_%d.pdf", save_dir_plot.c_str(), sector_addon_n, i+1));
        }
      }
    }


    for (int i = 0; i < 31; i++){
      for (int j = 0 ; j < channels.size();j++){
        if (i+1 == channels.at(j)) h_cosmic_dist[i]->Write();
      }
    }
  }

  return;
}

int Line_Test_Analysis(const std::string &config_file= "cosmics_config.config")
{
  bool debug = false;
  if (debug) cout<< "In the code... Getting config file"<<endl;
// Input configuration file
  TEnv *config_p = new TEnv(config_file.c_str());
  const int sector = config_p->GetValue("SECTOR", 0);
  const int all_runs = config_p->GetValue("ALLRUNS", 1);
  const int ch1 = config_p->GetValue("CHANNEL1", 0);
  const int ch2 = config_p->GetValue("CHANNEL2", 0);

  if (debug) cout<<"Setting sector header for sector "<<sector<<endl;
  char *sec_head = new char[10];
  if (sector < 10) sprintf(sec_head, "s0%d", sector);
  else sprintf(sec_head,"s%d", sector);

  const std::string data_dir = "../data/" + std::string(sec_head);
  const std::string save_dir_root = "../Results/"+std::string(sec_head)+"/root_hist/";
  const std::string save_dir_plot = "../Results/"+std::string(sec_head)+"/plots/";
  const std::string save_dir_raw = "../Results/"+std::string(sec_head)+"/root_raw/";

  std::vector<std::string> filenames;
  std::string test_type = "cosmics";

  if (debug) cout<<"Getting File names for test: "<<test_type<<endl;

  GetFileName(filenames, test_type, sector, all_runs, ch1, ch_2, debug);

  if (debug) cout<<"Got File names, going to analyze..."<<endl;
  if (debug) {
    for ( std::vector<std::string>::iterator it = filenames.begin(); it != filenames.end();++it ){
      cout<<*it<<endl;
    }
  }

  // Now we have out file names

  // analyze the data
  Analyze(filenames, sector, data_dir, save_dir_raw, save_dir_plot, save_dir_root, ch1, ch2, debug);

  return 1;
}
