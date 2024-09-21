 #!/usr/bin/env python

import os
import re
import math
import sys
import time
import optparse
    
p = optparse.OptionParser()

p.add_option('-d', '--debug',   action='store_true', default=False,   dest='debug')
p.add_option('-s', '--save',    action='store_true', default=False,   dest='save')
p.add_option('-b', '--batch',   action='store_true', default=False,   dest='batch')
p.add_option('-w', '--wait',    action='store_true', default=False,   dest='wait')
p.add_option('--wait-all',      action='store_true', default=False,   dest='wait_all')
p.add_option('--logy',          action='store_true', default=False,   dest='logy')
p.add_option('--png',           action='store_true', default=False,   dest='png')
p.add_option('--draw-all',      action='store_true', default=False,   dest='draw_all')
p.add_option('--draw-outliers', action='store_true', default=False,   dest='draw_outliers')
p.add_option('--draw-eff',      action='store_true', default=False,   dest='draw_eff')
p.add_option('--draw-eff-only', action='store_true', default=False,   dest='draw_eff_only')

p.add_option('--ref-run', '-r',       type='int',    default=302872,        dest='ref_run')
p.add_option('--select-run',          type='int',    default=None,          dest='select_run')
p.add_option('--plot-min-nout',       type='int',    default=3,             dest='plot_min_nout')
p.add_option('--draw-panels',         type='int',    default=1,             dest='draw_panels')

p.add_option('--min-sector-nden',     type='int',    default=499,          dest='min_sector_nden')
p.add_option('--min-sector-nnum',     type='int',    default=0,             dest='min_sector_nnum')
p.add_option('--min-panel-nden',      type='int',    default=24,            dest='min_panel_nden')
p.add_option('--min-panel-nnum',      type='int',    default=0,             dest='min_panel_nnum')

p.add_option('--min-layer-pp-hits',   type='float',  default=0.10,          dest='min_layer_pp_hits')
p.add_option('--min-high-panel-eff',  type='float',  default=0.70,          dest='min_high_panel_eff')
p.add_option('--max-low-panel-eff',   type='float',  default=0.20,          dest='max_low_panel_eff')
p.add_option('--max-good-panel-chi2', type='float',  default=4.0,           dest='max_good_panel_chi2')
p.add_option('--min-out-nsigma',      type='float',  default=4.0,           dest='min_out_nsigma')
p.add_option('--bad-eff',             type='float',  default=0.5,           dest='bad_eff')
p.add_option('--min-ilumi',           type='float',  default=50.0,          dest='min_ilumi')

p.add_option('--key', '-k',       type='string', default=None,            dest='key')
p.add_option('--input', '-i',     type='string', default=None,            dest='input')
p.add_option('--wait-func',       type='string', default=None,            dest='wait_func')
p.add_option('--wait-place',      type='string', default=None,            dest='wait_place')
p.add_option('--pref',            type='string', default=None,            dest='pref')
p.add_option('--post',            type='string', default=None,            dest='post')
p.add_option('--outdir', '-o',    type='string', default='plot-rpc-eff',  dest='outdir')
p.add_option('--outfile',         type='string', default='rpc_eff.root',  dest='outfile')

(options, args) = p.parse_args()

canvases      = {}
nprint_canvas = 0
rootfile      = None

import ROOT
import PhysicsAnpRPC.PhysicsAnpRPCEff as config

if options.batch:
    ROOT.gROOT.SetBatch(True)
    
config.setPlotDefaults(ROOT)
config.setRPCColorPalette(ROOT)

#========================================================================================================
def getLog(name, level='INFO', debug=False):

    import logging

    f = logging.Formatter('%(message)s')
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.addHandler(h)
    log.setLevel(logging.INFO)

    return log

msg = getLog(os.path.basename(__file__))

#==================================================================================================================
class Run:

    ''' Class to contain simple run information'''
    
    def __init__(self, parts):
        self.run_number       = int  (parts[0])
        self.nlbs             = int  (parts[1])
        self.peak_lumi        = float(parts[2])
        self.mean_lumi        = float(parts[3])
        self.int_lumi_online  = float(parts[4])
        self.int_lumi_physics = float(parts[5])        
        self.start_time       = long (parts[6])
        self.end_time         = long (parts[7])
        
        self.rfile          = None
        self.total_int_lumi = 0.0

        self.dtime          = None
        self.dtime_err_low  = None
        self.dtime_err_up   = None
        self.dtime_title    = None
        
        self.total_ilumi         = None
        self.total_ilumi_err_low = None
        self.total_ilumi_err_up  = None
        self.total_ilumi_title   = None            

    def __hash__(self):
        return self.run_number

    def __eq__(self, other):
        return (self.run_number == other.run_number)

    def __ne__(self, other):
        return not (self.run_number == other.run_number)
    
    def IsGoodLumiRun(self):
        return (self.GetPhysicsIntLumi() > options.min_ilumi)
        
    def GetAscTime(self):
        return getTimeAsStr(self.start_time)

    def GetDeltaTime(self, get_all=False):
        if get_all:
            return (self.dtime, self.dtime_err_low, self.dtime_err_up)
        return self.dtime

    def GetDeltaTimeTitle(self):
        return self.dtime_title

    def GetTotalIntLumi(self, get_all=False):
        if get_all:
            return (self.total_ilumi, self.total_ilumi_err_low, self.total_ilumi_err_up)
        return self.total_ilumi

    def GetTotalIntLumiTitle(self):
        return self.total_ilumi_title
    
    def GetPeakInstLumi(self):
        return self.peak_lumi
    
    def GetPhysicsIntLumi(self):
        return self.int_lumi_physics
    
    def AsStr(self):
        pl = ('%d'   %self.GetPeakInstLumi  ()).rjust(5)
        il = ('%.1f' %self.GetPhysicsIntLumi()).rjust(6)

        s = 'Run #%d %s - peak lumi=%s,  int lumi=%s' %(self.run_number, self.GetAscTime(), pl, il)
        return s

    def GetSimpleDateAsStr(self):
        return getTimeAsStr(self.start_time, do_simple=True)
    
    def GetHist(self, hpath):
        if self.rfile == None:
            return None

        h = self.rfile.Get(hpath)

        if h:
            h.SetStats(False)
            h.SetDirectory(0)

        return h

