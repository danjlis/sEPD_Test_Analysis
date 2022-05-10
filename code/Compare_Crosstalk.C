#include "yjUtility.h"

void Compare_Crosstalk(){
  SetyjPadStyle();
  gStyle->SetOptStat(0);
  TFile *f_1 = new TFile("crosstalk_dark_fixed.root");
  TFile *f_2 = new TFile("crosstalk_dark_orig.root");

  TH2D *h2_orig = (TH2D*)((TCanvas*)f_2->Get("c_cross"))->GetPrimitive("h2D_crosstalk");
  TH2D *h2_fixed = (TH2D*)((TCanvas*)f_1->Get("c_cross"))->GetPrimitive("h2D_crosstalk");
  TH1D *h_blank = new TH1D("h_blank", "", 30, -0.1, 0.5);

  TH1D *h_tile_fixed[31];
  TH1D *h_tile_orig[31];
  TH1D *h_tile_even_fixed[31];
  TH1D *h_tile_even_orig[31];
  TH1D *h_tile_odd_fixed[31];
  TH1D *h_tile_odd_orig[31];
  TGraph *near_fixed_tile[31];
  TGraph *far_fixed_tile[31];
  TGraph *near_orig_tile[31];
  TGraph *far_orig_tile[31];
  for (int i = 0; i < 31; i++){
    h_tile_fixed[i] = new TH1D(Form("h_tile_%d_fixed", i+1), "", 30, -0.1, 0.5);
    h_tile_orig[i] = new TH1D(Form("h_tile_%d_orig", i+1), "", 30, -0.1, 0.5);
    h_tile_even_fixed[i] = new TH1D(Form("h_tile_even_%d_fixed", i+1), "", 30, -0.1, 0.5);
    h_tile_even_orig[i] = new TH1D(Form("h_tile_even_%d_orig", i+1), "", 30, -0.1, 0.5);
    h_tile_odd_fixed[i] = new TH1D(Form("h_tile_odd_%d_fixed", i+1), "", 30, -0.1, 0.5);
    h_tile_odd_orig[i] = new TH1D(Form("h_tile_odd_%d_orig", i+1), "", 30, -0.1, 0.5);
    near_fixed_tile[i] = new TGraph();
    far_fixed_tile[i] = new TGraph();
    near_orig_tile[i] = new TGraph();
    far_orig_tile[i] = new TGraph();
  }
  TH1D *orig_near = new TH1D("orig_near","", 30, 0, 0.3);
  TH1D *fixed_near = new TH1D("fixed_near","", 30, 0, 0.3);
  TH1D *orig_far = new TH1D("orig_far","", 30, 0, 0.3);
  TH1D *fixed_far = new TH1D("fixed_far","", 30, 0, 0.3);

  int xbin = 31;
  int ybin = 31;

  int ct[31] = {0};
  for (int i = 1; i <= xbin; i++){
    for (int j = 1; j <= ybin; j++){
      //h_tile_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
      //h_tile_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
      if (i==1 || j == 1){
        h_tile_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
        h_tile_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          orig_near->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_near->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          near_fixed_tile[i-1]->Set(near_fixed_tile[i-1]->GetN() + 1);
          near_fixed_tile[i-1]->SetPoint(near_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          near_orig_tile[i-1]->Set(near_orig_tile[i-1]->GetN() + 1);
          near_orig_tile[i-1]->SetPoint(near_orig_tile[i-1]->GetN() - 1,h2_orig->GetBinContent(h2_orig->GetBin(i, j)),  0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          orig_far->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_far->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          far_fixed_tile[i-1]->Set(far_fixed_tile[i-1]->GetN() + 1);
          far_fixed_tile[i-1]->SetPoint(far_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          far_orig_tile[i-1]->Set(far_orig_tile[i-1]->GetN() + 1);
          far_orig_tile[i-1]->SetPoint(far_orig_tile[i-1]->GetN() - 1, h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
      else if (i%2 == 0 && j%2 == 0){
        //h_tile_even_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
        //h_tile_even_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
        h_tile_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
        h_tile_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          orig_near->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_near->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          near_fixed_tile[i-1]->Set(near_fixed_tile[i-1]->GetN() + 1);
          near_fixed_tile[i-1]->SetPoint(near_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          near_orig_tile[i-1]->Set(near_orig_tile[i-1]->GetN() + 1);
          near_orig_tile[i-1]->SetPoint(near_orig_tile[i-1]->GetN() - 1,h2_orig->GetBinContent(h2_orig->GetBin(i, j)),  0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          orig_far->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_far->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          far_fixed_tile[i-1]->Set(far_fixed_tile[i-1]->GetN() + 1);
          far_fixed_tile[i-1]->SetPoint(far_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          far_orig_tile[i-1]->Set(far_orig_tile[i-1]->GetN() + 1);
          far_orig_tile[i-1]->SetPoint(far_orig_tile[i-1]->GetN() - 1, h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
      else if (i%2 == 1 && j%2 == 1){
        //h_tile_odd_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
        //h_tile_odd_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
        h_tile_orig[i-1]->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
        h_tile_fixed[i-1]->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
        if (abs(i - j) <= 2){
          orig_near->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_near->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          near_fixed_tile[i-1]->Set(near_fixed_tile[i-1]->GetN() + 1);
          near_fixed_tile[i-1]->SetPoint(near_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          near_orig_tile[i-1]->Set(near_orig_tile[i-1]->GetN() + 1);
          near_orig_tile[i-1]->SetPoint(near_orig_tile[i-1]->GetN() - 1,h2_orig->GetBinContent(h2_orig->GetBin(i, j)),  0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
        else if (abs(i - j) <= 4){
          orig_far->Fill(h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 1);
          fixed_far->Fill(h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 1);
          far_fixed_tile[i-1]->Set(far_fixed_tile[i-1]->GetN() + 1);
          far_fixed_tile[i-1]->SetPoint(far_fixed_tile[i-1]->GetN() - 1,h2_fixed->GetBinContent(h2_fixed->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          far_orig_tile[i-1]->Set(far_orig_tile[i-1]->GetN() + 1);
          far_orig_tile[i-1]->SetPoint(far_orig_tile[i-1]->GetN() - 1, h2_orig->GetBinContent(h2_orig->GetBin(i, j)), 0.5+0.5*ct[i-1]);
          ct[i-1]++;
        }
      }
    }
  }
/*
  TCanvas* c1 = new TCanvas("c1","", 2000, 2000);
  TPad* pads[32];
  TPad* overlay[32];
  for (int i = 0; i < 8; i++){
    for (int j = 0; j < 4; j++){
      pads[8*j + i] = new TPad(Form("pad2_%d", 8*j+i), "", static_cast<double>(i)*0.125, static_cast<double>(j)*0.25, (static_cast<double>(i)+1.)*0.125, (static_cast<double>(j)+1.)*0.25);
      pads[8*j+i]->Draw();
      overlay[8*j + i] = new TPad(Form("pad2_%d", 8*j+i), "", static_cast<double>(i)*0.125, static_cast<double>(j)*0.25, (static_cast<double>(i)+1.)*0.125, (static_cast<double>(j)+1.)*0.25);
      overlay[8*j + i]->SetFillStyle(4000);
      overlay[8*j + i]->SetFillColor(0);
      overlay[8*j + i]->SetFrameFillStyle(4000);
      overlay[8*j + i]->SetTicks(0);

      overlay[8*j+i]->Draw();
    }
  }
*/
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
  for(int i = 0; i < 31; i++){
    double max_v = max(h_tile_orig[i]->GetBinContent(h_tile_orig[i]->GetMaximumBin()),h_tile_fixed[i]->GetBinContent(h_tile_fixed[i]->GetMaximumBin()));
    pad1->cd();
    h_tile_orig[i]->SetLineColor(kBlue +1);
    h_tile_orig[i]->SetFillColorAlpha(kBlue -2, .4);
    h_tile_orig[i]->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
    h_tile_orig[i]->GetXaxis()->SetTitleOffset(1.5);
    h_tile_orig[i]->GetYaxis()->SetTitleOffset(1.5);

    h_tile_fixed[i]->SetLineColor(kSpring+1);
    h_tile_fixed[i]->SetFillColorAlpha(kSpring -2, .4);
    h_tile_orig[i]->SetMaximum(max_v + 1.0);
    h_tile_orig[i]->DrawCopy("hist");
    h_tile_fixed[i]->DrawCopy("hist same");
    drawText("#bf{sEPD} Crosstalk Test", 0.55,0.85,0, 1, fontSize+5, fontType);
    drawText(Form("Tile %d", i+1), 0.55,0.8,0, 1, fontSize+5, fontType);

    overlay1->cd();
    h_blank->SetMaximum(6.);
    h_blank->GetXaxis()->SetLabelSize(0);
    h_blank->GetYaxis()->SetLabelSize(0);
    h_blank->GetXaxis()->SetTickLength(0);
    h_blank->GetYaxis()->SetTickLength(0);
    h_blank->Draw();
    near_orig_tile[i]->SetMarkerStyle(4);
    near_orig_tile[i]->SetMarkerSize(2);
    near_orig_tile[i]->SetMarkerColor(kBlack);
    near_orig_tile[i]->Draw("P");
    far_orig_tile[i]->SetMarkerStyle(4);
    far_orig_tile[i]->SetMarkerSize(2);
    far_orig_tile[i]->SetMarkerColor(kRed);
    far_orig_tile[i]->Draw("P");
    near_fixed_tile[i]->SetMarkerStyle(8);
    near_fixed_tile[i]->SetMarkerSize(2);
    near_fixed_tile[i]->SetMarkerColor(kBlack);
    near_fixed_tile[i]->Draw("P");
    far_fixed_tile[i]->SetMarkerStyle(8);
    far_fixed_tile[i]->SetMarkerSize(2);
    far_fixed_tile[i]->SetMarkerColor(kRed);
    far_fixed_tile[i]->Draw("P");

    TLegend *tl1 = new TLegend(0.5, 0.55, 0.9, 0.75);
    tl1->AddEntry( h_tile_orig[i],Form("Old DC: %.2f +/- %.2f", h_tile_orig[i]->GetMean(),  h_tile_orig[i]->GetRMS()));
    tl1->AddEntry(h_tile_fixed[i],Form("New DC: %.2f +/- %.2f", h_tile_fixed[i]->GetMean(),  h_tile_fixed[i]->GetRMS()));
    tl1->AddEntry(near_orig_tile[i],"Neighboring tile (Old)");
    tl1->AddEntry(far_orig_tile[i],"Next to neighboring tile (Old)");
    tl1->AddEntry(near_fixed_tile[i],"Neighboring tile (New)");
    tl1->AddEntry(far_fixed_tile[i],"Next to neighboring tile (New)");
    tl1->SetTextFont(50);
    tl1->Draw();
    c3->SaveAs(Form("./plot/Compare_Crosstalk_tile_%d.pdf", i+1));

  }
  TCanvas* c2 = new TCanvas("c2","", 600, 600);
  TPad *pad = new TPad("pad","", 0.0, 0.0, 1.0, 1.0);
  pad->SetBottomMargin(0.2);
  pad->Draw();
  orig_near->SetLineColor(kBlue +2);
  orig_near->SetLineWidth(2);

  orig_near->SetFillColorAlpha(kBlue -2, .4);
  fixed_near->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
  fixed_near->GetXaxis()->SetTitleOffset(2);
  fixed_near->SetLineColor(kSpring+2);
  fixed_near->SetLineWidth(2);

  fixed_near->SetFillColorAlpha(kSpring -2, .4);
  orig_near->Draw("hist");
  fixed_near->Draw("hist same");
  drawText("#bf{sEPD} Crosstalk Test", xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("Nearest neighbor distribution", xPos,yPos-dy,0, 1, fontSize+2, fontType);

  TLegend *tl = new TLegend(0.58, 0.6, 0.88, 0.75);
  tl->AddEntry(orig_near,"One Time");
  tl->AddEntry(fixed_near,"Many Times");
  tl->Draw();

  c2->SaveAs("./plot/near_old_new.pdf");

  yPos = 0.84;
  TCanvas* c4 = new TCanvas("c4","", 1200, 600);
  TPad *pad4 = new TPad("pad4","", 0.0, 0.0, .5, 1.0);
  pad4->SetBottomMargin(0.2);
  pad4->SetRightMargin(0);
  pad4->SetLeftMargin(0.2);
  pad4->Draw();
  TPad *pad5 = new TPad("pad5","", 0.5, 0.0, 1.0, 1.0);
  pad5->SetBottomMargin(0.2);
  pad5->SetLeftMargin(0);
  pad5->SetRightMargin(0.2);
  pad5->Draw();
  pad4->cd();
  fixed_far->SetLineColor(kBlue +2);
  fixed_far->SetLineWidth(2);
  double max_vi = max(fixed_far->GetBinContent(fixed_far->GetMaximumBin()),fixed_near->GetBinContent(fixed_near->GetMaximumBin()));
  fixed_far->SetMaximum(max_vi+1);
  fixed_far->SetFillColorAlpha(kBlue -2, .4);
  fixed_far->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
  fixed_far->GetXaxis()->SetTitleOffset(2);
  fixed_near->SetLineColor(kSpring+2);
  fixed_near->SetLineWidth(2);

  fixed_near->SetFillColorAlpha(kSpring -2, .4);
  fixed_far->Draw("hist");
  fixed_near->Draw("hist same");
  drawText("#bf{sEPD} Crosstalk Test", xPos,yPos,0, 1, fontSize+2, fontType);
  drawText("Nearest and Next to Nearest neighbor", xPos,yPos-dy,0, 1, fontSize+2, fontType);
  drawText("New Method", xPos,yPos-2*dy,0, 1, fontSize+2, fontType);


  pad5->cd();
  max_vi = max(orig_far->GetBinContent(orig_far->GetMaximumBin()),orig_near->GetBinContent(orig_near->GetMaximumBin()));
  orig_far->SetMaximum(max_vi+1);
  orig_far->SetLineColor(kBlue +2);
  orig_far->SetLineWidth(2);

  orig_far->SetFillColorAlpha(kBlue -2, .4);
  orig_far->SetTitle(";Signal_{away}/Signal_{over}; Counts (# tiles)");
  orig_far->GetXaxis()->SetTitleOffset(2);
  orig_far->GetYaxis()->SetTitleOffset(1.5);

  orig_near->SetLineColor(kSpring+2);
  orig_near->SetLineWidth(2);

  orig_near->SetFillColorAlpha(kSpring -2, .4);
  orig_far->Draw("hist Y+");
  orig_near->Draw("hist same");
  drawText("Old Method", xPos,yPos-dy,0, 1, fontSize+2, fontType);
  TLegend *tl4 = new TLegend(0.4, 0.55, 0.75, 0.75);
  tl4->AddEntry(orig_near,"Nearest Neighbor");
  tl4->AddEntry(orig_far,"Next to Nearest Neighbor");
  tl4->Draw();
  c4->SaveAs("./plot/near_nextnear_new.pdf");

  /*
  for(int i = 0 ; i < 31; i++){
    TCanvas* c3 = new TCanvas("c3","", 700, 700);
    TPad* pad1;
    TPad* overlay1;
    pad1= new TPad("pad1", "", 0,0,1,1);
    pad1->Draw();
    overlay1 = new TPad("overlay1","", 0, 0, 1,1);//"overlay[8*j + i]->SetFillStyle(4000);
    overlay1->SetFillColor(0);
    overlay1->SetFrameFillStyle(4000);
    overlay1->SetTicks(0);
    overlay1->Draw();
    pad1->cd();

    h_tile_orig[i]->SetLineColor(kBlue +1);
    h_tile_orig[i]->SetFillColorAlpha(kBlue -2, .4);
    h_tile_orig[i]->SetTitle(";Signal_{away}/Signal_{over};");
    h_tile_fixed[i]->SetLineColor(kSpring+1);
    h_tile_fixed[i]->SetFillColorAlpha(kSpring -2, .4);

    h_tile_orig[i]->GetYaxis()->SetTitle("Counts");

    h_tile_orig[i]->GetXaxis()->SetTitleOffset(1.5);
    h_tile_orig[i]->Draw("hist");
    h_tile_fixed[i]->Draw("hist same");

    drawText(Form("Tile %d", i+1), xPos2,yPos2,0, 1, fontSize2+2, fontType2);
    drawText("Crosstalk Test", xPos2,yPos2 - dy2,0, 1, fontSize2+2, fontType2);

  //  overlay1->cd();
  //  h_blank->Draw("hist same");
    near_orig_tile[i]->SetMarkerSize(2);

    far_orig_tile[i]->SetMarkerSize(2);

    near_fixed_tile[i]->SetMarkerSize(2);

    far_fixed_tile[i]->SetMarkerSize(2);

    near_orig_tile[i]->Draw("P");
    far_orig_tile[i]->Draw("P");
    near_fixed_tile[i]->Draw("P");
    far_fixed_tile[i]->Draw("P");
    TLegend *tl2 = new TLegend(0.45, 0.55, 0.9, 0.75);
    tl2->AddEntry(h_tile_orig[i],Form("Old DC: %.2f +/- %.2f", h_tile_orig[i]->GetMean(),  h_tile_orig[i]->GetRMS()));
    tl2->AddEntry(h_tile_fixed[i],Form("New DC: %.2f +/- %.2f", h_tile_fixed[i]->GetMean(),  h_tile_fixed[i]->GetRMS()));
    tl2->AddEntry(near_orig_tile[i],"Neighboring tile (Old)");
    tl2->AddEntry(far_orig_tile[i],"Next to neighboring tile (Old)");
    tl2->AddEntry(near_fixed_tile[i],"Neighboring tile (New)");
    tl2->AddEntry(far_fixed_tile[i],"Next to neighboring tile (New)");
    tl2->SetTextFont(43);
    tl2->Draw();

    c3->SaveAs(Form("../../plot/tile_%d_dark.pdf", i+1));

  }
  */
  return;
}
