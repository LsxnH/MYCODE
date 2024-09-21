from optparse import OptionParser
import ROOT as ROOT   
from ROOT import TH1F, TH2F, TFile
from array import array

import sys 
sys.path.append('/home/fhe/testarea/AnpWH/source/PhysicsAnpWH/macros/')
from plotMVAROC import *

can = makeCanvas('c1')
leg = getLegend(0.18, 0.93-4*0.08, 0.50, 0.93)
leg.SetTextSize(0.045)
datafile  = ROOT.TFile("out_withsys.root", 'READ')
hists    = []

qfliprate = getHist(datafile, "RATE")
qfliperror = getHist(datafile, "RATE_TOTAL_1UP")

etaArr = array('d', [0.0,0.6,1.1, 1.37, 1.7, 2.3, 2.47])

ptArr = qfliprate.GetYaxis().GetNbins()

pTRange = ['15 < p_{T} < 60 GeV',
           '60 < p_{T} < 90 GeV',
           '90 < p_{T} < 130 GeV',
           'p_{T} > 130 GeV',]


colors   = {0: 4,
            1: 2,
            2: 419, 
            3: 800} 

myhists = {}
isSame = False

for jbin in range(1, qfliprate.GetYaxis().GetNbins()+1):
    myhists[jbin-1] = TH1F('qflip_ptbin%d'%(jbin), '', len(etaArr)-1, etaArr)
    myhists[jbin-1].SetLineColor(colors[jbin-1])
    myhists[jbin-1].SetLineWidth(2)
    leg.AddEntry(myhists[jbin-1], pTRange[jbin-1], 'l')

    for ibin in range(1, qfliprate.GetXaxis().GetNbins()+1): 
        myhists[jbin-1].SetBinContent(ibin, qfliprate.GetBinContent(ibin, jbin))
        myhists[jbin-1].SetBinError(ibin, qfliperror.GetBinContent(ibin, jbin))

    if not isSame:
        myhists[jbin-1].SetStats(False)
        myhists[jbin-1].GetXaxis().SetTitleOffset(1.1)
        myhists[jbin-1].GetXaxis().SetTitleSize(0.045)
        myhists[jbin-1].GetXaxis().SetLabelSize(0.04)
        myhists[jbin-1].GetXaxis().SetTitle('|#eta|')

        myhists[jbin-1].GetYaxis().SetTitleOffset(1.15)
        myhists[jbin-1].GetYaxis().SetTitleSize(0.045)
        myhists[jbin-1].GetYaxis().SetLabelSize(0.04)

        myhists[jbin-1].GetYaxis().SetRangeUser(0.00, 0.05)
        myhists[jbin-1].GetYaxis().SetTitle('rate')

        myhists[jbin-1].Draw("E1")

        isSame = True

    else:
        myhists[jbin-1].Draw("E1 SAME") 
   
    #tex = getTopLatex(0.15, 0.85, pTRange[ptbin])
    #tex.Draw()
leg.Draw()
updateCanvas(can, path = "plot")

print(myhists)