#==================================================================================================================
class Eff:

    ''' 
    Class for efficiency measurements for one detector later for one run.        
    -- Run info
    -- Numerator   histogram for panel efficienies
    -- Denominator histogram for panel efficienies
    -- Help functions to access efficiency

    This class converts input 2d eta-phi histograms for numerator and 
    denominator into 1d histograms to be used with TEfficiency class.

    Each panel corresponds to one bin in input 2d histograms. After 
    conversion to 1d histograms, use bin number of the 1d histogram
    as unique id for each panel. Since 2d histograms have empty 
    bins, not every bin corresponds to valid histogram. 
    '''
    
    def __init__(self, run, detkey, label, hnum, hden):
        self.run      = run
        self.detkey   = detkey
        self.label    = label        
        self.is_ref   = (run.run_number == options.ref_run)
        
        self.storage          = []                   
        self.pos_outlier_bins = []
        self.neg_outlier_bins = []        
        
        self.den_hist = hden
        self.num_hist = hnum
        self.eff_hist = None
        
        if not self.den_hist:
            raise Exception('Eff - run #%d - invalid histogram: den%s' %(run.run_number, self.detkey))
            
        if not self.num_hist:
            raise Exception('Eff - run #%d - invalid histogram: num%s' %(run.run_number, self.detkey))

        self.nbinx     = self.den_hist.GetNbinsX()                   # number of X      bins of 2d RPC sector histogram
        self.nbiny     = self.den_hist.GetNbinsY()                   # number of y      bins of 2d RPC sector histogram
        self.nbin1d    = self.ComputeNGlobalEffBins(self.den_hist)+1 # number of global bins of 2d RPC sector histogram
        
        if self.num_hist.GetNbinsX() != self.nbinx or self.num_hist.GetNbinsY() != self.nbiny:
            raise Exception('Eff ctore - num and den histograms have different dimensions')

        self.den_th1 = self.ConvertToTH1(self.den_hist, '%s_den_th1' %self.den_hist.GetName())
        self.num_th1 = self.ConvertToTH1(self.num_hist, '%s_num_th1' %self.num_hist.GetName())
        self.eff_1d  = ROOT.TEfficiency(self.num_th1, self.den_th1)
        
        self.rpc_hist     = createRPCDetLayoutHist(self.den_hist)
        self.eff_val_hist = self.ConvertToTH2FromEff(self.eff_1d, '%s_eff_val' %self.den_hist.GetName())
        self.eff_err_hist = self.ConvertToTH2FromEff(self.eff_1d, '%s_eff_err' %self.den_hist.GetName(), True)

        self.nactive_panel = self.ComputeNActivePanels() # count number of panels that have denominator hits
        
    def GetPanelEff(self, gbin, do_err=False):
        if gbin < 1 or gbin > self.nbin1d:
            raise Exception('GetPanelEff - invalid bin number: %d' %gbin)
        
        eff_val = self.eff_1d.GetEfficiency(gbin)
            
        if do_err:
            err_low = self.eff_1d.GetEfficiencyErrorLow(gbin)
            err_up  = self.eff_1d.GetEfficiencyErrorUp (gbin)            
            return (eff_val, err_low, err_up)
        
        return eff_val
    
    def GetNGlobalEffBins(self):
        return self.nbin1d

    def GetNActivePanels(self):
        return self.nactive_panel

    def ComputeNActivePanels(self):
        nactive_panels = 0
        
        for i in range(1, self.nbin1d):
            if self.IsGoodDenBin(i):
                nactive_panels += 1

        return nactive_panels
            
    def ComputeNGlobalEffBins(self, h):
        nbin = 0

        for binx in range(1, h.GetNbinsX()+1):
            for biny in range(1, h.GetNbinsY()+1):
                nbin = max([nbin, h.GetBin(binx, biny)])

        return nbin
    
    def IsGoodEffRun(self):
        return (self.run.IsGoodLumiRun() and \
                self.GetNDenEntries() > options.min_sector_nden and \
                self.GetNNumEntries() > options.min_sector_nnum)
    
    def IsLiveEffBin(self, gbin):
        return self.IsGoodDenBin(gbin) and (self.num_th1.GetBinContent(gbin) > options.min_panel_nnum)

    def IsHighEffBin(self, gbin):
        return self.IsGoodDenBin(gbin) and (self.GetPanelEff(gbin) > options.min_high_panel_eff)

    def IsLowEffBin(self, gbin):
        return self.IsGoodDenBin(gbin) and (self.GetPanelEff(gbin) < options.max_low_panel_eff)

    def IsPosOutlierBin(self, gbin):
        return gbin in self.pos_outlier_bins

    def IsNegOutlierBin(self, gbin):
        return gbin in self.neg_outlier_bins
    
    def IsGoodDenBin(self, gbin):
        '''
        Check that global bin of 1d histogram contains non-zero denominator entries.
        Bins that do not have any denominator entries do not correspond to valid RPC panels.
        '''        
        return (self.den_th1.GetBinContent(gbin) > options.min_panel_nden)

    def CountNBins(self, func_name):
        try:
            func = getattr(self, func_name)
        except AttributeError,m:
            msg.error('CountNBins - missing function: %s' %func_name)
            raise AttributeError(m)

        icount = 0
        
        for ibin in range(1, self.nbin1d+1):
            if func(ibin):
                icount += 1

        return icount    

    def IsRef(self):
        return self.is_ref
    
    def GetRun(self):
        return self.run.run_number

    def GetKey(self):
        return self.detkey
    
    def GetRunKey(self):
        return 'Runs/Run_%d/%s' %(self.GetRun(), self.GetKey())
        
    def GetNDenEntries(self, gbin=None):
        if gbin == None:
            return self.den_hist.GetEntries()
        return self.den_th1.GetBinContent(gbin)        

    def GetNNumEntries(self, gbin=None):
        if gbin == None:
            return self.num_hist.GetEntries()
        return self.num_th1.GetBinContent(gbin)

    def GetTotalDenEntriesPerIntLumi(self):
        return self.den_hist.GetEntries()/self.run.GetPhysicsIntLumi()

    def GetTotalNumEntriesPerIntLumi(self):
        return self.num_hist.GetEntries()/self.run.GetPhysicsIntLumi()
    
    def GetNNumEntries(self, gbin=None):
        if gbin == None:
            return self.num_hist.GetEntries()
        return self.num_th1.GetBinContent(gbin)
    
    def GetXAxisName(self, gbin):
        binx, biny = self.GetLocal2Bin(gbin)
        return '#eta=%d' %(binx)

    def GetYAxisName(self, gbin):
        binx, biny = self.GetLocal2Bin(gbin)
        return '#phi=%d' %(biny)

    def GetBinTitle(self, gbin):
        binx, biny = self.GetLocal2Bin(gbin)
        return '#eta=%d #phi=%d' %(binx, biny)

    def GetBinKey(self, gbin):
        binx, biny = self.GetLocal2Bin(gbin)
        return 'eta_%02d_phi_%02d' %(binx, biny)
    
    def IsValidGlobalBin(self, gbin):
        ''' Check that this global bin contains at least 10 denominator entries'''
        return self.den_hist.GetBinContent(gbin) > 1.0
    
    def GetGlobalBin(self, binx, biny):
        return self.den_hist.GetBin(binx, biny)

    def GetLocal2Bin(self, bing):
        binx = ROOT.Long(0)
        biny = ROOT.Long(0)
        binz = ROOT.Long(0)

        self.den_hist.GetBinXYZ(bing, binx, biny, binz)
        return (int(binx), int(biny))

    def AddPosOutlierBin(self, ibin):
        self.pos_outlier_bins += [ibin]

    def AddNegOutlierBin(self, ibin):
        self.neg_outlier_bins += [ibin]
        
    def ConvertToTH1(self, th2, hname):
        '''Convert 2d histogram to flat 1d histogram'''
        
        if th2.Class().GetName().count('TH2') != 1:
            raise Exception('ConverttoTH1 - input is not TH2: name=%s class name=%s' %(th2.GetName(), th2.Class().GetName()))

        msg.debug('ConvertToTH1 - %s' %th2.GetName())
        msg.debug('   nbinx=%d' %self.nbinx)
        msg.debug('   nbiny=%d' %self.nbiny)

        th1 = ROOT.TH1D(hname, th2.GetTitle(), self.nbin1d, 0.5, self.nbin1d+0.5)
        th1.SetDirectory(0)             
        
        for binx in range(1, self.nbinx+1):
            for biny in range(1, self.nbiny+1):
                bing         = self.GetGlobalBin(binx, biny)
                binx_, biny_ = self.GetLocal2Bin(bing)

                if binx_ != binx or biny_ != biny:
                    raise Exception('ConvertToTH1 - logic bin error')

                if not self.IsValidGlobalBin(bing):
                    continue

                val = th2.GetBinContent(binx, biny)
                err = th2.GetBinError  (binx, biny)

                th1.SetBinContent(bing, val)
                th1.SetBinError  (bing, err)

        return th1
                
    def MakeEmptyTH2(self, hname, xtitle=None, ytitle=None, ztitle=None):
        msg.debug('MakeEmptyTH2 - name=%s' %(hname))

        th2 = self.den_hist.Clone(hname)
        th2.SetDirectory(0)
        th2.Reset()

        if xtitle != None:
            th2.GetXaxis().SetTitle(xtitle)
            th2.GetXaxis().CenterTitle()            

        if ytitle != None:
            th2.GetYaxis().SetTitle(ytitle)
            th2.GetYaxis().CenterTitle()

        if ztitle != None:
            th2.GetZaxis().SetTitle(ztitle)
            th2.GetZaxis().CenterTitle()                
            
        return th2

    def ConvertToTH2FromEff(self, th1, hname, fill_with_error=False):
        if th1.Class().GetName().count('TEfficiency') != 1:
            raise Exception('ConverttoTH2 - input is not TEfficiency: name=%s class name=%s' %(th1.GetName(), th1.Class().GetName()))

        msg.debug('ConvertToTH2FromEff - class name=%s: %s' %(th1.Class().GetName(), th1.GetName()))

        th2 = self.MakeEmptyTH2(hname)

        if fill_with_error:
            th2.GetZaxis().SetTitle('#sigma_{stat}(#epsilon_{panel})')
        else:
            th2.GetZaxis().SetTitle('#epsilon_{panel}')
    
        for bing in range(1, self.nbin1d):
            binx, biny = self.GetLocal2Bin(bing)
            bing_      = self.GetGlobalBin(binx, biny)

            if bing_ != bing:
                raise Exception('ConvertToTH2FromEff - logic bin error')

            eff      = th1.GetEfficiency        (bing)
            err_low  = th1.GetEfficiencyErrorLow(bing)
            err_up   = th1.GetEfficiencyErrorUp (bing)
            err_max  = max([err_low, err_up])

            if eff > 0 and False:
                print 'eff     = %f' %eff
                print 'err_low = %f' %err_low
                print 'err_up  = %f' %err_up

            if eff > 0.0:
                if fill_with_error:
                    th2.SetBinContent(binx, biny, err_max)
                else:
                    th2.SetBinContent(binx, biny, eff)

        return th2

    def DrawInputHist(self, wait=False):
        msg.debug('DrawAllHist - %s' %self.detkey)

        can = makeCanvas2d('c2')
        can.cd()
        can.SetGrid()
        
        self.rpc_hist.Draw('AXIG')        
        self.num_hist.Draw('COLZ SAME')
        self.DrawLabel()        
        updateCanvas(can, name='%s/%s' %(self.GetRunKey(), 'rpc_hits_num'), wait=wait)

        self.rpc_hist.Draw('AXIG')
        self.den_hist.Draw('COLZ SAME')
        self.DrawLabel()
        updateCanvas(can, name='%s/%s' %(self.GetRunKey(), 'rpc_hits_den'), wait=wait)

    def DrawEffValHist(self, wait=False):
        msg.debug('DrawEffValHist - %s' %self.detkey)

        can = makeCanvas2d('c2')        
        can.cd()
        can.SetGrid()
        
        self.rpc_hist    .Draw('AXIG')
        self.eff_val_hist.GetZaxis().SetRangeUser(0,1)
        self.eff_val_hist.Draw('COLZ SAME')
        run_text = ', Run # %d' %self.GetRun()
        self.DrawLabel(text=run_text)        
        updateCanvas(can, name='%s/%s' %(self.GetRunKey(), 'rpc_eff'), wait=wait)

    def DrawEffErrHist(self, wait=False):
        msg.debug('DrawEffErrHist - %s' %self.detkey)

        can = makeCanvas2d('c2')        
        can.cd()
        can.SetGrid()
        
        self.rpc_hist    .Draw('AXIG')        
        self.eff_err_hist.Draw('COLZ SAME')
        run_text = ', Run # %d' %self.GetRun()
        self.DrawLabel(text=run_text)        
        updateCanvas(can, name='%s/%s' %(self.GetRunKey(), 'rpc_eff_error'), wait=wait)

        
    def GetTimeDiffInDays(self, other):
        return getTimeDiffInDays(self.run, other.run)

    def PrintEffAsStr(self):
        s  = 'Run #%d int_lumi=%s' %(self.GetRun(), ('%.3f' %self.run.GetPhysicsIntLumi()).rjust(8))
        s += ' Nden=%7d Nnum=%7d' %(self.GetNDenEntries(), self.GetNNumEntries())
        return s

    def DrawLabel(self, x=None, y=None, text=''):

        if x == None: x = 0.10
        if y == None: y = 0.95

        lab = ROOT.TLatex(x, y, self.label + text)
        lab.SetNDC()
        lab.SetTextFont(42)
        lab.SetTextSize(0.045)
        lab.SetTextAlign(12)        
        lab.Draw('SAME')

        self.storage += [lab]
        return lab

#==================================================================================================================
class Panel:

    '''Efficiency of one panel a function of time and lumi for selected runs'''

    def __init__(self, effs, ibin):

        self.ibin        = ibin
        self.key         = None
        self.effs        = effs
        self.storage     = []

        self.xtitle      = None
        self.ytitle      = None        
        
        self.time_graph  = self.CreateTimeGraph(effs, ibin)
        self.time_p0     = None
        self.time_p0_err = None
        self.time_p1     = None
        self.time_p1_err = None
        self.time_chi2   = None
        self.time_ndof   = None
        self.time_status = None        
        
    def CreateTimeGraph(self, effs, ibin, debug=False):

        g = ROOT.TGraphAsymmErrors(len(effs))
        
        for ieff in range(0, len(effs)): 
            eff = effs[ieff]
            
            eff_val, err_low, err_up = eff.GetPanelEff(ibin, do_err=True)
            
            eff_low  = eff_val - err_low
            eff_up   = eff_val + err_up 
            
            xtime = eff.run.GetDeltaTime()
        
            g.SetPoint     (ieff, xtime, eff_val)
            g.SetPointError(ieff, 0.0, 0.0, err_low, err_up)

            if self.xtitle == None:
                self.xtitle = eff.run.GetDeltaTimeTitle()
            if self.ytitle == None:
                self.ytitle = eff.GetBinTitle(ibin)
            if self.key == None:
                self.key = eff.GetBinKey(ibin)
                
        return g

    def GetPanelKey(self):
        return self.key
        
    def FitTime(self):
        rf = self.time_graph.Fit('pol1', 'QS')
                
        if rf.Get():
            self.time_chi2   = rf.Get().Chi2()
            self.time_ndof   = rf.Get().Ndf()
            self.time_status = rf.Status()
            
            self.time_p0     = rf.Get().Parameter(0)
            self.time_p0_err = rf.Get().Error    (0)
            
            self.time_p1     = rf.Get().Parameter(1)
            self.time_p1_err = rf.Get().Error    (1)

            if self.time_ndof > 0:
                self.time_rchi2 = self.time_chi2/float(self.time_ndof)
            else:
                self.time_rchi2 = 0.0

            return self.time_status
        
        msg.warning('FitTime - invalid FitResultPtr')
        return None

    def DrawTimeGraph(self, color, style=None):

        h2 = makeBoxHistFromGraphs([self.time_graph], xtitle=self.xtitle, ytitle=self.ytitle, yscale_top=0.33)
        self.storage += [h2]
        
        setGraphsOptions([self.time_graph], color=color, style=style, xtitle=self.xtitle, ytitle=self.ytitle)

        h2             .Draw('')
        self.time_graph.Draw('P')

        return self.time_graph

