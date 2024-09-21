
import logging
import os
import re
import sys

loggers = {}
    
#========================================================================================================
def getLog(name, level='INFO', debug=False, print_time=False):

    global loggers

    if name in loggers:
        return loggers[name]

    if print_time:
        f = logging.Formatter('%(asctime)s - %(name)s: %(levelname)s - %(message)s', datefmt='%Y-%m-%d %H:%M:%S')
    else:
        f = logging.Formatter('%(name)s: %(levelname)s - %(message)s')
        
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.addHandler(h)
    
    if debug:
        log.setLevel(logging.DEBUG)
    else:
        if level == 'DEBUG':   log.setLevel(logging.DEBUG)
        if level == 'INFO':    log.setLevel(logging.INFO)
        if level == 'WARNING': log.setLevel(logging.WARNING)    
        if level == 'ERROR':   log.setLevel(logging.ERROR)

    return log

#========================================================================================================
def setPlotDefaults(root, options = None):

    root.gROOT.SetStyle('Plain')

    root.gStyle.SetFillColor(10)           
    root.gStyle.SetFrameFillColor(10)      
    root.gStyle.SetCanvasColor(10)         
    root.gStyle.SetPadColor(10)            
    root.gStyle.SetTitleFillColor(0)       
    root.gStyle.SetStatColor(10)   
    
    root.gStyle.SetCanvasBorderMode(0)
    root.gStyle.SetFrameBorderMode(0) 
    root.gStyle.SetPadBorderMode(0)   
    root.gStyle.SetDrawBorder(0)      
    root.gStyle.SetTitleBorderSize(0)
    
    root.gStyle.SetFuncWidth(2)
    root.gStyle.SetHistLineWidth(2)
    root.gStyle.SetFuncColor(2)
    
    root.gStyle.SetPadTopMargin(0.08)
    root.gStyle.SetPadBottomMargin(0.16)
    root.gStyle.SetPadLeftMargin(0.14)
    root.gStyle.SetPadRightMargin(0.08)
  
    # set axis ticks on top and right
    root.gStyle.SetPadTickX(1)         
    root.gStyle.SetPadTickY(1)         
  
    # Set the background color to white
    root.gStyle.SetFillColor(10)           
    root.gStyle.SetFrameFillColor(10)      
    root.gStyle.SetCanvasColor(10)         
    root.gStyle.SetPadColor(10)            
    root.gStyle.SetTitleFillColor(0)       
    root.gStyle.SetStatColor(10)           
  
  
    # Turn off all borders
    root.gStyle.SetCanvasBorderMode(0)
    root.gStyle.SetFrameBorderMode(0) 
    root.gStyle.SetPadBorderMode(0)   
    root.gStyle.SetDrawBorder(0)      
    root.gStyle.SetTitleBorderSize(0) 
  
    # Set the size of the default canvas
    root.gStyle.SetCanvasDefH(400)          
    root.gStyle.SetCanvasDefW(650)          
  
    # Set fonts
    font = 42
    root.gStyle.SetLabelFont(font,'xyz')
    root.gStyle.SetStatFont(font)       
    root.gStyle.SetTitleFont(font)      
    root.gStyle.SetTitleFont(font,'xyz')
    root.gStyle.SetTextFont(font)       
    root.gStyle.SetTitleX(0.3)        
    root.gStyle.SetTitleW(0.4)        
  
   # Set Line Widths
    root.gStyle.SetFuncWidth(2)
    root.gStyle.SetHistLineWidth(2)
    root.gStyle.SetFuncColor(2)
  
   # Set tick marks and turn off grids
    root.gStyle.SetNdivisions(505,'xyz')
  
   # Set Data/Stat/... and other options
    root.gStyle.SetOptDate(0)
    root.gStyle.SetDateX(0.1)
    root.gStyle.SetDateY(0.1)
    root.gStyle.SetOptStat('reimo')
    root.gStyle.SetOptFit(111)
    root.gStyle.SetStatFormat('6.3f')
    root.gStyle.SetFitFormat('5.4g')
    root.gStyle.SetPaintTextFormat('0.2f')

    root.gStyle.SetStatX(0.919)
    root.gStyle.SetStatY(0.919)
    root.gStyle.SetOptTitle(0)
    root.gStyle.SetStatBorderSize(0)

    root.gStyle.SetLabelSize(0.04, 'xyz')
    
    root.gStyle.SetTitleOffset(1.05,'xz')
    root.gStyle.SetTitleOffset(1.20,'y')
    root.gStyle.SetTitleOffset(1.30,'z')    
    root.gStyle.SetTitleSize(0.055, 'xyz')
    #root.gStyle.SetTextAlign(22)
    root.gStyle.SetTextSize(0.12)
    
    #root.gStyle.SetPaperSize(root.TStyle.kA4)  
    root.gStyle.SetPalette(1)
  
   #root.gStyle.SetHistMinimumZero(True)
    root.TGaxis.SetExponentOffset(0.03, -0.055, 'x')

    root.gROOT.ForceStyle()

#========================================================================================================
def setRPCColorPalette(ROOT):

    from array import array
    
    NRGBs = 6
    NCont = 40
    
    stops = array('d', [ 0.00, 0.30, 0.50, 0.80, 0.95,  1.00 ])
    green = array('d', [ 0.00, 0.00, 0.80, 1.00, 0.80,  1.00 ])
    blue  = array('d', [ 0.00, 0.00, 0.00, 0.00, 0.00,  0.00 ])
    red   = array('d', [ 0.80, 1.00, 1.00, 0.75, 0.00,  0.00 ])
    
    FI = ROOT.TColor.CreateGradientColorTable(NRGBs,
					      stops,
                                              red,
                                              green,
                                              blue,
					      NCont)

    MyPalette = array('i')
    
    for i in range(0, NCont):
        MyPalette.append(FI+i)
    
    ROOT.gStyle.SetPalette(NCont, MyPalette)
