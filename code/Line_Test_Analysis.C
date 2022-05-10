//Line Test Analysis
//Author: Daniel Lis
//Brief: Runs through the raw text file and makes root files and histograms relevant to the data.


// This is the analysis for the Line Test
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
//#include "draw_scan.C"
#include "yjUtility.h"
#include "sEPD_functions.C"
#include "sEPD_Line_Functions.C"
// Line Scan Analysis code

void Analyze(std::vector<std::string> &filenames, const int sector, const std::string data_dir, const std::string save_dir_raw, const std::string save_dir_plot, const std::string save_dir_root, int ch1, int ch2, bool debug = true){
  SetyjPadStyle();

  // boolean for choosing the coarse of action for the dark current calculation
  bool doContDark = true;
  bool doTempReadout = false;
  bool plotTempDark = false;
  gStyle->SetOptStat(0);

  // How many files are we looking at?
  int size = filenames.size();
  std::vector<int> channels;

  // sector add on for the directories and files
  char *sector_addon = new char[5];
  if (sector < 10) sprintf(sector_addon, "s0%d", sector);
  else sprintf(sector_addon,"s%d", sector);
  if (debug) cout<< "Destination directory for all the stuff: "<<save_dir_raw<<endl;

  //Making the crosstalk histogram for the sector
  TH2D *h2D_crosstalk = new TH2D("h2D_crosstalk","",31, 0.5, 31.5, 31, 0.5, 31.5);
  TH1D *h_bin_response[32];
  TH1D *hp_ratio = new TH1D("h_ratio","",15, 0.5, 15.5);

  double max_x[32];
  double bag[32];
  double end[32];
  TFile *out_hist_file = new TFile(Form("%sLine_Test_hists.root", save_dir_root.c_str()), "recreate");

  // Beginning the file loop....
  for (int ff = 0; ff < size; ff++){
    channels.clear();
    if (debug) cout<<"In Directory: "<<filenames.at(ff)<<endl;

    if (debug) cout<<"Line Scan "<<ff<<"...."<<endl;
    // Make the root file and save it to save_dir_raw
    GetChannels(filenames.at(ff), channels, debug);
    int n_channels = channels.size();
    int ch1 = channels.at(0);
    int ch2 = channels.at(n_channels - 1);
    if (true) {
      cout<<"Channels scanned: ";
      for (int j = 0; j < n_channels; j++){
        cout<<channels.at(j)<<" ";
      }
      cout<< ". "<<endl;
    }
    std::string fname;
    fname = MakeRootFile_Line(filenames.at(ff), data_dir, save_dir_raw, n_channels, debug);
    if (debug){
      cout<<"Made Root File"<<endl;
    }

    // Reading in the root file that was just made:

    char *root_path = new char[100];
    sprintf(root_path, "%s%s.root", save_dir_raw.c_str(), fname.c_str());
    TFile* fin = new TFile(root_path, "read");
    if(!fin){
      cout<<"No File Found here"<<endl;
      continue;
    }
    TTree* inTree_p = (TTree*) fin ->Get("sEPDTree");
    if(!inTree_p){
      cout<<"No TTree found here..."<<endl;
      continue;
    }
    Float_t xpos, ypos;
    vector<Int_t>* ch=nullptr;
    vector<Int_t>* tile=nullptr;
    vector<Int_t>* trial=nullptr;
    vector<Int_t>* device=nullptr;
    vector<Float_t>* imon=nullptr;
    vector<Float_t>* rmon=nullptr;
    vector<Float_t>* vcomp=nullptr;
    vector<Float_t>* temp_b=nullptr;
    vector<Float_t>* temp_c=nullptr;

    inTree_p->SetBranchAddress("xpos", &xpos);
    inTree_p->SetBranchAddress("ypos", &ypos);
    inTree_p->SetBranchAddress("device", &device);
    inTree_p->SetBranchAddress("trial", &trial);
    inTree_p->SetBranchAddress("channel", &ch);
    inTree_p->SetBranchAddress("tile", &tile);
    inTree_p->SetBranchAddress("imon", &imon);
    inTree_p->SetBranchAddress("rmon", &rmon);
    inTree_p->SetBranchAddress("vcomp", &vcomp);
    inTree_p->SetBranchAddress("temp_b", &temp_b);
    inTree_p->SetBranchAddress("temp_c", &temp_c);

    vector<string> params, paramNames;
    double xfinalcm;
    double xorigincm;
    double xstepcm;
    double yfinalcm;
    double yorigincm;
    double ystepcm;
    int nsteps;
    double nRep;
    std::string data_path = "/" + fname + ".txt";
    if (debug) cout<< "Getting Parameters from file..."<<data_path<<endl;
    GetParameters_Line(paramNames, params, data_path, data_dir, debug);
    if (debug) {
      cout << "Done getting info: "<<params.size()<<endl;
      for ( int i = 0; i < params.size(); i++){
        cout<<paramNames[i]<<" : "<<params[i]<<endl;
      }
    }
    for (int i = 0; i < paramNames.size(); i++){
      if (paramNames[i] == "xorigincm" || paramNames[i] == " xorigincm") xorigincm = stof(params[i]);
      if (paramNames[i] == " xfinalcm" || paramNames[i] == "xfinalcm") xfinalcm = stof(params[i]);
      if (paramNames[i] == " xsteplengthcm" || paramNames[i] == "xsteplengthcm") xstepcm = stof(params[i]);
      if (paramNames[i] == "yorigincm" || paramNames[i] == " yorigincm") yorigincm = stof(params[i]);
      if (paramNames[i] == " yfinalcm" || paramNames[i] == "yfinalcm") yfinalcm = stof(params[i]);
      if (paramNames[i] == " ysteplengthcm" || paramNames[i] == "ysteplengthcm") ystepcm = stof(params[i]);
      if (paramNames[i] == " nRepeat" || paramNames[i] == "nRepeat") nRep = stof(params[i]);

    }
    // Looking for the vertical or horizontal line scans
    double origincm;
    double finalcm;
    double stepcm;
    int x_or_y;
    if (xorigincm == xfinalcm){
      x_or_y = 1;
      origincm = yorigincm;
      finalcm = yfinalcm;
      stepcm = ystepcm;
    }
    else {
      x_or_y = 0;
      origincm = xorigincm;
      finalcm = xfinalcm;
      stepcm = xstepcm;
    }

    nsteps = floor((finalcm - origincm)/stepcm);
    if (debug){
      cout<<"Parameters: "<<endl;
      cout<< "xorigincm : "<<xorigincm<<endl;
      cout<< "yorigincm : "<<yorigincm<<endl;
      cout<< "xfinalcm : "<<xfinalcm<<endl;
      cout<< "yfinalcm : "<<yfinalcm<<endl;
      cout<< "xsteplengthcm : "<<xstepcm<<endl;
      cout<< "ysteplengthcm : "<<ystepcm<<endl;
      cout<< "nsteps : "<<nsteps<<endl;
      cout<< "nRep : "<<nRep<<endl;
    }
    const int NTRIALS = 5; //static_cast<int>(nRep);
    const int NTILE = 32;

    // Making arrays and all the different histograms
    TH1D *h1_tile_dc[NTILE];
    TH1D *h1_tile_dc2[NTILE];
    TH1D *h1_temp_scan[NTILE];
    TH1D *h1_RMS_scan[NTILE];
    TH1D *h1_RMS_on[NTILE];
    TH1D *h1_RMS_off[NTILE];
    TH1D *h1_RMS_on2[NTILE];
    TH1D *h1_RMS_off2[NTILE];
    int n_bin_tile[NTILE];
    vector<std::array<float, 32>> dark_current;
    vector<std::array<float, 32>> temp_b_dark;
    vector<std::array<float, 32>> temp_c_dark;
    float dc_temp[NTRIALS] = {};
    float tb_temp[NTRIALS] = {};
    float tc_temp[NTRIALS] = {};
    for ( int i = 0; i < NTILE; i++ ){

        h1_tile_dc[i] = new TH1D(Form("h1_tile_dc_%d_%d_ch%d",ch1, ch2, i), "",10, 0.1, 0.56);
        h1_temp_scan[i] = new TH1D(Form("h1_temp_scan_%d_%d_ch%d",ch1, ch2, i), "",100, 0.0, 1.0);
        h1_RMS_scan[i] = new TH1D(Form("h1_RMS_scan_%d_%d_ch%d",ch1, ch2, i), ";RMS [#mu A];",100, 0.0, 0.025);
        h1_RMS_on[i] = new TH1D(Form("h1_RMS_on_%d_%d_ch%d",ch1, ch2, i), "",100, 0.0, 0.025);
        h1_RMS_off[i] = new TH1D(Form("h1_RMS_off_%d_%d_ch%d", ch1, ch2, i), "",100, 0.0, 0.025);
        h1_tile_dc2[i] = new TH1D(Form("h1_tile_dc2_%d_%d_ch%d",ch1, ch2, i), "",10, 0.1, 0.6);
        h1_RMS_on2[i] = new TH1D(Form("h1_RMS_on2_%d_%d_ch%d",ch1, ch2, i), "",100, 0.0, 0.025);
        h1_RMS_off2[i] = new TH1D(Form("h1_RMS_off2_%d_%d_ch%d",ch1, ch2, i), "",100, 0.0, 0.025);
    }
    TH1D *h1_bin_response_temp[NTILE];
    TProfile *h1_tile_response[NTILE];
    TProfile *h1_tile_response_norm[NTILE];
    TH2D *h2_tile_rmon_imon[NTILE];
    TProfile *h1_tile_rmon[NTILE];
    TH1D *h1_tile_rmon_1[NTILE];
    TGraph *h1_all_rmon[NTILE];
    TGraph *h1_all_imon[NTILE];
    for (int i = 0; i < NTILE; i++){
      h1_bin_response_temp[i] = new TH1D(Form("h1_bin_response_temp_%d_%d_ch%d",ch1, ch2, i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h1_tile_response[i] = new TProfile(Form("h1_tile_response_%d_%d_ch%d",ch1, ch2, i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h1_tile_response_norm[i] = new TProfile(Form("h1_tile_response_norm_%d_%d_ch%d",ch1, ch2, i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h2_tile_rmon_imon[i] = new TH2D(Form("h2_tile_rmon_imon_%d_%d_ch%d",ch1, ch2, i), "", 50, 55, 61, 60, 0.0, 1.2);
      h1_tile_rmon[i] = new TProfile(Form("h1_tile_rmon_%d_%d_ch%d",ch1, ch2, i), "", nsteps+1, origincm-stepcm/2, finalcm+stepcm/2);
      h1_tile_rmon_1[i] = new TH1D(Form("h1_tile_rmon_1_%d_%d_ch%d",ch1, ch2, i), "", 100, 47, 70);
      h1_all_rmon[i] = new TGraph();
      h1_all_imon[i] = new TGraph();
    }

    double mean_dc[NTILE];
    double rms_dc[NTILE];
    double mean_dc2[NTILE];
    double rms_dc2[NTILE];
    int nScanCount[NTILE];
    double slope_dc[NTILE];

    //number of steps per line,
    int nEntries = inTree_p->GetEntries();
    double last_pos[2] = {0, 0};
    double pos[2];
    double max_y = 0.4;

    //Dark Current and temperature:
    std::array<float, NTILE> arr_dc = {0};

    std::array<float, NTILE> arr_tempb = {0};
    std::array<float, NTILE> arr_tempc = {0};
    if (doContDark){
      for ( int i = 0; i < nEntries; i++ ){
        inTree_p->GetEntry(i);
        pos[0] = xpos;
        pos[1] = ypos;

        if (last_pos[x_or_y] != pos[x_or_y]){
          if (last_pos[0] == 0){
            dark_current.push_back(arr_dc);
            if (doTempReadout)  temp_b_dark.push_back(arr_tempb);
            if (doTempReadout)temp_c_dark.push_back(arr_tempc);
            for (int l = 0; l < NTILE; l++){
              cout<<last_pos[0]<<", "<<last_pos[1]<<": Tile "<< l << " : "<<arr_dc[l]<<endl;
              arr_dc[l] = 0;
              if (doTempReadout)arr_tempb[l] = 0;
              if (doTempReadout)arr_tempc[l] = 0;
            }
          }
          last_pos[0] = pos[0];
          last_pos[1] = pos[1];
        }
        for(int it = 0; it<tile->size(); it++){
          Float_t imonTemp = imon->at(it);
          Float_t tbTemp;
          if (doTempReadout) tbTemp = temp_b->at(it);
          Float_t tcTemp;
          if (doTempReadout) tcTemp = temp_c->at(it);

          if(xpos==0 && ypos==0){
            arr_dc[tile->at(it)] += imonTemp/static_cast<double>(NTRIALS);
            if (doTempReadout)arr_tempb[tile->at(it)] += tbTemp/static_cast<double>(NTRIALS);
            if (doTempReadout)arr_tempc[tile->at(it)] += tcTemp/static_cast<double>(NTRIALS);
          }
        }
      }
    }

    for ( int i = 0; i < nEntries; i++ ){

      inTree_p->GetEntry(i);
      //calculate dark current

      for(int it = 0; it<tile->size(); it++){
        Float_t imonTemp = imon->at(it);

        pos[0] = xpos;
        pos[1] = ypos;


        if(xpos==0 && ypos==0 && i < 500){//for dark current
          h1_tile_dc[tile->at(it)]->Fill(imonTemp);
        }
        else if(xpos==0 && ypos==0){//for dark current
          h1_tile_dc2[tile->at(it)]->Fill(imonTemp);
        }
        else{
          if (i == nRep) last_pos[x_or_y] = pos[x_or_y];
          if (last_pos[x_or_y] == pos[x_or_y]){
            h1_temp_scan[tile->at(it)]->Fill(imonTemp);
          }
          else {
            //cout<<"rms: "<<h1_temp_scan[tile->at(it)]->GetRMS()<<endl;
            h1_RMS_scan[tile->at(it)]->Fill(h1_temp_scan[tile->at(it)]->GetRMS());
            h1_temp_scan[tile->at(it)]->Reset();
          }
        }
        if (!(last_pos[x_or_y] == pos[x_or_y])) last_pos[x_or_y] = pos[x_or_y];
      }
    }

    /////////////////////////////////////
    // Calculate Dark Current

    if (debug) printf("DarkCurrent Values: \n");
    if (debug) printf(" --------------------------------------------- \n");
    double ymid = 24;
    for(int it = 0; it < NTILE; it++){
        mean_dc[it] = h1_tile_dc[it]->GetMean();
        rms_dc[it] = h1_tile_dc[it]->GetRMS();

      if (debug) printf("Tile %d: %f +/- %f -> %f +/- %f = %f\n", it, mean_dc[it], rms_dc[it], mean_dc2[it], rms_dc2[it], slope_dc[it]);
      nScanCount[it] = 0;
    }

    if (debug) printf("Dark current data: \n");
    int size_dc_data=dark_current.size();
    if (debug) cout<<"Size of dark current data: "<<size_dc_data<<endl;


    int zcount = 0;
    int bb =0;
    double totals = 0.;
    vector<double> vec_xpos = {};
    vector<double> vec_ypos = {};

    vec_xpos.push_back(last_pos[0]);
    vec_ypos.push_back(last_pos[1]);

    last_pos[0] = 0;
    last_pos[1] = 0;

    for (int l = 0; l < NTILE; l++){
      arr_dc[l] = 0;
      arr_tempb[l] = 0;
      arr_tempc[l] = 0;
    }
    for (int i = 0; i < nEntries; i++){
      inTree_p->GetEntry(i);
      pos[0] = xpos;
      pos[1] = ypos;

      if (last_pos[x_or_y] != pos[x_or_y]){
        if (pos[0] == 0){
          for (int l = 0; l < NTILE; l++){
            arr_dc[l] = 0;
            arr_tempb[l] = 0;
            arr_tempc[l] = 0;
          }
        }
        last_pos[0] = pos[0];
        last_pos[1] = pos[1];
      }

      for(int it = 0; it<tile->size(); it++){
        double imonTemp = imon->at(it);
        double rmonTemp = rmon->at(it);
        Float_t tbTemp;
        if (doTempReadout) tbTemp = temp_b->at(it);
        Float_t tcTemp;
        if (doTempReadout) tcTemp = temp_c->at(it);

        if(xpos==0 && ypos==0){
          arr_dc[tile->at(it)] += imonTemp/static_cast<double>(NTRIALS);
          if (doTempReadout)arr_tempb[tile->at(it)] += tbTemp/static_cast<double>(NTRIALS);
          if (doTempReadout)arr_tempc[tile->at(it)] += tcTemp/static_cast<double>(NTRIALS);
        }

        h1_all_rmon[tile->at(it)]->Set(h1_all_rmon[tile->at(it)]->GetN()+1);
        h1_all_rmon[tile->at(it)]->SetPoint(h1_all_rmon[tile->at(it)]->GetN()-1, h1_all_rmon[tile->at(it)]->GetN()-1, rmonTemp*42);
        h1_tile_rmon_1[tile->at(it)]->Fill(rmonTemp*42);// = new TProfile(Form("h1_tile_rmon_%d", i), "", nsteps+1, xorigincm-xstepcm/2, xfinalcm+xstepcm/2);
        h2_tile_rmon_imon[tile->at(it)]->Fill(rmonTemp*42, imonTemp);
        if(!(pos[0]==0 && pos[1]==0)){//for dark current
          if (doContDark){
            h1_tile_response_norm[tile->at(it)]->Fill(pos[x_or_y], imonTemp - (arr_dc[tile->at(it)]));
            h1_tile_response[tile->at(it)]->Fill(pos[x_or_y], imonTemp - (arr_dc[tile->at(it)]));
          }
          else {
            h1_tile_response_norm[tile->at(it)]->Fill(pos[x_or_y], imonTemp - (mean_dc[tile->at(it)]));
            h1_tile_response[tile->at(it)]->Fill(pos[x_or_y], imonTemp - (mean_dc[tile->at(it)]));
          }
          h1_tile_rmon[tile->at(it)]->Fill(pos[x_or_y], rmonTemp*42);
          h1_all_imon[tile->at(it)]->Set(h1_all_imon[tile->at(it)]->GetN()+1);
          h1_all_imon[tile->at(it)]->SetPoint(h1_all_imon[tile->at(it)]->GetN()-1, h1_all_imon[tile->at(it)]->GetN()-1, imonTemp);
            //std::cout<<pos[0]<<", "<<pos[1]<<": "<< tile->at(it)<<" = "<<imonTemp<<endl;
        }
      }//tile
      if (last_pos[0] != pos[0] || last_pos[1] != pos[1]){
        last_pos[0] = pos[0];
        last_pos[1] = pos[1];
        vec_xpos.push_back(last_pos[0]);
        vec_ypos.push_back(last_pos[1]);

      }
    }

    for (int i = 0; i < NTILE; i++){
      double s = h1_tile_response[i]->GetBinContent(h1_tile_response[i]->GetMaximumBin());
      h1_tile_response_norm[i]->Scale(1./s);
    }


    TH1D *h_blank = new TH1D("h_blank", ";x [cm]; <I>_{sig} - <I>_{dark}", nsteps, 4, 88);
    h_blank->SetMaximum(max_y);
    h_blank->SetMinimum(-0.1);
    int colors16[16] = {kBlack, kRed+1, kBlue+1, kGreen-3, kCyan+1, kOrange -3, kMagenta -3, kGray, kRed -1, kBlue -1, kGreen -7, kCyan - 3, kOrange+1, kMagenta -5, kViolet+6, kAzure +3};
    float xPos = 0.2;
    //float xPos = 0.85;
    float yPos = 0.86;
    float yPos2 = 0.2;
    float dy = 0.04;
    float dy2 = 0.05;
    float fontType = 43;
    float fontSize = 13;
    TCanvas *c_full = new TCanvas("c_full", "");
    int oo = 0;
    max_y = 0.5;
    double min_y = -0.1;
    int ch_1  = channels.at(0);
    int ch_2 = channels.at(n_channels - 1);
    for (int i = 0; i < NTILE; i++){
      int ns = 0;
      for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
        if (i == *it) ns++;
      }
      if (ns == 0) continue;
      if (oo == 0){
        h1_tile_response[i]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
        h1_tile_response[i]->SetLineColor(colors16[0]);
        h1_tile_response[i]->SetLineWidth(2);
        h1_tile_response[i]->SetMaximum(max_y);
        h1_tile_response[i]->SetMinimum(min_y);
        h1_tile_response[i]->Draw("hist");
      }
      else{
        h1_tile_response[i]->SetLineColor(colors16[oo]);
        h1_tile_response[i]->SetLineWidth(2);
        h1_tile_response[i]->Draw("hist same");
      }
      oo++;
    }
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test #bf{NOT NORMALIZED}", sector_addon),xPos,yPos-dy2,0, 1, fontSize, fontType);
    if (oo > 2){
      if (ch_1 == 30 || ch_2 == 30) drawText("Tile 1 and Even Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      else if (ch_1 == 31 || ch_2 == 31) drawText("Tile 1 and Odd Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    }
    else drawText(Form("Tiles %d to %d",ch_2, ch_1),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    //drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);
    out_hist_file->cd();

    if (oo > 2){
      if (ch_1 == 30 || ch_2 == 30) c_full->SetName("c_full_even");
      else if (ch_1 == 31 || ch_2 == 31) c_full->SetName("c_full_odd");
      c_full->Write();
    }
    c_full->SaveAs(Form("%s%s.png", save_dir_plot.c_str(), fname.c_str()));
    c_full->SaveAs(Form("%s%s.pdf", save_dir_plot.c_str(), fname.c_str()));

/////////////////

    if ((ch_1 == 30 || ch_2 == 30)&& (ch_1 == 1 || ch_2 == 1)) {
  TCanvas *c_four = new TCanvas("c_four", "");
  int k = 0;
  for (int i = 0; i <13 ; i++){
    int h1 = 0;
    int h2 = 0;
    for (int j = 0; j < 4;j++){
      if(j == 0){
        double centerbin = 0;
        int maxbin = h1_tile_response[2*(i+1)+k ]->GetMaximumBin();
        for ( int l = maxbin; l < nsteps; l++){
          if(h1_tile_response[2*(i +1)+ k]->GetBinContent(l) <= h1_tile_response[2*(i+2)+k]->GetBinContent(l)){
            centerbin = l;
            break;
          }
        }
        if (centerbin == 0)centerbin = maxbin + 5;
        double low_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) - 25;
        double high_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) + 25;

        h_blank->GetXaxis()->SetRangeUser(low_x, high_x);
        h_blank->Draw("hist");
        h1_tile_response[2*i + k]->SetLineColor(colors16[0]);
        h1_tile_response[2*i + k]->SetLineWidth(2);
        h1_tile_response[2*i + k]->SetMaximum(max_y);
        h1_tile_response[2*i + k]->Draw("hist same");
        h1 = 2*i+k;
        if(h1 == 0) h1 = 1;
      }
      else{
        h1_tile_response[2*(i+j)+k]->SetLineColor(colors16[j]);
        h1_tile_response[2*(i+j)+k]->SetLineWidth(2);
        h1_tile_response[2*(i+j)+k]->Draw("hist same");
        h2 = 2*(i+j)+k;
      }
    }
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", sector_addon) ,xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Sector %s", sector_addon),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    // drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);
    drawText(Form( "Tiles: %d, %d, %d, %d", h1, h2 - 4, h2 -2, h2),xPos,yPos-4*dy2,0, 1, fontSize, fontType);

    c_four->SaveAs(Form("%s%s_%d_%d.png", save_dir_plot.c_str(), fname.c_str(), h1, h2));
    c_four->SaveAs(Form("%s%s_%d_%d.pdf", save_dir_plot.c_str(), fname.c_str(), h1, h2));
  }

}
    else if ((ch_1 == 31 || ch_2 == 31) && (ch_1 == 1 || ch_2 == 1)) {
  TCanvas *c_four = new TCanvas("c_four", "");
  int k = 1;
  for (int i = 0; i <13 ; i++){
    int h1 = 0;
    int h2 = 0;
    for (int j = 0; j < 4;j++){
      if(j == 0){
        double centerbin = 0;
        int maxbin = h1_tile_response[2*(i+1)+k ]->GetMaximumBin();
        for ( int l = maxbin; l < nsteps; l++){
          if(h1_tile_response[2*(i +1)+ k]->GetBinContent(l) <= h1_tile_response[2*(i+2)+k]->GetBinContent(l)){
            centerbin = l;
            break;
          }
        }
        if (centerbin == 0)centerbin = maxbin + 5;
        double low_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) - 25;
        double high_x = h1_tile_response[2*i+k]->GetBinCenter(centerbin) + 25;

        h_blank->GetXaxis()->SetRangeUser(low_x, high_x);
        h_blank->Draw("hist");
        h1_tile_response[2*i + k]->SetLineColor(colors16[0]);
        h1_tile_response[2*i + k]->SetLineWidth(2);
        h1_tile_response[2*i + k]->SetMaximum(max_y);
        h1_tile_response[2*i+k]->SetMinimum(min_y);

        h1_tile_response[2*i + k]->Draw("hist same");
        h1 = 2*i+k;
        if(h1 == 0) h1 = 1;
      }
      else{
        h1_tile_response[2*(i+j)+k]->SetLineColor(colors16[j]);
        h1_tile_response[2*(i+j)+k]->SetLineWidth(2);
        h1_tile_response[2*(i+j)+k]->Draw("hist same");
        h2 = 2*(i+j)+k;
      }
    }
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test", sector_addon),xPos,yPos-dy2,0, 1, fontSize, fontType);
    drawText(Form("Sector %s", sector_addon),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    // drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);
    drawText(Form( "Tiles: %d, %d, %d, %d", h1, h2 - 4, h2 -2, h2),xPos,yPos-4*dy2,0, 1, fontSize, fontType);

    c_four->SaveAs(Form("%s%s_%d_%d.png", save_dir_plot.c_str(), fname.c_str(), h1, h2));
    c_four->SaveAs(Form("%s%s_%d_%d.pdf", save_dir_plot.c_str(), fname.c_str(), h1, h2));
  }

}

//////////////////////
    TCanvas *c_full_norm = new TCanvas("c_full_norm", "");
    oo = 0;
    max_y = 1.3;
    for (int i = 0; i < NTILE; i++){
      int ns = 0;
      for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
        if (i == *it) ns++;
      }
      if (ns == 0) continue;
      if (oo == 0){
        h1_tile_response_norm[i]->SetTitle(";x [cm]; <I>_{sig} - <I>_{dark}");
        h1_tile_response_norm[i]->SetLineColor(colors16[0]);
        h1_tile_response_norm[i]->SetLineWidth(2);
        h1_tile_response_norm[i]->SetMaximum(max_y);
        h1_tile_response_norm[i]->SetMinimum(min_y);

        h1_tile_response_norm[i]->Draw("hist");
      }
      else{
        h1_tile_response_norm[i]->SetLineColor(colors16[oo]);
        h1_tile_response_norm[i]->SetLineWidth(2);
        h1_tile_response_norm[i]->Draw("hist same");
      }
      oo++;
    }
    drawText("#bf{sPHENIX} #it{Internal}",xPos,yPos,0, 1, fontSize+2, fontType);
    drawText(Form("sEPD %s Crosstalk Test #bf{NORMALIZED}", sector_addon),xPos,yPos-dy2,0, 1, fontSize, fontType);
    if (oo > 2){
      if (ch_1 == 30 || ch_2 == 30) drawText("Tile 1 and Even Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
      else if (ch_1 == 31 || ch_2 == 31) drawText("Tile 1 and Odd Tiles",xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    }
    else drawText(Form("Tiles %d to %d",ch_2, ch_1),xPos,yPos-2*dy2,0, 1, fontSize, fontType);
    //drawText("SiPM Switched",xPos,yPos-3*dy2,0, 1, fontSize, fontType);
    out_hist_file->cd();

    if (oo > 2){
      if (ch_1 == 30 || ch_2 == 30) c_full->SetName("c_full_even_norm");
      else if (ch_1 == 31 || ch_2 == 31) c_full->SetName("c_full_odd_norm");
      c_full_norm->Write();
    }
    c_full_norm->SaveAs(Form("%s%s_norm.png", save_dir_plot.c_str(), fname.c_str()));
    c_full_norm->SaveAs(Form("%s%s_norm.pdf", save_dir_plot.c_str(), fname.c_str()));

    if (x_or_y == 1){

      int b = static_cast<int>(floor(static_cast<double>(ch_1)/2.));
      double *rats_rats_rats;
      rats_rats_rats = GetRatio(h1_tile_response, ch_1, ch_2);

      cout<<ch_1<<" + "<<ch_2<<" ratio at "<<b<<": "<<*(rats_rats_rats)<<" +/- "<<*(rats_rats_rats+1)<<endl;

      double gain_comp = *(rats_rats_rats)/SiPM_Ratio[ch_1 - ch_1%2];
      hp_ratio->SetBinContent(floor(ch_1/2), gain_comp);

      hp_ratio->SetBinError(floor(ch_1/2), *(rats_rats_rats+1));

    }
    FillCrossTalk(h2D_crosstalk, h1_tile_response_norm, ch_1, ch_2);
    int write = GetCrosstalkDistance(h1_bin_response_temp, max_x,bag,end, h1_tile_response_norm, ch_1, ch_2);
    for (int i = 0; i < NTILE; i++){
      int ns = 0;
      int oo = 0;
      for (std::vector<int>::iterator it = channels.begin(); it != channels.end(); ++it){
        if (i == *it) ns++;
      }
      if (ns == 0) continue;
      if (write == 1){
        h_bin_response[i] = (TH1D*) h1_bin_response_temp[i]->Clone();
        h_bin_response[i]->Write();
      }
      h1_tile_dc[i]->Write();
      h1_RMS_scan[i]->Write();
      h1_RMS_on[i]->Write();
      h1_RMS_off[i]->Write();
      h1_tile_response[i]->Write();
      h1_tile_response_norm[i]->Write();
      h2_tile_rmon_imon[i]->Write();
      h1_tile_rmon[i]->Write();
      h1_tile_rmon_1[i]->Write();
      h1_all_rmon[i]->Write();
      h1_all_imon[i]->Write();
    }
    if (debug) cout<<"End file "<<ff<<endl;
  }

  TCanvas *c_cross = new TCanvas("c_cross","",1000, 1000);
  gPad->SetLogz();
  for (int i = 1; i< 32; i++){
    h2D_crosstalk->Fill(i, i, 1);
  }
  h2D_crosstalk->SetTitle(";Read Tile; Source Over Tile");
  h2D_crosstalk->Draw("colz");
  c_cross->SaveAs(Form("%s/crosstalk_test.png", save_dir_plot.c_str()));
  c_cross->SaveAs(Form("%s/crosstalk_test.pdf", save_dir_plot.c_str()));


  out_hist_file->cd();
  h2D_crosstalk->Write();


  /////////

  TH1D *h_blank = new TH1D("h_blank", "", 30, -0.1, 0.5);

  TH1D *h_tile_cross[31];
  TGraph *g_near[31];
  TGraph *g_far[31];
  for (int i = 0; i < 31; i++){
    h_tile_cross[i] = new TH1D(Form("h_tile_%d_fixed", i+1), "", 30, -0.1, 0.5);
    g_near[i] = new TGraph();
    g_far[i] = new TGraph();
  }
  TH1D *h_tile_cross_near = new TH1D("h_tile_cross_near","", 32, -0.1, 0.3);
  TH1D *h_tile_cross_far = new TH1D("h_tile_cross_far","", 32, -0.1, 0.3);

  int xbin = 31;
  int ybin = 31;

  int ct[31] = {0};
  for (int i = 1; i <= xbin; i++){
    for (int j = 1; j <= ybin; j++){
      if (i==1 || j == 1){
        h_tile_cross[i-1]->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          h_tile_cross_near->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_near[i-1]->Set(g_near[i-1]->GetN() + 1);
          g_near[i-1]->SetPoint(g_near[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          h_tile_cross_far->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_far[i-1]->Set(g_far[i-1]->GetN() + 1);
          g_far[i-1]->SetPoint(g_far[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
      else if (i%2 == 0 && j%2 == 0){
        h_tile_cross[i-1]->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          h_tile_cross_near->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_near[i-1]->Set(g_near[i-1]->GetN() + 1);
          g_near[i-1]->SetPoint(g_near[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          h_tile_cross_far->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_far[i-1]->Set(g_far[i-1]->GetN() + 1);
          g_far[i-1]->SetPoint(g_far[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
      else if (i%2 == 1 && j%2 == 1){
        h_tile_cross[i-1]->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          h_tile_cross_near->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_near[i-1]->Set(g_near[i-1]->GetN() + 1);
          g_near[i-1]->SetPoint(g_near[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          h_tile_cross_far->Fill(h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 1);
          g_far[i-1]->Set(g_far[i-1]->GetN() + 1);
          g_far[i-1]->SetPoint(g_far[i-1]->GetN() - 1,h2D_crosstalk->GetBinContent(h2D_crosstalk->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
    }
  }

  float xPos = 0.4;
  float xPos2 = 0.55;
  float yPos = 0.8;
  float yPos2 = 0.83;
  float dy = 0.05;
  float dy2 = 0.06;
  float fontType = 43;
  float fontSize = 13;
  float fontType2 = 43;
  float fontSize2 = 15;

  TCanvas* c3 = new TCanvas("c3","", 700, 700);
  TPad* pad1;
  TPad* overlay1;
  pad1= new TPad("pad1", "", 0,0,1,1);
  pad1->Draw();
  overlay1 = new TPad("overlay1","", 0, 0, 1,1);//"overlay[8*j + i]->SetFillStyle(4000);
  overlay1->SetFillColor(0);
  overlay1->SetFrameFillStyle(4004);
  overlay1->SetFillStyle(4004);

  overlay1->SetTicks(0);
  overlay1->Draw();
  SetyjPadStyle();
  for(int i = 0; i < 31; i++){
    pad1->cd();
    h_tile_cross[i]->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
    h_tile_cross[i]->GetXaxis()->SetTitleOffset(1.5);
    h_tile_cross[i]->GetYaxis()->SetTitleOffset(1.5);

    h_tile_cross[i]->SetLineColor(kBlue+1);
    h_tile_cross[i]->SetFillColorAlpha(kBlue -2, .4);
    h_tile_cross[i]->DrawCopy("hist");
    drawText("#bf{sEPD} Crosstalk Test", 0.55,0.85,0, 1, fontSize+5, fontType);
    drawText(Form("#bf{%s} Tile %d",sector_addon, i+1), 0.55,0.8,0, 1, fontSize+5, fontType);

    overlay1->cd();
    h_blank->SetMaximum(6.);
    h_blank->GetXaxis()->SetLabelSize(0);
    h_blank->GetYaxis()->SetLabelSize(0);
    h_blank->GetXaxis()->SetTickLength(0);
    h_blank->GetYaxis()->SetTickLength(0);
    h_blank->Draw();
    g_near[i]->SetMarkerStyle(8);
    g_near[i]->SetMarkerSize(2);
    g_near[i]->SetMarkerColor(kBlack);
    g_near[i]->Draw("P");
    g_far[i]->SetMarkerStyle(8);
    g_far[i]->SetMarkerSize(2);
    g_far[i]->SetMarkerColor(kRed);
    g_far[i]->Draw("P");

    TLegend *tl1 = new TLegend(0.5, 0.55, 0.9, 0.75);
    tl1->AddEntry(h_tile_cross[i],Form("Crosstalk: %.2f +/- %.2f", h_tile_cross[i]->GetMean(),  h_tile_cross[i]->GetRMS()));
    tl1->AddEntry(g_near[i],"Neighboring tile (New)");
    tl1->AddEntry(g_far[i],"Next to neighboring tile (New)");
    tl1->SetTextFont(50);
    tl1->Draw();
    c3->SaveAs(Form("%s/Crosstalk_%s_tile_%d.pdf", save_dir_plot.c_str(),sector_addon, i+1));

  }
  yPos = 0.84;
  xPos = xPos + 0.1;
  TCanvas* c4 = new TCanvas("c4","", 600, 600);
  TPad *pad4 = new TPad("pad4","", 0.0, 0.0, 1.0, 1.0);

  pad4->SetBottomMargin(0.2);
  pad4->Draw();
  h_tile_cross_far->SetLineColor(kBlue +2);
  h_tile_cross_far->SetLineWidth(2);
  double max_vi = max(h_tile_cross_far->GetBinContent(h_tile_cross_far->GetMaximumBin()),h_tile_cross_near->GetBinContent(h_tile_cross_near->GetMaximumBin()));
  h_tile_cross_far->SetMaximum(max_vi+1);
  h_tile_cross_far->SetMinimum(0);

  h_tile_cross_far->SetFillColorAlpha(kBlue -2, .4);
  h_tile_cross_far->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
  h_tile_cross_far->GetXaxis()->SetTitleOffset(2);
  h_tile_cross_near->SetLineColor(kSpring+2);
  h_tile_cross_near->SetLineWidth(2);

  h_tile_cross_near->SetFillColorAlpha(kSpring -2, .4);
  h_tile_cross_far->DrawCopy("hist");
  h_tile_cross_near->DrawCopy("hist same");
  TLine *tl = new TLine(h_tile_cross_near->GetMean(),0,h_tile_cross_near->GetMean(), max_vi + 1 );
  tl->SetLineWidth(2);
  tl->SetLineColor(kBlack);
  tl->SetLineStyle(9);
  tl->Draw();
  tl = new TLine(h_tile_cross_far->GetMean(),0,h_tile_cross_far->GetMean(), max_vi + 1 );
  tl->SetLineWidth(2);
  tl->SetLineColor(kBlack);
  tl->SetLineStyle(9);
  tl->Draw();
  drawText(Form("#bf{sEPD} Crosstalk Test - #bf{%s}", sector_addon), xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("Nearest and Next to Nearest neighbor", xPos,yPos-dy,0, 1, fontSize+2, fontType);
  TLegend *tl4 = new TLegend(0.6, 0.55, 0.91, 0.75);
  tl4->AddEntry(h_tile_cross_near,"Nearest Neighbor");
  tl4->AddEntry(h_tile_cross_far,"Next Neighbor");
  tl4->Draw();
  drawText(Form("Near Neighbor Mean: %.3f", h_tile_cross_near->GetMean()), xPos + 0.12 ,0.51,0, 1, fontSize+2, fontType);
  drawText(Form("Next Neighbor Mean: %.3f", h_tile_cross_far->GetMean()), xPos+.12,0.51 - dy,0, 1, fontSize+2, fontType);

  c4->SaveAs(Form("%s/nextandnear_crosstalk_%s.pdf", save_dir_plot.c_str(), sector_addon));
  out_hist_file->cd();
  h_tile_cross_near->Write();
  h_tile_cross_far->Write();
////////////



  const int cb = h_bin_response[1]->GetNbinsX();
  const int bb = 12;

  TProfile *hp_crosstalk_distance = new TProfile("hp_crosstalk_distance","", bb, -2., 2.,"s");
  TProfile *hp_crosstalk_distance_tile[32];
  for (int i = 0; i < 32; i++){
    hp_crosstalk_distance_tile[i] = new TProfile(Form("hp_crosstalk_distance_%d",i),"", bb, -2., 2.,"s");
  }
  for (int i = 1; i < 32; i++){
    cout<<"bagend "<<i<<": "<<max_x[i]<<", "<<bag[i]<<", "<<end[i]<<endl;
    if (i == 30) cout<<i<<endl;
    for (int j = 1; j <= cb ; j++){
      double x = h_bin_response[i]->GetBinCenter(j);
      if ( x < bag[i]){
        if ( i > 3 ){
          if (x > bag[i-2]){
            double third = -1*((abs(x - bag[i]))/(abs(bag[i] - bag[i-2])));
            if (i == 30) cout<<x<<" -> "<<third<<endl;

            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));
          }
          else if (x > bag[i - 4] && i != 4){
            double third = -1 - ((abs(x - bag[i - 2]))/(abs(bag[i - 2] - bag[i-4])));
            if (i == 30) cout<<x<<" -> "<<third<<endl;

            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));

          }
          else if (x > bag[1] && i == 4){
            double third = -1 - ((abs(x - bag[1]))/(abs(bag[2] - bag[1])));
            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));

          }
        }
        else if (i > 1){
          if (x > bag[1]){
            double third = -1*abs((x - bag[i])/(bag[i] - bag[1]));
            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));
          }
        }
      }
      if ( x > end[i]){
        if ( i < 28 ){
          if (x < end[i+2]){
            double third = abs((x - end[i])/(end[i+2] - end[i]));
            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));
          }
          else if (x < end[i+4]){
            double third = 1 + abs((x - end[i + 2])/(end[i + 4] - end[i + 2]));
            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));
          }
        }
        else if (i < 30){
          if (x < end[i+2]){
            double third = abs((x - end[i])/(end[i+2] - end[i]));
            hp_crosstalk_distance->Fill(third, h_bin_response[i]->GetBinContent(j));
            hp_crosstalk_distance_tile[i]->Fill(third, h_bin_response[i]->GetBinContent(j));
          }
        }
      }
    }
  }
  double v,ve,vv,vve;
  TMultiGraph *g_outliers = new TMultiGraph();


  xPos = 0.2;
  yPos = 0.87;
  double maxp = 0.5;
  double minp = -0.1;
  TCanvas *c8 = new TCanvas("c8","",700, 500);
  hp_crosstalk_distance->SetLineColor(kRed);
  hp_crosstalk_distance->SetLineWidth(2);
  hp_crosstalk_distance->SetMaximum(maxp);
  hp_crosstalk_distance->SetMinimum(minp);
  hp_crosstalk_distance->SetTitle(";Distance from Observed Tile (#frac{1}{L_{tile}}); <Signal_{away}/Signal_{over}>_{bin}");

  hp_crosstalk_distance->DrawCopy("E1");
  drawText(Form("#bf{sPHENIX} sEPD Crosstalk Test - #bf{%s}", sector_addon), xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("Adjacent Tile Average", xPos,yPos - dy,0, 1, fontSize+2, fontType);
  double y1 = minp;
  double y2 = maxp;
  double x1 = -1;
  TLine *t = new TLine(x1, y1, x1, y2);
  t->SetLineStyle(10);
  t->SetLineColor(kBlack);
  t->SetLineWidth(1);
  t->Draw();
  x1 = 0;
  t = new TLine(x1, y1, x1, y2);
  t->SetLineStyle(1);
  t->SetLineColor(kBlack);
  t->SetLineWidth(1);
  t->Draw();
  x1 = 1;
  t = new TLine(x1, y1, x1, y2);
  t->SetLineStyle(10);
  t->SetLineColor(kBlack);
  t->SetLineWidth(1);
  t->Draw();
  drawText("Tile: i - 2", 0.2,yPos - 4*dy,0, 1, fontSize+5, fontType);
  drawText(" i - 1 ", 0.4 ,yPos - 3*dy,0, 1, fontSize+5, fontType);
  drawText(" i + 1 ", 0.6,yPos - 3*dy,0, 1, fontSize+5, fontType);
  drawText(" i + 2 ", 0.8,yPos - 4*dy,0, 1, fontSize+5, fontType);

  c8->SaveAs(Form("%s/Adjacent_tiles_%s.pdf", save_dir_plot.c_str(), sector_addon));
  out_hist_file->cd();

  hp_crosstalk_distance->Write();

  TCanvas *c9 = new TCanvas("c9","", 500, 500);

  gPad->SetGridx(1);
  gPad->SetGridy(1);
  hp_ratio->SetMarkerColor(kBlue+2);
  hp_ratio->SetMaximum(2);
  hp_ratio->SetMinimum(0);
  hp_ratio->SetLineColor(kBlue+2);
  hp_ratio->SetLineWidth(2);
  hp_ratio->SetMarkerSize(1);
  hp_ratio->SetMarkerStyle(8);
  hp_ratio->SetTitle(";Tile Row; Gain_{odd}/Gain_{even}");
  hp_ratio->DrawCopy("E2");
  cout<<"hp_ratio: "<<hp_ratio->GetBinContent(5);
  drawText("#bf{sPHENIX} Internal",xPos,yPos,0, 1, fontSize+2, fontType);
  drawText(Form("#bf{%s} Gain Comparison",sector_addon),xPos,yPos - dy,0, 1, fontSize+2, fontType);

  c9->SaveAs(Form("%s/GainComparison_%s.pdf", save_dir_plot.c_str(), sector_addon));
  out_hist_file->cd();
  hp_ratio->Write();
  out_hist_file->Close();




  return;
}

int Line_Test_Analysis(const std::string &config_file= "line_config.config")
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
  std::string test_type = "line";

  if (debug) cout<<"Getting File names for test: "<<test_type<<endl;

  GetFileName(filenames, test_type, sector, all_runs, ch1, ch2, debug);

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