#==================================================================================================================
class Layer:

    '''Efficiency of one or more RPC layer for selected runs'''

    def __init__(self, detkey, label, effs):
        self.detkey      = detkey
        self.label       = label
        self.effs        = effs
        self.draw_label  = None

    def GetUniqueRuns(self):
        runs = set()

        for eff in self.effs:
            runs.add(eff.run)

        runs = list(runs)
        runs.sort(key = lambda x: x.start_time)
        
        return runs
        
    def CreateGraphFromFunc(self, eff_func_name, color, style=2, debug=False):        

        if debug:
            msg.info('CreateGraph - len(self.effs) = %d' %len(self.effs))

        runs = self.GetUniqueRuns()

        g = ROOT.TGraph(len(runs))
        g.SetLineColor  (color)
        g.SetLineWidth  (2)
        g.SetMarkerColor(color)
        g.SetMarkerSize (1)
        g.SetMarkerStyle(style)        
            
        for irun in range(0, len(runs)):
            run = runs[irun]
            
            icount = 0
            xtime  = run.GetDeltaTime()
            
            for eff in self.effs:
                if eff.run == run:            
                    icount += eff.CountNBins(eff_func_name)
        
            g.SetPoint(irun, xtime, icount)

            if debug:
                msg.info('%3d: %f, %f' %(irun, xtime, icount))

        return g      

    def GetMeanDenEntriesPerPanelPerIntLumi(self):
        npanel = self.GetLayerNPanels()
        
        if npanel < 1 or len(self.effs) < 1:
            return 0.0
        
        nsum = 0.0

        for eff in self.effs:
            nsum += eff.GetTotalDenEntriesPerIntLumi()

        return nsum/float(len(self.effs)*npanel)

    def GetMeanNumEntriesPerPanelPerIntLumi(self):
        npanel = self.GetLayerNPanels()
        
        if npanel < 1 or len(self.effs) < 1:
            return 0.0
        
        nsum = 0.0

        for eff in self.effs:
            nsum += eff.GetTotalNumEntriesPerIntLumi()

        return nsum/float(len(self.effs)*npanel)
    
    def GetDetKey(self):
        return self.detkey
    
    def GetRefEff(self):
        return findRefEff(self.effs)

    def GetLayerNPanels(self):
        npanel = 0        
        for eff in self.effs:
            npanel = max([npanel, eff.GetNActivePanels()])            
        return npanel

    def GetLayerNGlobalEffBins():
        nbin = 0        
        for eff in self.effs:
            nbin = max([nbin, eff.GetNGlobalEffBins()])            
        return nbin
    
    def RemoveRunsWithLowDenCounts(self):
        npanel = self.GetLayerNPanels()
        
        if npanel < 1:
            return

        del_effs = []

        for eff in self.effs:
            ratio = eff.GetTotalDenEntriesPerIntLumi()/float(npanel)
        
            if ratio < options.min_layer_pp_hits:
                del_effs += [eff]

        if len(del_effs):
            s = '%d run(s) with low #hits per panel per pb^-1' %(len(del_effs))
            msg.info('RemoveRunsWithLowDenCounts - will remove %s' %s)
                
            for eff in del_effs:
                self.effs.remove(eff)
                
                ratio = eff.GetTotalDenEntriesPerIntLumi()/float(npanel)
                msg.info('   %s --> REMOVED RATIO=%f < %.1f' %(eff.PrintEffAsStr(), ratio, options.min_layer_pp_hits))

    def GetDeltaTimeTitle(self):
        for eff in self.effs:
            return eff.run.GetDeltaTimeTitle()
        return None
    
    def DrawEffHists(self):

        for eff in self.effs:
            eff.DrawInputHist (options.draw_all)
            eff.DrawEffValHist(options.draw_eff or options.draw_eff_only)                        
            eff.DrawEffErrHist(options.draw_all)
                
    def DrawLayerLabel(self, x=None, y=None):

        if x == None: x = 0.15
        if y == None: y = 0.95

        if self.draw_label == None:
            lab = ROOT.TLatex(x, y, self.label)
            lab.SetNDC()
            lab.SetTextFont(42)
            lab.SetTextSize(0.047)
            lab.SetTextAlign(12) 
            
            self.draw_label = lab

        self.draw_label.Draw()
        return self.draw_label

#==================================================================================================================
class Quiet:
    '''
    Context manager for silencing certain ROOT operations.  Usage:
    with Quiet(level = ROOT.kInfo+1):
       foo_that_makes_output
    You can set a higher or lower warning level to ignore different
    kinds of messages.  After the end of indentation, the level is set
    back to what it was previously.
    '''
    
    def __init__(self, level = ROOT.kInfo + 1):
        self.level = level
        
    def __enter__(self):
        self.oldlevel = ROOT.gErrorIgnoreLevel        
        ROOT.gErrorIgnoreLevel = self.level
        
    def __exit__(self, type, value, traceback):
        ROOT.gErrorIgnoreLevel = self.oldlevel

#==================================================================================================================
class ExecFunc:
    '''
    Context manager for running global function:
       - function must be defined in this file
       - function must take 2 arguments
       - second argument must be boolean variable
    '''
    
    def __init__(self, arg, name, place=None, wait=False):
        self.arg   = arg
        self.name  = name
        self.place = place
        self.wait  = wait

        if not self.wait:
            func_wait  = False
            func_set   = False
        
            place_wait = False
            place_set  = False        

            if self.place and options.wait_place:
                place_set  = True
                place_wait = re.search(options.wait_place, self.place)
            
            if options.wait_func:
                func_set  = True
                func_wait = re.search(options.wait_func, self.name)           

            if   place_set and func_set: self.wait = place_wait and func_wait
            elif place_set:              self.wait = place_wait
            elif               func_set: self.wait =                func_wait

        if options.wait_all:
            self.wait = True
            
    def __enter__(self):
        msg.info('%s - start execution' %(self.name))
        
        self.start_time = time.time()

        if self.name not in globals():
            raise NameError('ExecFunc - unknown global function: "%s"' %self.name)

        #
        # Execute functon named "name" from this file
        #
        globals()[self.name](self.arg, self.wait)
        
    def __exit__(self, type, value, traceback):        
        msg.info('%s - total time: %.1fs' %(self.name, time.time()-self.start_time))
        
#==================================================================================================================
# Functions below this lone
#==================================================================================================================
def getTimeDiffInDays(t0, t1):

    try:
        time0 = t0.start_time
    except AttributeError:
        time0 = t0

    try:
        time1 = t1.start_time
    except AttributeError:
        time1 = t1

    return (time0 - time1)/(1.0e9*3600.0*24.0)
        
#==================================================================================================================
def getTimeAsStr(timestamp, do_simple=False):

    if do_simple:
        gmt = time.gmtime(timestamp*1.0e-9)
        return '%d-%02d-%02d' %(gmt[0], gmt[1], gmt[2])

    return time.asctime(time.gmtime(timestamp*1.0e-9))

#==================================================================================================================
def updateCanvas(can, name=None, wait=options.wait, logy=False, debug=False):

    '''
    Process current canvas: 
      - set options as requested
      - if requested, update canvas and wait for primitive (click, etc) if requested
      - print canvas if output name is provided
    '''

    if debug:
        msg.info('updateCanvas - canvas=%s, name=%s' %(can.GetName(), name))
    
    if not can:
        msg.info('updateCanvas - current canvas is closed: exit now')
        sys.exit(0)

    curr_mode = None
    curr_logy = None

    do_silent = not options.wait or not wait
    
    if do_silent:
        curr_mode = can.IsBatch()
        can.SetBatch(True)
    
    if logy:
        curr_logy = can.GetLogy()
        can.SetLogy(True)

    if debug:
         msg.info('   do_silent      =%s' %do_silent)
         msg.info('   curr_mode      =%s' %curr_mode)
         msg.info('   curr_logy      =%s' %curr_logy)
         msg.info('   can.IsBatch()  =%s' %can.IsBatch())
        
    can.Modified()
    can.Update()

    if not do_silent:
        if debug:
             msg.info('   wait primitive...')
            
        can.WaitPrimitive()

    if not can:
        msg.info('updateCanvas - current canvas is closed: exit now')        
        sys.exit(0)
                
    if options.save and name != None:
        if debug:
             msg.info('   print canvas')
        
        global nprint_canvas
        nprint_canvas += 1
        
        with Quiet():
            if options.png:
                can.Print('%s.png' %getOutName(name), 'png')
            else:
                can.Print('%s.pdf' %getOutName(name), 'pdf')
        
    if curr_logy != None:
        can.SetLogy(curr_logy)

    if curr_mode != None:
        can.SetBatch(curr_mode)

    if debug:
         msg.info('   all done')
        
#==================================================================================================================
def saveObj(dirpath, name, obj):

    '''Record object into output ROOT file'''

    if options.outfile == None or len(options.outfile) < 6:
        return
    
    global rootfile

    if rootfile == None:
        rootfile = ROOT.TFile(getOutName(options.outfile), 'RECREATE')

    odir = rootfile.GetDirectory(dirpath)

    if not odir:
        rootfile.mkdir(dirpath)

    odir = rootfile.GetDirectory(dirpath)

    if not odir:
        msg.error('saveObj - failed to create output directory: %s' %dirpath)
        return 0

    return odir.WriteObject(obj, name)
        
#==================================================================================================================
def makeCanvas1d(name):
    
    global canvases
    
    try:
        c = canvases[name]
        c.cd()
        c.Clear()
        c.Draw()
        return c
    except KeyError:
        pass

    ROOT.gStyle.SetPadTopMargin   (0.10)
    ROOT.gStyle.SetPadRightMargin (0.10) 
    ROOT.gStyle.SetPadLeftMargin  (0.18)
    ROOT.gStyle.SetPadBottomMargin(0.15)

    can = ROOT.TCanvas(name, name, 850, 550)
    
    if options.logy:
        can.SetLogy(True)

    can.cd()
    can.Draw()
    canvases[name] = can
    
    return makeCanvas1d(name)

#==================================================================================================================
def makeCanvas2d(name):
    
    global canvases

    try:
        c = canvases[name]
        c.cd()
        c.Clear()
        c.Draw()        
        return c
    except KeyError:
        pass

    mt = 0.08
    mb = 0.15

    mr = 0.22    
    ml = 0.15
    
    ROOT.gStyle.SetPadTopMargin   (mt)
    ROOT.gStyle.SetPadBottomMargin(mb)
    
    ROOT.gStyle.SetPadRightMargin (mr) 
    ROOT.gStyle.SetPadLeftMargin  (ml)

    xsize = int(850/(1.0-mr-ml))
    ysize = int(850/(1.0-mt-mb))
    
    can = ROOT.TCanvas(name, name, xsize, ysize)
    
    if options.logy:
        can.SetLogy(True)

    can.Draw()
    canvases[name] = can
    
    return can

#==================================================================================================================
def makeLabel(self, text, x=None, y=None):
        
    if x == None: x = 0.15
    if y == None: y = 0.95

    lab = ROOT.TLatex(x, y, text)
    lab.SetNDC()
    lab.SetTextFont(42)
    lab.SetTextSize(0.04)
    lab.SetTextAlign(12)        
    lab.Draw('SAME')

    return lab

#==================================================================================================================
def makeLegend(x, y, w=0.15, h=0.1):

    leg = ROOT.TLegend(x, y, x+w, y+h)
    leg.SetBorderSize(0)
    leg.SetFillStyle (0)
    leg.SetNColumns(1)
    leg.SetTextFont(42)
    leg.SetTextSize(0.04)
    leg.SetTextAlign(12)        

    return leg

#==================================================================================================================
def getOutName(name):

    if options.pref != None:
        name = '%s_%s' %(options.pref, name)

    if options.post != None:
        name = '%s_%s' %(name, options.post)

        
    if options.outdir != None:
        fullpath = '%s/%s' %(options.outdir.rstrip('/'), name)
    else:
        fullpath = name

    outdir = os.path.dirname (fullpath)

    if len(outdir) > 0:
        if not os.path.isdir(outdir):
            os.makedirs(outdir)

    return fullpath

#==================================================================================================================
def getGoodEffRuns(dirlist):

    '''Read list of data runs input text file and selected good runs'''    
    
    if options.input == None:
        return []

    #----------------------------------------------------------------------------------
    # Read run list from text file, select "good" runs and find input ROOT file for each run
    #    
    runs = []       
    
    for ifile in options.input.split(','):
        if not os.path.isfile(ifile):
            msg.warning('getGoodEffRuns - file does not exist: %s' %ifile)
            continue
        
        runs += getGoodEffRunsForOneFile(ifile, dirlist)


    ilumi         = 0.0
    ilumi_physics = 0.0         

    for run in runs:
        ilumi         += run.int_lumi_online
        ilumi_physics += run.int_lumi_physics        

        run.total_int_lumi = ilumi_physics
        
    #----------------------------------------------------------------------------------
    # Sort runs in time and fill time difference with respect to the first run
    #    
    runs.sort(key = lambda x: x.start_time) # sort runs by time

    for run in runs:
        run.dtime         = getTimeDiffInDays(run, runs[0])
        run.dtime_err_low = 0.0
        run.dtime_err_up  = 0.0
        run.dtime_title   = 'Days since %s' %runs[0].GetSimpleDateAsStr()
    
    #----------------------------------------------------------------------------------
    # Print selected runs
    #        
    msg.info('getGoodEffRuns - selected %d runs with RPC detector efficiency files' %len(runs))

    for r in runs:
        msg.info('   %s' %r.AsStr())

    msg.info('   total integrated luminosity:                    %.1f' %ilumi)
    msg.info('   total integrated luminosity with ATLAS physics: %.1f' %ilumi_physics) 

    return runs

