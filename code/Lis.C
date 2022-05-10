#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TFile.h"
#include <dirent.h>
#include "sEPD_Line_Functions.C"

void Lis(){

  int sector = 5;
  std::string filename_even = "sEPD_20220425-1515_s05_line_ch1_30.txt";
  std::string filename_odd = "sEPD_20220426-0923_s05_line_ch1_31.txt";

  const std::string datadir = "/home/daniel/Documents/Boulder/sPhenix/Projects/sEPD/sEPD_Testing/data/s05/";
  const std::string savedir = "/home/daniel/Documents/Boulder/sPhenix/Projects/sEPD/Ryan/";

  bool debug = true;

  MakeRootFile_Line(filename_odd, datadir, savedir, 16, debug);
  MakeRootFile_Line(filename_even, datadir, savedir, 16, debug);

  return;
}
