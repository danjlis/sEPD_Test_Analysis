#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>

void Merge_Files(){
  std::string fname_start = "../data/s05/sEPD_20220422-1257_s05_full.txt";
  std::string fname_end = "/home/daniel/Documents/Boulder/sPhenix/Projects/sEPD/sEPD_Testing/data/s05/sEPD_20220425-0855_s05_full.txt";

  ifstream infile_start(fname_start);
  ifstream infile_end(fname_end);

  Float_t xpos, ypos;

  /////////////////////////////////////
  // IMPORT NUMBERS FROM THE TXT FILE
  Long64_t nLines = 0;
  Bool_t isDiffPosition = false;
  ofstream outfile;
  outfile.open(fname_start, std::ios::app);

  while (infile_start){
   string s;
   if (!getline( infile_start, s )) break;
   if (s.empty()) continue;
   if (!isdigit(s.at(0))) continue;

   istringstream ss( s );
   vector<string> vec_str;
   cout<<"hi"<<endl;

   while (ss)
   {
     string stemp;
     if (!getline( ss, stemp, ',' )) break;
     //cout<<stemp<<endl;

     vec_str.push_back( stemp );
   }

   if(vec_str.size()==8){
     xpos = stof(vec_str[4]);
     ypos = stof(vec_str[5]);
     cout<<xpos<<", "<<ypos<<endl;
   }
  }
  int device, tile, channel, trial;
  float xpos_temp, ypos_temp;
  float temp_b, temp_c, imon, rmon, vcomp;
  bool write_line = false;
  bool here_fisrt = true;
  while (infile_end){
   string s;
   if (!getline( infile_end, s )) break;
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
   if (vec_str.size()==11 && !write_line){
     xpos_temp = stof(vec_str[4]);
     ypos_temp = stof(vec_str[5]);
     if (xpos_temp == 86 && ypos_temp == 4.5){
       write_line = true;
       cout<<xpos_temp<<", "<<ypos_temp<<endl;
     }
   }
   if(vec_str.size()==11 && write_line){
     device = stoi(vec_str[0]);
     channel = stoi(vec_str[1]);
     tile = stoi(vec_str[2]);
     trial = stoi(vec_str[3]);
     xpos_temp = stof(vec_str[4]);
     ypos_temp = stof(vec_str[5]);
     temp_b = stof(vec_str[6]);
     temp_c = stof(vec_str[7]);
     imon = stof(vec_str[8]);
     rmon = stof(vec_str[9]);
     vcomp = stof(vec_str[10]);

     outfile<<device<<", "<<channel<<", "<<tile<<", "<< trial<<", "<<xpos_temp<<", "<<ypos_temp<<", "<<temp_b<<", "<<temp_c<<endl;
     outfile<<", "<<imon<<", "<<rmon<<", "<<vcomp<<endl;
     outfile<<" "<<endl;
   }
  }
  outfile.close();
  return;
}
