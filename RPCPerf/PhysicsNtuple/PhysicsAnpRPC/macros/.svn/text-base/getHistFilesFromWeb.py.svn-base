#!/usr/bin/env python

import os
import re
import sys
import time
import optparse
import urllib   
    
import PhysicsAnpRPC.PhysicsAnpRPCEff as rpc

p = optparse.OptionParser()

p.add_option('-d', '--debug',   action='store_true', default=False,   dest='debug')
p.add_option('-w', '--wait',    action='store_true', default=False,   dest='wait')
p.add_option('-s', '--save',    action='store_true', default=False,   dest='save')
p.add_option('--logy',          action='store_true', default=False,   dest='logy')

p.add_option('--year',         type='int',    default=2016,          dest='year')
p.add_option('--input', '-i',  type='string', default=None,          dest='input')
p.add_option('--pref',         type='string', default=None,          dest='pref')
p.add_option('--post',         type='string', default=None,          dest='post')
p.add_option('--outdir', '-o', type='string', default=None,          dest='outdir')

(options, args) = p.parse_args()

#========================================================================================================
def getLog(name, level='INFO', debug=False):

    import logging

    f = logging.Formatter('%(message)s - %(levelname)s')
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.addHandler(h)
    log.setLevel(logging.INFO)

    return log

msg = getLog(os.path.basename(__file__))

#==================================================================================================================
class Run:
    
    def __init__(self, parts):
        self.run_number = int  (parts[0])

    def AsStr(self):
        return 'Run %d' %(self.run_number)

#==================================================================================================================
def getOutName(name):

    if options.pref != None:
        name = '%s_%s' %(options.pref, name)

    if options.post != None:
        name = '%s_%s' %(name, options.post)

    if options.outdir != None:

        if not os.path.isdir(options.outdir):
            os.makedirs(options.outdir)

        name = '%s/%s' %(options.outdir.rstrip('/'), name)
        
    return name

#==================================================================================================================
def getFile(hpath, local_file): 

    testfile = urllib.URLopener()

    try:
        testfile.retrieve(hpath, local_file)
        msg.info   ('   downloaded:    %s' %hpath)
        return True
    except IOError:
        msg.warning('   failed to get: %s' %hpath)

    return False

#==================================================================================================================
def getRunFiles(run):         

    import urllib
    
    hpath = 'http://atlasui02.na.infn.it:8088/~l1mu/%s/EfficiencyMain/00%d/o.root' %(options.year, run)
    lfile = 'o_run%d.root' %run

    getFile(hpath, getOutName(lfile))

    hpath = 'http://atlasui02.na.infn.it:8088/~l1mu/%s/EfficiencyMain/00%d/panel_eff.root' %(options.year, run)
    lfile = 'panel_eff_run%d.root' %run

    getFile(hpath, getOutName(lfile))

#==================================================================================================================
def getRuns():
    
    runs = []

    if options.input == None or not os.path.isfile(options.input):
        return runs

    ifile = open(options.input, 'r')
    
    for iline in ifile.readlines():
        if iline.count('#'):
            continue
        
        parts = iline.rstrip('\n').split()
        
        if len(parts) < 6:
            continue

        runs += [Run(parts)]

    return runs

#==================================================================================================================
def main():

    runs = getRuns()
    
    for r in runs:
        print r.AsStr()

        getRunFiles(r.run_number)

#==================================================================================================================
if __name__ == '__main__':

    timeStart = time.time()
    msg.info('Current time: %s' %(time.asctime(time.localtime())))  

    main()

    msg.info('Total processing time: %.1fs' %(time.time()-timeStart))
    msg.info('All done')