#==================================================================================================================
def getGoodEffRunsForOneFile(filepath, dirlist):

    ifile = open(filepath, 'r')
    runs  = []

    for iline in ifile.readlines():
        if iline.count('#'):
            continue
        
        parts = iline.rstrip('\n').split()
        
        if len(parts) < 6:
            continue

        run = Run(parts)
        
        if options.select_run != None and options.select_run != run.run_number: 
            rejects += ['does not match selected run=%d' %(options.select_run)]
        
        if not run.IsGoodLumiRun(): 
            rejects = ['failed lumi cut']

        ifile = None

        for idir in dirlist:            
            ifile = '%s/panel_eff_run%s.root' %(idir, run.run_number)
        
            if os.path.isfile(ifile):
                break
            else:
                ifile = None

        if ifile != None:
            msg.info(run.AsStr())            
            run.rfile = ROOT.TFile(ifile, 'READ')
            runs += [run]
        else:
            msg.info('%s --> missing input file' %run.AsStr())

    return runs

#==================================================================================================================
def fillHist(h, val):
    xmin = h.GetXaxis().GetXmin()
    xmax = h.GetXaxis().GetXmax()

    if   val < xmin: h.Fill(xmin + 1.0e-9*abs(xmax-xmin))
    elif val > xmax: h.Fill(xmax - 1.0e-9*abs(xmax-xmin))
    else:            h.Fill(val)

#==================================================================================================================
def getEffDiffNSigma(e1, e2, ibin, use_1st_err_only=False):

    val_e1 = e1.GetEfficiency(ibin)
    val_e2 = e2.GetEfficiency(ibin)

    err_low_e1 = e1.GetEfficiencyErrorLow(ibin)
    err_low_e2 = e2.GetEfficiencyErrorLow(ibin)

    err_up_e1 = e1.GetEfficiencyErrorUp(ibin)
    err_up_e2 = e2.GetEfficiencyErrorUp(ibin)

    err_e1 = max([err_low_e1, err_up_e1])
    err_e2 = max([err_low_e2, err_up_e2])

    if use_1st_err_only:
        err = err_e1
    else:
        err = math.sqrt(err_e1*err_e1 + err_e2*err_e2)
    
    if err > 0.0:
        return (val_e1-val_e2)/err

    return None

#==================================================================================================================
def setGraphsOptions(graphs, color, size=2, style=None, xtitle=None, ytitle=None):

    for g in graphs:
        g.Draw('P')
        g.SetLineWidth (size)
        g.SetMarkerSize(size)

        if color:
            g.SetLineColor  (color)
            g.SetMarkerColor(color)

        if style:
            g.SetMarkerStyle(style)            
        
        if xtitle:
            g.GetXaxis().SetTitle(xtitle)
            g.GetXaxis().CenterTitle()

        if ytitle:
            g.GetYaxis().SetTitle(ytitle)
            g.GetYaxis().CenterTitle()

#==================================================================================================================
def copyGraphOptions(g0, g1):

    g0.SetLineWidth (g1.GetLineWidth())
    g0.SetMarkerSize(g1.GetMarkerSize())

    g0.SetLineColor  (g1.GetLineColor())
    g0.SetMarkerColor(g1.GetMarkerColor())
    g0.SetMarkerStyle(g1.GetMarkerStyle())    

    if len(g1.GetXaxis().GetTitle()) > 0:
        g.GetXaxis().SetTitle(g1.GetXaxis().GetTitle())
        g.GetXaxis().CenterTitle()

    if len(g1.GetYaxis().GetTitle()) > 0:
        g.GetYaxis().SetTitle(g1.GetYaxis().GetTitle())
        g.GetYaxis().CenterTitle()
            
#==================================================================================================================
def setHistOptions(hists, xtitle=None, ytitle=None, ztitle=None, color=None):
    
    '''Set histogram options'''

    for h in hists:
    
        h.SetDirectory(0)
        h.SetStats(False)

        if color:
            h.SetLineColor(color)
            h.SetLineWidth(2)
        
        if xtitle:
            h.GetXaxis().SetTitle(xtitle)
            h.GetXaxis().CenterTitle()
            
        if ytitle:
            h.GetYaxis().SetTitle(ytitle)
            h.GetYaxis().CenterTitle()

        if ztitle:
            h.GetZaxis().SetTitle(ztitle)
            h.GetZaxis().CenterTitle()

#==================================================================================================================
def divideGraphs(gnum, gden, debug=False):

    if gnum.GetN() != gden.GetN():
        msg.warning('divideGraphs - unequal size of graphs: %d != %d' %(gnum.GetN(), gden.GetN()))
        return None

    g = ROOT.TGraph(gnum.GetN())
    copyGraphOptions(g, gnum)

    if debug:
        msg.info('divideGraph - N=%d' %g.GetN())
    
    for i in range(0, gnum.GetN()):
        xnum_ = ROOT.Double(0.0)
        ynum_ = ROOT.Double(0.0)

        xden_ = ROOT.Double(0.0)
        yden_ = ROOT.Double(0.0)

        gnum.GetPoint(i, xnum_, ynum_)
        gden.GetPoint(i, xden_, yden_)        

        xnum = float(xnum_)
        xden = float(xden_)
        
        ynum = float(ynum_)
        yden = float(yden_)
        
        if abs(xnum - xden) > 0.0:
            msg.warning('divideGraphs - different x values: %f != %f' %(xnum, xden))
            
        if yden > 0.0:
            g.SetPoint(i, xden, ynum/yden)

            if debug:            
                msg.info('i=%d, xden=%f, ynum=%f, yden=%f, ratio=%f' %(i, xden, ynum, yden, ynum/yden))
        else:
            g.SetPoint(i, xden, 0.0)
            msg.warning('divideGraphs - non-positive denominator: %f' %(yden))

    if debug:            
        for i in range(0, g.GetN()):
            x_ = ROOT.Double(0.0)
            y_ = ROOT.Double(0.0)
            
            g.GetPoint(i, x_, y_)
            gden.GetPoint(i, xden_, yden_)        
            
            x = float(x_)
            y = float(y_)
        
            print 'i=%d, x=%f, y=%f' %(i, x, y)
            
    return g
            
#==================================================================================================================
def makeBoxHistFromGraphs(graphs, xtitle, ytitle, do_err=True, yscale_top=0.05, yscale_bottom=0.05, debug=False):

    '''
    Created 2d histogram that fully encloses the input graphs
    '''

    if debug:
        print '-----------------------------------------------'
        print xtitle
        print ytitle    
    
    xmin = None
    xmax = None
    ymin = None
    ymax = None

    for g in graphs:
        for i in range(0, g.GetN()):
            x_ = ROOT.Double(0.0)
            y_ = ROOT.Double(0.0)

            g.GetPoint(i, x_, y_)

            x = float(x_)
            y = float(y_)
            
            if do_err:
                xerr_low  = g.GetErrorXlow (i)
                xerr_high = g.GetErrorXhigh(i)

                yerr_low  = g.GetErrorYlow (i)
                yerr_high = g.GetErrorYhigh(i)

                x_low  = x - xerr_low
                x_high = x + xerr_high

                y_low  = y - yerr_low
                y_high = y + yerr_high
            else:
                x_low  = x
                x_high = x

                y_low  = y
                y_high = y

            if debug:
                print 'x_low =%f' %x_low
                print 'x_high=%f' %x_high
                print 'y_low =%f' %y_low
                print 'y_high=%f' %y_high

            
            if xmin == None:
                xmin = x_low
                xmax = x_high
                ymin = y_low
                ymax = y_high 
            else:
                xmin = min([x_low,  xmin])
                xmax = max([x_high, xmax])
                ymin = min([y_low,  ymin])
                ymax = max([y_high, ymax])

    if xmin == None:
        return None

    if debug:
        print '----------------'
        print 'do_err=%s' %do_err
        print 'xmin=%s' %xmin
        print 'xmax=%s' %xmax
        print 'ymin=%s' %ymin
        print 'ymax=%s' %ymax
    
    deltax = xmax - xmin
    deltay = ymax - ymin

    if not deltax > 0.0: deltax = 1.0
    if not deltay > 0.0: deltay = 1.0    
    
    xmin = xmin - 0.05*deltax
    xmax = xmax + 0.05*deltax    

    ymin  = ymin - yscale_bottom*deltay
    ymax  = ymax + yscale_top   *deltay
    
    h2 = ROOT.TH2F('box', 'box', 1, xmin, xmax, 1, ymin, ymax)
    h2.SetStats(False)
    h2.SetDirectory(0)

    h2.GetXaxis().SetTitle(xtitle)
    h2.GetYaxis().SetTitle(ytitle)

    h2.GetXaxis().CenterTitle()
    h2.GetYaxis().CenterTitle()

    return h2

#==================================================================================================================
def fillHistFromGraphYValues(graphs, hname, xtitle, ytitle, nbin=25, color=1):

    '''
    Created 1d histogram that fully contains y values of the input graphs
    '''

    h2 = makeBoxHistFromGraphs(graphs, xtitle='', ytitle='', do_err=False, yscale_top=0.0, yscale_bottom=0.0)

    ymin   = h2.GetYaxis().GetXmin()
    ymax   = h2.GetYaxis().GetXmax()
    
    deltay = ymax - ymin    
    ymin1d = ymin - 1.0*deltay/float(nbin - 2)
    ymax1d = ymax + 1.0*deltay/float(nbin - 2)
    
    h1 = ROOT.TH1F(hname, '', nbin, ymin1d, ymax1d)
    h1.SetStats(False)
    h1.SetDirectory(0)

    h1.SetLineColor(color)
    h1.SetLineWidth(2)
    
    h1.GetXaxis().SetTitle(xtitle)
    h1.GetYaxis().SetTitle(ytitle)

    h1.GetXaxis().CenterTitle()
    h1.GetYaxis().CenterTitle()    

    for g in graphs:
        for i in range(0, g.GetN()):
            x = ROOT.Double(0.0)
            y = ROOT.Double(0.0)

            g.GetPoint(i, x, y)
            h1.Fill(y)

    return h1

#==================================================================================================================
def makeBoxHistFromHists(hists, xtitle=None, ytitle=None, do_err=True, yscale_top=0.05, yscale_bottom=0.0, debug=False):

    '''
    Created 2d histogram that fully enclodes the input histograms
    '''

    xmins = []
    xmaxs = []
    ymins = []
    ymaxs = []

    for h in hists:
        xmins += [h.GetXaxis().GetXmin()]
        xmaxs += [h.GetXaxis().GetXmax()]
        
        for i in range(1, h.GetNbinsX()+1):
            val = h.GetBinContent(i)
            err = h.GetBinError  (i)

            if not err > 0.0 or not val > 0.0:
                continue
            
            if do_err:
                ymins += [val - err]
                ymaxs += [val + err]                
            else:
                ymins += [val]
                ymaxs += [val]

        if xtitle == None and len(h.GetXaxis().GetTitle()) > 0:
            xtitle = h.GetXaxis().GetTitle()
            
        if ytitle == None and len(h.GetYaxis().GetTitle()) > 0:
            ytitle = h.GetYaxis().GetTitle()
            
    if len(xmins) == 0 or len(ymins) == 0:
        return None

    xmin = min(xmins)
    xmax = max(xmaxs)

    ymin   = min(ymins)
    ymax   = max(ymaxs)    
    deltay = ymax - ymin
    
    ymin  = ymin - yscale_bottom*deltay
    ymax  = ymax + yscale_top   *deltay
    
    h2 = ROOT.TH2F('box', 'box', 1, xmin, xmax, 1, ymin, ymax)
    setHistOptions([h2], xtitle=xtitle, ytitle=ytitle)

    return h2

#==================================================================================================================
def makeHistFromValues(values, name, nbin, xtitle=None, ytitle=None, color=1, npadx=1):

    '''Create 1d histogram from values'''
    
    xmin = min(values)
    xmax = max(values)
    span = xmax - xmin

    if nbin <= 1 or nbin <= 2*npadx:
        msg.warning('makeHistFromValues - invalid number of bins: %d' %nbin)
        return None
    
    if not span > 0.0:
        span = 1.0

    xmin_axis = xmin - span/float(nbin - 2*npadx)
    xmax_axis = xmax + span/float(nbin - 2*npadx)
    
    h = ROOT.TH1D(name, '', nbin, xmin_axis, xmax_axis)
    setHistOptions([h], xtitle=xtitle, ytitle=ytitle, color=color)

    for v in values:
        h.Fill(v)

    return h

