from optparse import OptionParser
import ROOT as ROOT   
from ROOT import TH1F, TH2F, TFile
from array import array
import sys 

datafile  = ROOT.TFile("out_withsys.root", 'READ')

qfliprate  = datafile.Get("RATE")
qfliperror = datafile.Get("RATE_TOTAL_1UP")
qflipstat  = datafile.Get("RATE_STAT_1UP")
qflipbkg   = datafile.Get("RATE_BKG_1UP")
qflipmll   = datafile.Get("RATE_MLLWINDOW_1UP")

listerror  = []
liststat   = []
listbkg    = []
listmll    = []

for jbin in range(1, qfliprate.GetYaxis().GetNbins()+1):
    for ibin in range(1, qfliprate.GetXaxis().GetNbins()+1): 
        listerror.append(round(100*qfliperror.GetBinContent(ibin, jbin)/qfliprate.GetBinContent(ibin, jbin), 2))
        liststat .append(round(100*qflipstat .GetBinContent(ibin, jbin)/qfliprate.GetBinContent(ibin, jbin), 2))
        listbkg  .append(round(100*qflipbkg  .GetBinContent(ibin, jbin)/qfliprate.GetBinContent(ibin, jbin), 2))
        listmll  .append(round(100*qflipmll  .GetBinContent(ibin, jbin)/qfliprate.GetBinContent(ibin, jbin), 2))

listerror.sort()
liststat.sort()
listbkg.sort()
listmll.sort()

print "total error", listerror
print "stats error", liststat
print "bkg error",   listbkg
print "mll error",   listmll