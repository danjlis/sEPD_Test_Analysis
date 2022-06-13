
// Functions for the Line Test Analysis
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>

std::string MakeRootFile_Cosmics(std::string inputFileName, const std::string datadir, const std::string savedir, int ttt = 32, bool debug = false){

 if (debug) cout << "RUNNING: make_sEPD_rootfile_v1 for " << inputFileName << " input file" << endl;
  if (debug) {
    cout<<"Printing Parameters: "<<endl;
    cout<<"datadir: "<<datadir<<endl;
    cout<<"savedir: "<<savedir<<endl;
    cout<<"ttt: "<<ttt<<endl;
  }
  std::string path = datadir + "/" + inputFileName;
  std::string rootname = inputFileName;
  std::string delimiter = ".";
  size_t pos = 0;
  std::string token;
  while ((pos = rootname.find(delimiter)) != std::string::npos) {
      rootname = rootname.substr(0, pos);
  }
  std::string fname = rootname;
  rootname = savedir + rootname + ".root";
  ifstream infile(path);

  vector<Int_t> device;
  vector<Int_t> ch;
  vector<Int_t> tile, trial;
  vector<Float_t> vmin;

  std::string last_part;
  if (debug) {
    cout<<"Printing paths: "<<endl;
    cout<<"datapath: "<<path<<endl;
    cout<<"savedir: "<<rootname<<endl;
  }
  /////////////////////////////////////
  // DEFINE TREE AND OUTPUT ROOT FILE
  TFile* fout = new TFile(rootname.c_str(), "recreate");
  TTree* tr = new TTree("sEPDTree", "");
  tr->Branch("device", &device);
  tr->Branch("channel", &ch);
  tr->Branch("trial", &trial);
  tr->Branch("tile", &tile);
  tr->Branch("vmin", &imon);


  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;

  while (infile){
    string s;
    if (!getline( infile, s )) break;
    if (s.empty()) continue;
    if (!isdigit(s.at(0))) continue;

    istringstream ss( s );
    vector<string> vec_str;

    while (ss)
    {
      string stemp;
      if (!getline( ss, stemp, ',' )) break;
      vec_str.push_back( stemp );
    }

    if(vec_str.size()==5){
      device.push_back(stoi(vec_str[0]));
      ch.push_back(stoi(vec_str[1]));
      tile.push_back(stoi(vec_str[2]));
      trial.push_back(stoi(vec_str[3]));
      vmin.push_back(stof(vec_str[4]));
      nLines++;
      if(tile.size() == ttt){
        if (debug){
        //  cout<<"sample Imon: "<<vec_str[6]<<endl;
        }
        tr->Fill();
        device.clear();
        ch.clear();
        tile.clear();
        trial.clear();
        vmin.clear();
      }
    }
  }

  if (debug) cout << "total numer of lines analyzed = " << nLines << endl;
  fout->cd();
  tr->Write("", TObject::kOverwrite);
  //delete tr;
  fout->Close();
  if (debug) cout << "DONE: " <<  rootname << " has been created" << endl;

  return fname;
}