#==================================================================================================================
def getGoodEffRunsForBin(effs, ibin):

    '''Collect runs with valid efficiency measurement for the current bin'''

    good_effs = []

    for eff in effs: 
        if eff.IsGoodDenBin(ibin):
            good_effs += [eff]

    return good_effs

#==================================================================================================================
def findRefEff(effs):
    
    for eff in effs:
        if eff.IsRef():
            return eff

    return None

#==================================================================================================================
def mergeLayers(layers, merge_key, detkey, label):

    msg.info('mergeLayers - merge_key="%s", detley="%s", label="%s"'%(merge_key, detkey, label))
    
    mlayer = Layer(detkey, label, [])

    for layer in layers:
        if re.search(merge_key, layer.GetDetKey()) != None:
            mlayer.effs += layer.effs
            msg.info('   %s  --> ADDED' %layer.GetDetKey())
            
    return mlayer

#==================================================================================================================
def plotPanelEffAsFuncOfTime(effs, ibin, outlier_runs):
    
    '''Plot efficiency of one panel as a function of time and instantenous luminosity'''

    if len(effs) < 2: # need at least two runs to make plots
        return

    msg.debug('plotPanelEffAsFuncOfTime - plot %d efficiency runs for bin=%d' %(len(effs), ibin))
    
    gtime     = ROOT.TGraphAsymmErrors(len(effs))
    gtime_ref = ROOT.TGraphAsymmErrors(1)
    gtime_out = ROOT.TGraphAsymmErrors(len(outlier_runs))

    ytitle      = '#epsilon_{panel}(%s %s)' %(effs[0].GetXAxisName(ibin), effs[0].GetYAxisName(ibin))
    xtitle_time = effs[0].run.GetDeltaTimeTitle()

    for ieff in range(0, len(effs)): 
        eff = effs[ieff]
        
        eff_val, err_low, err_up = eff.GetPanelEff(ibin, do_err=True)

        eff_low  = eff_val - err_low
        eff_up   = eff_val + err_up 

        xtime = eff.run.GetDeltaTime()
        
        gtime.SetPoint     (ieff, xtime, eff_val)
        gtime.SetPointError(ieff, 0.0, 0.0, err_low, err_up)

        if eff.IsRef():
            gtime_ref.SetPoint     (ieff, xtime, eff_val)
            gtime_ref.SetPointError(ieff, 0.0, 0.0, err_low, err_up)

        if eff.GetRun() in outlier_runs:
            gtime_out.SetPoint      (ieff, xtime, eff_val)
            gtime_out.SetPointError(ieff, 0.0, 0.0, err_low, err_up)

    h2 = makeBoxHistFromGraphs([gtime], xtitle_time, ytitle)
    
    setGraphsOptions([gtime,   ], ROOT.kBlack, xtitle=xtitle_time, ytitle=ytitle)
    setGraphsOptions([gtime_ref], ROOT.kBlue,  xtitle=xtitle_time, ytitle=ytitle)
    setGraphsOptions([gtime_out], ROOT.kRed,   xtitle=xtitle_time, ytitle=ytitle)

    oname = 'References/outlier_panel_%s' %(effs[0].GetBinKey(ibin))
    
    c = makeCanvas1d('c1')
    h2       .Draw('')
    gtime    .Draw('PSAME')
    gtime_ref.Draw('PSAME')
    gtime_out.Draw('PSAME')
    effs[0]  .DrawLabel()
    updateCanvas(c, name=oname, wait=True)

    msg.debug('plotPanelEffAsFuncOfTime - all done')
    
#==================================================================================================================
def plotEffRef(layer, wait=False, nsigma=options.min_out_nsigma):
    
    '''Plot panel efficiency for different runs with respect to one reference run'''

    if len(layer.effs) < 2:
        msg.warning('plotEffRef - too few efficiency entries')
        return

    eff_ref = layer.GetRefEff()

    if eff_ref == None:
        msg.warning('plotEffRef - missing reference efficiency run')
        return

    #----------------------------------------------------------------------------------
    # Book histograms
    #    
    max_abs = 0.5
    max_sig = 5.0

    habs = ROOT.TH1D('ref_diff_abs', '', 200, -max_abs, max_abs)
    hsig = ROOT.TH1D('ref_diff_sig', '', 200, -max_sig, max_sig)

    habs.GetXaxis().SetTitle('#epsilon_{panel} - #epsilon_{panel}^{Run %d}'                 %(eff_ref.GetRun()))
    hsig.GetXaxis().SetTitle('(#epsilon_{panel} - #epsilon_{panel}^{Run %d})/#sigma_{stat}' %(eff_ref.GetRun()))

    habs.GetYaxis().SetTitle('panels/runs')
    hsig.GetYaxis().SetTitle('panels/runs')

    habs.GetXaxis().CenterTitle()
    habs.GetYaxis().CenterTitle()

    hsig.GetXaxis().CenterTitle()
    hsig.GetYaxis().CenterTitle()

    habs.SetStats(False)
    hsig.SetStats(False)

    habs.SetDirectory(0)
    hsig.SetDirectory(0)

    #----------------------------------------------------------------------------------
    # Find panels/runs with statistically signficant deviation with respect to reference run
    #
    outliers = {}

    for eff in layer.effs:        
        if eff.IsRef():
            continue

        hpos = eff.MakeEmptyTH2('outliers_pos_with_nsigma_gt%d' %(nsigma), ztitle= '#epsilon_{panel}-#epsilon_{panel}^{%d}'  %eff_ref.GetRun())
        hneg = eff.MakeEmptyTH2('outliers_neg_with_nsigma_gt%d' %(nsigma), ztitle='|#epsilon_{panel}-#epsilon_{panel}^{%d}|' %eff_ref.GetRun())
        
        for ibin in range(1, eff_ref.GetNGlobalEffBins()):
            if not eff_ref.IsGoodDenBin(ibin):
                continue        

            if not eff.IsGoodDenBin(ibin):
                msg.debug('plotEffRef - bin=%d has good denominator for reference but not for current run:' %(ibin))
                msg.debug('Reference: %s - nden[%d]=%f' %(eff_ref.PrintEffAsStr(),ibin,  eff_ref.GetNDenEntries(ibin)))
                msg.debug('           %s - nden[%d]=%f' %(eff    .PrintEffAsStr(),ibin,  eff    .GetNDenEntries(ibin)))
                continue
                
            val_eff_cur = eff    .GetPanelEff(ibin)
            val_eff_ref = eff_ref.GetPanelEff(ibin)        

            if not (val_eff_ref + val_eff_cur > 0.0):
                msg.debug('Invalid efficiency reference for bin=%d' %ibin)
                continue

            diff_eff = (val_eff_cur - val_eff_ref)
            
            fillHist(habs, diff_eff)
                
            diff_eff_nsig = getEffDiffNSigma(eff_ref.eff_1d, eff.eff_1d, ibin, use_1st_err_only=False)
            
            if diff_eff_nsig == None:
                continue
            
            fillHist(hsig, diff_eff_nsig)

            if abs(diff_eff_nsig) > nsigma:

                binx, biny = eff.GetLocal2Bin(ibin)

                if diff_eff > 0.0:
                    hpos.SetBinContent(binx, biny,     diff_eff)
                    eff.AddPosOutlierBin(ibin)                    
                else:
                    hneg.SetBinContent(binx, biny, abs(diff_eff))
                    eff.AddNegOutlierBin(ibin)
                    
                try:
                    outliers[ibin] += [eff.GetRun()]
                except KeyError:
                    outliers[ibin]  = [eff.GetRun()]
                    
                ss = ('%.1f' %diff_eff_nsig).rjust(5)
                msg.debug('Run #%d - nsigma=%s, binx=%2d, biny=%2d' %(eff.GetRun(), ss, binx, biny))

        c2 = makeCanvas2d('c2')

        lpos = ' - #uparrow outliers'
        lneg = ' - #downarrow outliers'

        eff.rpc_hist.Draw('AXIG')
        hpos.Draw('COLZ2 SAME')
        eff.DrawLabel(text=lpos)
        updateCanvas(c2, name='%s/%s' %(eff.GetRunKey(), hpos.GetName()), wait=False)

        eff.rpc_hist.Draw('AXIG')
        hneg.Draw('COLZ2 SAME')
        eff.DrawLabel(text=lneg)
        updateCanvas(c2, name='%s/%s' %(eff.GetRunKey(), hneg.GetName()), wait=False)
        
    #----------------------------------------------------------------------------------
    # Fill 2d histograms with location of outlier panels.
    #                    
    c = makeCanvas1d('c1')

    ROOT.gStyle.SetOptStat('')
    hsig.SetStats(True)
    hsig.Fit('gaus', 'QS', '', -(max_sig-0.5), +(max_sig-0.5))
    
    for h in [habs, hsig]:    
        h.Draw()
        layer.DrawLayerLabel()
        updateCanvas(c, name='%s/%s' %(layer.GetDetKey(), h.GetName()), wait=wait)

    msg.info('Process %d outlier panels' %len(outliers))

    for iout in sorted(outliers.keys()):
        outlier_runs = outliers[iout]
        plotPanelEffAsFuncOfTime(layer.effs, iout, outlier_runs=outlier_runs)

#==================================================================================================================
def plotEffDiffEtaPhi(strip_layers, wait=False, bad_eff=options.bad_eff):
    
    '''Plot panel efficiency difference between eta and phi strips for the same run'''
    msg.debug('*********************************************************plotEffDiffEtaPhi function beginning')
    if len(strip_layers) != 2:
        msg.warning('plotEffDiffEtaPhi - not exactly two efficiency entries (I need one for phi, one for eta)')
        return
    
    try:
        layer_eta = strip_layers['eta']
        layer_phi = strip_layers['phi']
    except KeyError:
        msg.warning('plotEffDiffEtaPhi - eta and/or phi layer is missing')
        return
        
    for eff_phi in layer_phi.effs:
        eff_eta = None
        
        for eff in layer_eta.effs:
            if eff.GetRun() == eff_phi.GetRun():
                eff_eta = eff
                break
        
        if eff_eta == None:
            msg.info('plotEffDiffEtaPhi - run %d is missing matching eta run')
            continue
        #
        # Found eta and phi efficiencis for same ATLAS run and for same layer, doublet, gas gap
        #
        #define histograms
        hdif = eff_eta.MakeEmptyTH2('rpc_eff_eta_phi_diff', ztitle= '|#epsilon_{#eta strip}-#epsilon_{#phi strip}|')
        hdif_nsigma = eff_eta.MakeEmptyTH2('rpc_eff_eta_phi_diff_nsigma', ztitle= '|#epsilon_{#eta strip}-#epsilon_{#phi strip}| if > 3#sigma_{stat}')
        handave = eff_eta.MakeEmptyTH2('rpc_eff_average_bad_eta_and_phi', ztitle= '(#epsilon_{#eta strip}+#epsilon_{#phi strip})/2' )
        horave = eff_eta.MakeEmptyTH2('rpc_eff_average_bad_eta_or_phi', ztitle= '(#epsilon_{#eta strip}+#epsilon_{#phi strip})/2')
        handworse = eff_eta.MakeEmptyTH2('rpc_eff_worse_bad_eta_and_phi', ztitle= '#epsilon_{#eta strip} or #epsilon_{#phi strip} (smaller one)')
        horworse = eff_eta.MakeEmptyTH2('rpc_eff_worse_bad_eta_or_phi', ztitle= '#epsilon_{#eta strip} or #epsilon_{#phi strip} (smaller one) ')
        horminusandworse = eff_eta.MakeEmptyTH2('rpc_eff_worse_bad_eta_or_phi_minus_and', ztitle= '#epsilon_{#eta strip} or #epsilon_{#phi strip}')
        
        #separately for eta and phi efficiencies on Z axis
        heta_and = eff_eta.MakeEmptyTH2('rpc_eta_eff_bad_eta_and_phi', ztitle= '#epsilon^{#eta strip}_{panel}')
        heta_or = eff_eta.MakeEmptyTH2('rpc_eta_eff_bad_eta_or_phi', ztitle= '#epsilon^{#eta strip}_{panel}')
        heta_or_minus_and = eff_eta.MakeEmptyTH2('rpc_eta_eff_bad_eta_or_phi_minus_and', ztitle= '#epsilon^{#eta strip}_{panel}')
        hphi_and = eff_eta.MakeEmptyTH2('rpc_phi_eff_bad_eta_and_phi', ztitle= '#epsilon^{#phi strip}_{panel}')
        hphi_or = eff_eta.MakeEmptyTH2('rpc_phi_eff_bad_eta_or_phi', ztitle= '#epsilon^{#phi strip}_{panel}')
        hphi_or_minus_and = eff_eta.MakeEmptyTH2('rpc_phi_eff_bad_eta_or_phi_minus_and', ztitle= '#epsilon^{#phi strip}_{panel}')
    
        handave.GetZaxis().SetRangeUser(0,bad_eff)
        horave.GetZaxis().SetRangeUser(0,bad_eff)
        handworse.GetZaxis().SetRangeUser(0,bad_eff)
        horworse.GetZaxis().SetRangeUser(0,bad_eff)
        horminusandworse.GetZaxis().SetRangeUser(0,bad_eff)
        heta_and.GetZaxis().SetRangeUser(0,bad_eff)
        heta_or.GetZaxis().SetRangeUser(0,bad_eff)
        heta_or_minus_and.GetZaxis().SetRangeUser(0,bad_eff)
        hphi_and.GetZaxis().SetRangeUser(0,bad_eff)
        hphi_or.GetZaxis().SetRangeUser(0,bad_eff)
        hphi_or_minus_and.GetZaxis().SetRangeUser(0,bad_eff)
        
        for ibin in range(1, eff_eta.GetNGlobalEffBins()):
            if not eff_eta.IsGoodDenBin(ibin):
                continue
            if not eff_phi.IsGoodDenBin(ibin):
                continue
            val_eff_eta = eff_eta.GetPanelEff(ibin)
            val_eff_phi = eff_phi.GetPanelEff(ibin)
        
            if not (val_eff_eta + val_eff_phi > 0.0 ):
                msg.debug('Invalid efficiency reference for bin=%d' %ibin)
                continue
                
            #compute absolute difference between the two efficiencies
            diff_eff = abs(val_eff_eta - val_eff_phi)
            binx, biny = eff_eta.GetLocal2Bin(ibin)
            hdif.SetBinContent(binx, biny, diff_eff)
            
            diff_eff_nsig = getEffDiffNSigma(eff_eta.eff_1d, eff_phi.eff_1d, ibin, use_1st_err_only=False)
            if (abs(diff_eff_nsig) > 3.0):
                hdif_nsigma.SetBinContent(binx, biny, diff_eff)
        
            #compute average efficiency of the two when bad eta and bad phi (below 50%)
            average_eff = (val_eff_eta + val_eff_phi) / 2 
            if (val_eff_eta < bad_eff and val_eff_phi < bad_eff):
                heta_and.SetBinContent(binx, biny, val_eff_eta)
                hphi_and.SetBinContent(binx, biny, val_eff_phi)
                handave.SetBinContent(binx, biny, average_eff)
                if (val_eff_eta < val_eff_phi):
                    handworse.SetBinContent(binx, biny, val_eff_eta)
                else: 
                    handworse.SetBinContent(binx, biny, val_eff_phi)
            if (val_eff_eta < bad_eff or val_eff_phi < bad_eff):
                if (val_eff_eta < bad_eff):
                    heta_or.SetBinContent(binx, biny, val_eff_eta)
                if (val_eff_phi < bad_eff):
                    hphi_or.SetBinContent(binx, biny, val_eff_phi)
                horave.SetBinContent(binx, biny, average_eff)
                if (val_eff_eta < val_eff_phi):
                    if (val_eff_eta < bad_eff and val_eff_phi < bad_eff):
                        pass
                    else:
                        if (val_eff_eta < bad_eff):
                            heta_or_minus_and.SetBinContent(binx, biny, val_eff_eta)
                        if (val_eff_phi < bad_eff): 
                            hphi_or_minus_and.SetBinContent(binx, biny, val_eff_phi)
                    horworse.SetBinContent(binx, biny, val_eff_eta)
                    if (val_eff_eta < bad_eff and val_eff_phi < bad_eff):
                        pass
                    else: 
                        horminusandworse.SetBinContent(binx, biny, val_eff_eta)
                else: 
                    if (val_eff_eta < bad_eff and val_eff_phi < bad_eff):
                        pass
                    else:
                        if (val_eff_eta < bad_eff):
                            heta_or_minus_and.SetBinContent(binx, biny, val_eff_eta)
                        if (val_eff_phi < bad_eff): 
                            hphi_or_minus_and.SetBinContent(binx, biny, val_eff_phi)
                    horworse.SetBinContent(binx, biny, val_eff_phi)
                    if (val_eff_eta < bad_eff and val_eff_phi < bad_eff):
                        pass
                    else:
                        horminusandworse.SetBinContent(binx, biny, val_eff_phi)
        
        eff_eta.label = strip_layers['eta'].label
        hdif.GetZaxis().SetRangeUser(0,1)
        hdif_nsigma.GetZaxis().SetRangeUser(0,1)
        plotEffDiffOneEtaPhi(hdif, eff_eta, ', #eta-#phi diff.', wait=False)
        plotEffDiffOneEtaPhi(hdif_nsigma, eff_eta, ', #eta-#phi diff.', wait=False)
        plotEffDiffOneEtaPhi(handave, eff_eta, ', bad #eta and #phi', wait=False)
        plotEffDiffOneEtaPhi(horave, eff_eta, ', bad #eta or #phi', wait=False)
        plotEffDiffOneEtaPhi(handworse, eff_eta, ', bad #eta and #phi', wait=False)
        plotEffDiffOneEtaPhi(horworse, eff_eta, ', bad #eta or #phi', wait=False)
        plotEffDiffOneEtaPhi(horminusandworse, eff_eta, ', bad #eta or #phi -and', wait=False)
        
        plotEffDiffOneEtaPhi(heta_and, eff_eta, ', bad #eta and #phi', wait=False)
        plotEffDiffOneEtaPhi(heta_or, eff_eta, ', bad #eta or #phi', wait=False)
        plotEffDiffOneEtaPhi(heta_or_minus_and, eff_eta, ', bad #eta or #phi -and', wait=False)
        plotEffDiffOneEtaPhi(hphi_and, eff_eta, ', bad #eta and #phi', wait=False)
        plotEffDiffOneEtaPhi(hphi_or, eff_eta, ', bad #eta or #phi', wait=False)
        plotEffDiffOneEtaPhi(hphi_or_minus_and, eff_eta, ', bad #eta or #phi -and', wait=False)
    
#==================================================================================================================
def plotEffDiffOneEtaPhi(histo, eff_eta, histo_name, wait=options.wait):
    
    '''Plot efficiency difference/other between phi and eta for the same run, layer, doublete and gas gap'''
    
    can1 = makeCanvas2d('can1')
    can1.SetGrid()
    eff_eta.rpc_hist.Draw('AXIG')
    histo.Draw('COLZ2 SAME')
    run_number_text=', Run # %d' %eff_eta.GetRun()
    complete_text=histo_name+run_number_text
    eff_eta.DrawLabel(text=complete_text)
    updateCanvas(can1, name='%s/%s' %(eff_eta.GetRunKey(), histo.GetName()), wait=False)

#==================================================================================================================
def plotTotalCountsPerLayer(layer, wait=True, do_print=False):
    
    '''Plot total number of numerator and denominator hits per RPC layer'''

    if layer.GetLayerNPanels() < 1:
        msg.info('plotTotalCountsPerLayer - %s has too few %d active panels' %(layer.GetDetKey(), layer.GetLayerNPanels()))
        return    
    
    msg.info('plotTotalCountsPerLayer - plot numerator and denominator hits per pb^-1 as a function of time')
    msg.info('   %s has %d active panels' %(layer.GetDetKey(), layer.GetLayerNPanels()))

    gnum    = ROOT.TGraphAsymmErrors(len(layer.effs))
    gnum_pp = ROOT.TGraphAsymmErrors(len(layer.effs))    
    gden    = ROOT.TGraphAsymmErrors(len(layer.effs))
    gden_pp = ROOT.TGraphAsymmErrors(len(layer.effs))

    setGraphsOptions([gnum, gnum_pp], color=2, size=1, style=31)
    setGraphsOptions([gden, gden_pp], color=4, size=1, style=2)

    scale_pp = 1.0/float(layer.GetLayerNPanels())

    time_xtitle = None
    
    for ieff in range(0, len(layer.effs)):
        eff = layer.effs[ieff]

        dtime = eff.run.GetDeltaTime()
        ynum  = eff.GetTotalNumEntriesPerIntLumi()
        yden  = eff.GetTotalDenEntriesPerIntLumi()

        gnum   .SetPoint(ieff, dtime, ynum)
        gnum_pp.SetPoint(ieff, dtime, ynum*scale_pp)
        
        gden   .SetPoint(ieff, dtime, yden)
        gden_pp.SetPoint(ieff, dtime, yden*scale_pp)

        if time_xtitle == None:
            time_xtitle = eff.run.GetDeltaTimeTitle()
        
        if do_print:
            msg.info('dtime=%3d, ynum=%s, yden=%s' %(int(dtime), ('%.0f' %ynum).rjust(5), ('%.0f' %yden).rjust(5)))

    c = makeCanvas1d('c1')
    
    #----------------------------------------------------------------------------------    
    h2 = makeBoxHistFromGraphs([gnum, gden], time_xtitle, 'Layer hits per pb^{-1}',
                                do_err=False, yscale_top=0.3, yscale_bottom=0.1)

    h2.Draw('')
    gnum.Draw('P')
    gden.Draw('P')
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.70, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(gnum, 'numerator',   'p')
    leg.AddEntry(gden, 'denominaror', 'p')
    leg.Draw('SAME')
    
    updateCanvas(c, name = '%s/%s' %(layer.GetDetKey(), 'layer_hits_vs_time'), wait=wait)

    #----------------------------------------------------------------------------------    
    h2 = makeBoxHistFromGraphs([gnum_pp, gden_pp], time_xtitle, 'Layer hits per panel per pb^{-1}',
                                do_err=False, yscale_top=0.3, yscale_bottom=0.1)

    h2.Draw('')
    gnum_pp.Draw('P')
    gden_pp.Draw('P')
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.70, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(gnum_pp, 'numerator',   'p')
    leg.AddEntry(gden_pp, 'denominaror', 'p')
    leg.Draw('SAME')
    
    updateCanvas(c, name = '%s/%s' %(layer.GetDetKey(), 'layer_hits_vs_time_per_panel'), wait=wait)
    
    #----------------------------------------------------------------------------------
    hnum = fillHistFromGraphYValues([gnum_pp], 'hnum', xtitle='Numerator hits per pb^{-1}',   ytitle='Runs', nbin=50, color=2)
    hden = fillHistFromGraphYValues([gden_pp], 'hden', xtitle='Denominator hits per pb^{-1}', ytitle='Runs', nbin=50, color=4)

    hnum.Draw('')
    updateCanvas(c, name = '%s/%s' %(layer.GetDetKey(), 'layer_hits_num_per_panel'), wait=False)
    
    hden.Draw('')
    updateCanvas(c, name = '%s/%s' %(layer.GetDetKey(), 'layer_hits_den_per_panel'), wait=False)

    #----------------------------------------------------------------------------------    
    saveObj('%s/' %layer.GetDetKey(), 'hits_num_vs_time',           gnum)
    saveObj('%s/' %layer.GetDetKey(), 'hits_num_vs_time_per_panel', gnum_pp)
    
    saveObj('%s/' %layer.GetDetKey(), 'hits_den_vs_time',           gden)
    saveObj('%s/' %layer.GetDetKey(), 'hits_den_vs_time_per_panel', gden_pp)

#==================================================================================================================
def plotLayersTotalCounts(layers, wait=False):
    
    '''Plot total number of numerator and denominator hits per RPC layer per panel for all layers and all runs'''

    if len(layers) < 1:
        msg.warning('plotLayersTotalCounts - too few %d layers' %len(layers))
        return

    list_ynum_pp = []
    list_yden_pp = []    
    
    for layer in layers:    
        scale_pp = 1.0/float(layer.GetLayerNPanels())
    
        for ieff in range(0, len(layer.effs)):
            eff = layer.effs[ieff]

            dtime = eff.run.GetDeltaTime()
            ynum  = eff.GetTotalNumEntriesPerIntLumi()
            yden  = eff.GetTotalDenEntriesPerIntLumi()

            list_ynum_pp += [ynum*scale_pp]
            list_yden_pp += [yden*scale_pp]

    nbin   = int(math.sqrt(len(list_ynum_pp)))
    ytitle = 'Detector layers/Data runs'
    xtitle = 'Mean hits per panel per pb^{-1}'

    hnum = makeHistFromValues(list_ynum_pp, 'all_layers_hits_num_per_panel', nbin, xtitle=xtitle, ytitle=ytitle, color=2)    
    hden = makeHistFromValues(list_yden_pp, 'all_layers_hits_den_per_panel', nbin, xtitle=xtitle, ytitle=ytitle, color=4)

    h2 = makeBoxHistFromHists([hden, hnum], yscale_top=0.30)
    
    c = makeCanvas1d('c1')

    h2.Draw()
    hnum.Draw('SAME')
    hden.Draw('SAME')

    leg = makeLegend(x=0.64, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(hnum, 'numerator')
    leg.AddEntry(hden, 'denominator')
    leg.Draw('SAME')    
    
    updateCanvas(c, name='hits_per_panel_per_inv_pb', wait=wait)
    
#==================================================================================================================
def plotSinglePanelEffs(layer, wait=False):
    
    '''Plot individual panel efficiency as a function of time and luminosity'''

    eff_ref = layer.GetRefEff()
    
    if eff_ref == None:
        return None

    hstatus    = ROOT.TH1D('fit_panel_eff_vs_time_status',       '',   2, -0.5,   1.5)
    hchi2      = ROOT.TH1D('fit_panel_eff_vs_time_chi2',         '', 100,  0.0,  10.0)
    hchi2_good = ROOT.TH1D('fit_panel_eff_vs_time_chi2_goodfit', '', 100,  0.0,  10.0)
    
    hp0        = ROOT.TH1D('fit_panel_eff_vs_time_p0',           '', 100,  0.05, 1.05)
    hp0_good   = ROOT.TH1D('fit_panel_eff_vs_time_p0_goodfit',   '', 100,  0.05, 1.05)
    
    hp1        = ROOT.TH1D('fit_panel_eff_vs_time_p1',           '', 100, -0.001, 0.001)
    hp1_good   = ROOT.TH1D('fit_panel_eff_vs_time_p1_goodfit',   '', 100, -0.001, 0.001)

    setHistOptions([hstatus],    'Time fit status',            'Panels')
    setHistOptions([hchi2],      '#chi^{2}/ndof for time fit', 'Panels')
    setHistOptions([hchi2_good], '#chi^{2}/ndof for time fit', 'Good fit panels', color=4)
    
    setHistOptions([hp0],        'Time fit parameter 0 [#epsilon_{panel} (#Deltat = 0)]', 'Panels')
    setHistOptions([hp0_good],   'Time fit parameter 0 [#epsilon_{panel} (#Deltat = 0)]',  'Panels', color=4)
    
    setHistOptions([hp1],        'Time fit parameter 1 [#epsilon_{panel} vs. #Deltat slope]', 'Panels')
    setHistOptions([hp1_good],   'Time fit parameter 1 [#epsilon_{panel} vs. #Deltat slope]', 'Panels', color=4)
    
    can = makeCanvas1d('c1')

    panels_all  = []
    panels_good = []
    
    for ibin in range(1, eff_ref.GetNGlobalEffBins()+1):
        good_effs = getGoodEffRunsForBin(layer.effs, ibin)

        if len(good_effs) == 0:
            continue

        p = Panel(good_effs, ibin)
        outdir = None

        if p.FitTime() == 0:
            fillHist(hstatus, 0.0)
            fillHist(hchi2,   p.time_rchi2)
            fillHist(hp0,     p.time_p0)
            fillHist(hp1,     p.time_p1)

            panels_all += [p]
            
            if p.time_rchi2 < options.max_good_panel_chi2:
                outdir = 'panel_eff_fit_good_chi2'
                panels_good += [p]
            
                fillHist(hchi2_good, p.time_rchi2)
                fillHist(hp0_good,   p.time_p0)
                fillHist(hp1_good,   p.time_p1)            
            else:
                outdir = 'panel_eff_fit_bad_chi2'
        else:
            outdir = 'panel_eff_fit_bad_fit'
            fillHist(hstatus, 1.0)
            msg.warning('plotSinglePanelEffs - fit failed for bin=%d' %ibin)

        if options.draw_panels:
            outname = '%s/%s/panel_eff_vs_time_%s' %(layer.GetDetKey(), outdir, p.GetPanelKey())
            
            p.DrawTimeGraph(color=1)
            layer.DrawLayerLabel()
            updateCanvas(can, name=outname, wait=False)
            
    msg.info('plotSinglePanelEffs - print summary')
    msg.info('   N of all  panel fits: %d' %len(panels_all))
    msg.info('   N of good panel fits: %d' %len(panels_good))
    msg.info('   Mean p0 for good fits: %f' %hp0_good.GetMean())
    msg.info('   Mean p1 for good fits: %f' %hp1_good.GetMean())    

    #----------------------------------------------------------------------------------    
    hstatus.Draw()
    layer.DrawLayerLabel()
    updateCanvas(can, name='%s/%s' %(layer.GetDetKey(), hstatus.GetName()), wait=wait, logy=True)

    #----------------------------------------------------------------------------------
    h2 = makeBoxHistFromHists([hchi2, hchi2_good], yscale_top=0.30)
    
    h2        .Draw()
    hchi2     .Draw('SAME')
    hchi2_good.Draw('SAME')
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.64, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(hchi2,      'all fits',                                                  'l')
    leg.AddEntry(hchi2_good, 'fits with #chi^{2}/ndof<%.1f' %options.max_good_panel_chi2, 'l')
    leg.Draw('SAME')
    
    updateCanvas(can, name='%s/%s' %(layer.GetDetKey(), hchi2.GetName()), wait=wait)

    #----------------------------------------------------------------------------------
    h2 = makeBoxHistFromHists([hp0, hp0_good], yscale_top=0.30)

    h2      .Draw()
    hp0     .Draw('SAME')
    hp0_good.Draw('SAME')
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.64, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(hp0,      'all fits',                                                  'l')
    leg.AddEntry(hp0_good, 'fits with #chi^{2}/ndof<%.1f' %options.max_good_panel_chi2, 'l')
    leg.Draw('SAME')
    
    updateCanvas(can, name='%s/%s' %(layer.GetDetKey(), hp0.GetName()), wait=wait)

    #----------------------------------------------------------------------------------
    h2 = makeBoxHistFromHists([hp1, hp1_good], yscale_top=0.30)

    h2      .Draw()    
    hp1     .Draw('SAME')
    hp1_good.Draw('SAME')
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.64, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(hp1,      'all fits',                                                  'l')
    leg.AddEntry(hp1_good, 'fits with #chi^{2}/ndof<%.1f' %options.max_good_panel_chi2, 'l')
    leg.Draw('SAME')
    
    updateCanvas(can, name='%s/%s' %(layer.GetDetKey(), hp1.GetName()), wait=wait)

#==================================================================================================================
def plotAllPanelEffs(layer, wait=False):
    
    '''Plot efficiency for all panels and all runs'''

    heff_all  = ROOT.TH1D('panel_eff_all',  '', 525, 0.0, 1.05)
    heff_live = ROOT.TH1D('panel_eff_live', '', 525, 0.0, 1.05)
    heff_high = ROOT.TH1D('panel_eff_high', '', 525, 0.0, 1.05)

    setHistOptions([heff_all] , '#epsilon_{panel}', 'All panels')
    setHistOptions([heff_live], '#epsilon_{panel}', 'Live panels')
    setHistOptions([heff_high], '#epsilon_{panel}', 'High panels')
    
    for eff in layer.effs:
        msg.debug(eff.PrintEffAsStr())

        for gbin in range(0, eff.GetNGlobalEffBins()):
            if not eff.IsGoodDenBin(gbin):
                continue

            panel_eff = eff.GetPanelEff(gbin)
            
            heff_all.Fill(panel_eff)

            if eff.IsGoodDenBin(gbin):
                heff_all.Fill(panel_eff)
            else:
                continue
                
            if eff.IsLiveEffBin(gbin):
                heff_live.Fill(panel_eff)

            if eff.IsHighEffBin(gbin):
                heff_high.Fill(panel_eff)

    c = makeCanvas1d('c1')
    heff_live.Draw()
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/%s' %(layer.GetDetKey(), heff_live.GetName()), wait=wait)

#==================================================================================================================
def plotPanelCounts(layer, wait=False):
    
    '''
    Plot efficiency bins for nearly dead panels: 
    denominator entries present but no numerator entries
    '''

    graph_good_den = layer.CreateGraphFromFunc('IsGoodDenBin', 1)
    graph_live_eff = layer.CreateGraphFromFunc('IsLiveEffBin', 4)
    graph_low_eff  = layer.CreateGraphFromFunc('IsLowEffBin',  2)
    graph_high_eff = layer.CreateGraphFromFunc('IsHighEffBin', ROOT.kViolet, style=21)
    graph_poutlier = layer.CreateGraphFromFunc('IsPosOutlierBin', 4)
    graph_noutlier = layer.CreateGraphFromFunc('IsNegOutlierBin', 2, style=3)

    rgraph_live_eff = divideGraphs(graph_live_eff, graph_good_den)
    rgraph_high_eff = divideGraphs(graph_high_eff, graph_good_den)
    rgraph_low_eff  = divideGraphs(graph_low_eff,  graph_good_den)
    
    c = makeCanvas1d('c1')

    xtitle = layer.GetDeltaTimeTitle()
    
    #----------------------------------------------------------------------------------    
    h2 = makeBoxHistFromGraphs([graph_good_den], xtitle, 'N. of active panels',
                               do_err=False, yscale_bottom=0.1, yscale_top=0.1)

    h2.Draw()
    graph_good_den.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/counts_vs_time_active' %(layer.GetDetKey()), wait=wait)

    #----------------------------------------------------------------------------------
    ytitle = 'N. of panels with #epsilon > %0.2f' %options.min_high_panel_eff
    
    h2 = makeBoxHistFromGraphs([graph_high_eff], xtitle, ytitle,
                               do_err=False, yscale_bottom=0.1, yscale_top=0.1)

    h2.Draw()
    graph_high_eff.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/counts_vs_time_high_eff' %(layer.GetDetKey()), wait=wait)

    #----------------------------------------------------------------------------------
    ytitle = 'N. of panels with #epsilon < %.2f' %options.max_low_panel_eff
    
    h2 = makeBoxHistFromGraphs([graph_low_eff], xtitle, ytitle,
                               do_err=False, yscale_bottom=0.1, yscale_top=0.1)

    h2.Draw()
    graph_low_eff.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/counts_vs_time_low_eff' %(layer.GetDetKey()), wait=wait)

    #----------------------------------------------------------------------------------
    h2 = makeBoxHistFromGraphs([graph_poutlier, graph_noutlier], xtitle, ytitle = 'N. of outlier panels',
                               do_err=False, yscale_bottom=0.1, yscale_top=0.35)

    h2.Draw()
    graph_poutlier.Draw('P')
    graph_noutlier.Draw('P')    
    layer.DrawLayerLabel()

    leg = makeLegend(x=0.65, y=0.77, w=0.15, h=0.12)
    leg.AddEntry(graph_noutlier, '#downarrow outliers', 'p')
    leg.AddEntry(graph_poutlier, '#uparrow outliers',   'p')    
    leg.Draw('SAME')
    
    updateCanvas(c, name='%s/counts_vs_time_outlier' %(layer.GetDetKey()), wait=wait)
    
    #----------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------
    #----------------------------------------------------------------------------------    
    h2 = makeBoxHistFromGraphs([rgraph_live_eff], xtitle, 'Fraction of panels with #epsilon > 0',
                               do_err=False, yscale_bottom=0.1, yscale_top=0.1)

    h2.Draw()
    rgraph_live_eff.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/fraction_vs_time_good_eff' %(layer.GetDetKey()), wait=wait)
    
    #----------------------------------------------------------------------------------    
    ytitle = 'Fraction of panels with #epsilon > %0.2f' %options.min_high_panel_eff
    
    h2 = makeBoxHistFromGraphs([rgraph_high_eff], xtitle, ytitle,
                               do_err=False, yscale_top=0.1, yscale_bottom=0.1)

    h2.Draw()
    rgraph_high_eff.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/fraction_vs_time_good_eff' %(layer.GetDetKey()), wait=wait)
    
    #----------------------------------------------------------------------------------
    ytitle = 'Fraction of panels with #epsilon < %.2f' %options.max_low_panel_eff
    
    h2 = makeBoxHistFromGraphs([rgraph_low_eff], xtitle, ytitle,
                               do_err=False, yscale_bottom=0.1, yscale_top=0.1)

    h2.Draw()
    rgraph_low_eff.Draw('P')
    layer.DrawLayerLabel()
    updateCanvas(c, name='%s/fraction_vs_time_low_eff' %(layer.GetDetKey()), wait=wait)

#==================================================================================================================
def checkHistConsistency(h1, h2):

    '''Check that there is no overlap between two histograms: RPC sub-stations
       should not have overlapping panels.
    '''
    
    if h1.GetNbinsX() != h2.GetNbinsX():
        raise Exception('checkHistConsistency - mismatch in number of X bins')

    if h1.GetNbinsY() != h2.GetNbinsY():
        raise Exception('checkHistConsistency - mismatch in number of Y bins')

    for binx in range(1, h1.GetNbinsX()+1):
        for biny in range(1, h1.GetNbinsY()+1):
            v1 = h1.GetBinContent(binx, biny)
            v2 = h2.GetBinContent(binx, biny)

            if v1 > 0.0 and v2 > 0.0:
                raise Exception('checkHistConsistency - same bin has non-zero value in both histograms: %d, %d' %(binx, biny))

    return True

#==================================================================================================================
def createRPCDetLayoutHist(h):

    '''Create empty histogram with RPC detector layout'''

    hrpc = ROOT.TH2F('rpc_layout', '',
                     h.GetNbinsX(), h.GetXaxis().GetXmin(), h.GetXaxis().GetXmax(),
                     16,            h.GetYaxis().GetXmin(), h.GetYaxis().GetXmax())
    
    xtitle = '#eta-index'
    ytitle = '#phi sector'    
    
    setHistOptions([hrpc], xtitle, ytitle)

    hrpc.GetXaxis().SetNdivisions(218, True)
    hrpc.GetXaxis().SetLabelSize(0.03)
    hrpc.GetYaxis().SetLabelSize(0.03)    

    sector_labels = ['L1',  'S2',   'L3',  'S4',  'L5',  'S6',
		     'L7',  'S8',   'L9',  'S10', 'L11', 'FG12',
		     'L13', 'FG14', 'L15', 'S16']
    
    for i in range(0, len(sector_labels)):
        hrpc.GetYaxis().SetBinLabel(i+1, sector_labels[i])

    return hrpc

#==================================================================================================================
def getLabel(layer, strip, doublet, gasgap):

    l = ''

    if   layer == 'BM': l = 'Middle layer'
    elif layer == 'BO': l = 'Outer layer'
    else:
        raise Exception('getLabel - uknown layer: %s' %layer)
    
    if   strip == 'eta': l += ', #eta strips'
    elif strip == 'phi': l += ', #phi strips'
    else:
        raise Exception('getLabel - uknown strips: %s' %strip)

    if doublet in [1, 2]:
        l += ', dR%d' %doublet
    else:
        raise Exception('getLabel - uknown doublet: %s' %doublet)

    if gasgap in [1, 2]:
        l += ', gas gap %d' %gasgap
    else:
        raise Exception('getLabel - uknown gas gap: %s' %gasgap)

    return l
    
#==================================================================================================================
def getLabelNoStrips(layer, doublet, gasgap):

    l = ''

    if   layer == 'BM': l = 'Middle layer'
    elif layer == 'BO': l = 'Outer layer'
    else:
        raise Exception('getLabel - uknown layer: %s' %layer)
    
    if doublet in [1, 2]:
        l += ', dR%d' %doublet
    else:
        raise Exception('getLabel - uknown doublet: %s' %doublet)

    if gasgap in [1, 2]:
        l += ', gas gap %d' %gasgap
    else:
        raise Exception('getLabel - uknown gas gap: %s' %gasgap)

    return l

#==================================================================================================================
def collectNumDenHists(run, layer, key, detkey):

    '''Read numerator and denominator histograms for efficiency measurements.
       Sum together histograms within each RPC detector layer.
    '''

    stations = {'BM': ['BML', 'BMS', 'BMF', 'BME'],
                'BO': ['BOL', 'BOS', 'BOF', 'BOG']}
    
    hden = None
    hnum = None
    
    for station in stations[layer]:
        hbase = '%sEtaPhi_%s'  %(station, key)
        
        if options.key != None and re.search(options.key, detkey) == None:
            continue
        
        den_hist = run.GetHist('den%s' %hbase)
        num_hist = run.GetHist('num%s' %hbase)

        if not den_hist:
            raise Exception('Run #%d - failed to get histogram: den%s' %(run.run_number, hbase))
        if not num_hist:
            raise Exception('Run #%d - failed to get histogram: num%s' %(run.run_number, hbase))

        if hden == None and hnum == None:
            hden = den_hist
            hnum = num_hist
        else:
            checkHistConsistency(hden, den_hist)
            checkHistConsistency(hnum, num_hist)
            
            hden.Add(den_hist)
            hnum.Add(num_hist)

    if hden == None or hnum == None:
        msg.debug('collectNumDenHists - failed to find valid numerator or denominator histograms')
        return (hnum, hden)

    sectorLabel = ['L1',  'S2',   'L3',  'S4',  'L5',  'S6',
		   'L7',  'S8',   'L9',  'S10', 'L11', 'FG12',
		   'L13', 'FG14', 'L15', 'S16']

    xtitle     = '#eta-index'
    ytitle     = '#phi sector'
    ztitle_num = 'Numerator entries'
    ztitle_den = 'Denominaror entries'    
    
    setHistOptions([hnum], xtitle, ytitle, ztitle_num)
    setHistOptions([hden], xtitle, ytitle, ztitle_den)

    return (hnum, hden)
        
#==================================================================================================================
def main():

    if len(args) < 1:
        msg.error('Need at least one input directory')
        return

    #----------------------------------------------------------------------------------
    # Find and open files with RPC panel efficiency histograms
    #
    eff_runs = getGoodEffRuns(args)

    if len(eff_runs) < 1:
        msg.warning('Zero good efficiency runs - nothing to do')
        return
    
    #----------------------------------------------------------------------------------
    # Create list of eff histograms
    #
    layers   = ['BM', 'BO']
    doublets = [1, 2]
    gasgaps  = [1, 2]
    strips   = ['eta', 'phi']    
    
    rpc_layers = []    

    for layer in layers:
        for doublet in doublets:
            for gasgap in gasgaps:

                strip_layers = {}
                label_no_strips = getLabelNoStrips(layer, doublet, gasgap)
                for strip in strips:            
                    key    = '%sStrips_dR%d_gg%d' %(strip, doublet, gasgap)
                    detkey = '%s_%s' %(layer, key)
                    label  = getLabel(layer, strip, doublet, gasgap)

                    msg.info('----------------------------------------------------------------------')
                    msg.info('Process %s' %label)
                    msg.info('Detector key: %s' %detkey)
                    
                    effs = []

                    for run in eff_runs:
                        hnum, hden = collectNumDenHists(run, layer, key, detkey)

                        if not hnum or not hden:
                            continue
                        
                        eff = Eff(run, detkey, label, hnum, hden)
                    
                        if not eff.IsGoodEffRun(): 
                            msg.info('%s -> REJECT' %eff.PrintEffAsStr())
                            continue

                        msg.info(eff.PrintEffAsStr())

                        effs += [eff]                  

                    if len(effs) == 0:
                        msg.info('Selected %d effieciency runs - continue' %len(effs))
                        continue

                    #----------------------------------------------------------------------------------
                    # Run effiency analysis
                    #
                    if options.draw_eff_only:
                        continue
                    
                    rpc_layer = Layer(detkey, label, effs)
                    rpc_layer_no_strip_name = Layer(detkey, label_no_strips, effs)
                    rpc_layer.RemoveRunsWithLowDenCounts()
                    rpc_layers += [rpc_layer]
                    
                    strip_layers[strip] = rpc_layer_no_strip_name
                    
                    rpc_layer.DrawEffHists()
                    
                    with ExecFunc(rpc_layer, 'plotSinglePanelEffs'):
                        pass

                    with ExecFunc(rpc_layer, 'plotTotalCountsPerLayer'):
                        pass

                    with ExecFunc(rpc_layer, 'plotAllPanelEffs'):
                        pass

                    with ExecFunc(rpc_layer, 'plotEffRef'):
                        pass

                    with ExecFunc(rpc_layer, 'plotPanelCounts'):
                        pass                    
                with ExecFunc(strip_layers, 'plotEffDiffEtaPhi'):
                    pass
                    
    ###----------------------------------------------------------------------------------
    ### Sort and print RPC layer name
    
    rpc_layers.sort(key = lambda x: x.detkey)
    
    msg.info('Processed %d RPC detector layers with valid measurements' %len(rpc_layers))
    
    for l in rpc_layers:
        snum = ('%.2f' %(l.GetMeanNumEntriesPerPanelPerIntLumi())).rjust(5)
        sden = ('%.2f' %(l.GetMeanDenEntriesPerPanelPerIntLumi())).rjust(5)
        shit = 'mean #hits per panel per pb^1: num=%s, den=%s' %(snum, sden)
        
        msg.info('   %s - max #panels=%3d, %s' %(l.GetDetKey(), l.GetLayerNPanels(), shit))
    
    with ExecFunc(rpc_layers, 'plotLayersTotalCounts'):
        pass

    #----------------------------------------------------------------------------------
    # Make summary plots for layers merged together
    #
    merged_layers = []
    
    merged_layers += [mergeLayers(rpc_layers, 'BM_.*Strips_dR1', 'BM_dR1', 'Middle layer, dR1')]
    merged_layers += [mergeLayers(rpc_layers, 'BM_.*Strips_dR2', 'BM_dR2', 'Middle layer, dR2')]

    merged_layers += [mergeLayers(rpc_layers, 'BO_.*Strips_dR1', 'BO_dR1', 'Outer layer, dR1')]
    merged_layers += [mergeLayers(rpc_layers, 'BO_.*Strips_dR2', 'BO_dR2', 'Outer layer, dR2')]

    
    merged_layers += [mergeLayers(rpc_layers, 'BM_etaStrips', 'BM_etaStrips', 'Middle layer, #eta strips')]
    merged_layers += [mergeLayers(rpc_layers, 'BM_phiStrips', 'BM_phiStrips', 'Middle layer, #phi strips')]

    merged_layers += [mergeLayers(rpc_layers, 'BO_etaStrips', 'BO_etaStrips', 'Outer layer, #eta strips')]
    merged_layers += [mergeLayers(rpc_layers, 'BO_phiStrips', 'BO_phiStrips', 'Outer layer, #phi strips')]
    
    for layer in merged_layers:
        with ExecFunc(layer, 'plotPanelCounts', place='merged'):
            pass

        with ExecFunc(rpc_layer, 'plotSinglePanelEffs', place='merged'):
            pass
        
#==================================================================================================================
if __name__ == '__main__':

    timeStart = time.time()
    msg.info('Current time: %s' %(time.asctime(time.localtime())))    
    
    main()

    if rootfile:
        msg.info('Save output ROOT file...')
        rootfile.Write()
        rootfile.Close()
        msg.info('Save output ROOT file... done')

    msg.info('Number of printed canvases: %d' %nprint_canvas)
    msg.info('Current time: %s' %(time.asctime(time.localtime())))
    msg.info('Total job time: %.1fs' %(time.time()-timeStart))        
    msg.info('All done')
