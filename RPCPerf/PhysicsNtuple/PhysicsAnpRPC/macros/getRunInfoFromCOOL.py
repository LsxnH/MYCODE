#!/bin/env python

###----------------------------------------------------------------------------------
### Process options
###
from optparse import OptionParser

p = OptionParser()

p.add_option('--beg-run',  type='int',          default=300000,  dest='beg_run')
p.add_option('--end-run',  type='int',          default=300500,  dest='end_run')

p.add_option('--pref',         type='string',   default=None,          dest='pref')
p.add_option('--post',         type='string',   default=None,          dest='post')
p.add_option('--outdir', '-o', type='string',   default=None,          dest='outdir')

p.add_option('-d',           action='store_true', default=False, dest='debug')
p.add_option('-s', '--save', action='store_true', default=False, dest='save')

(options, args) = p.parse_args()

import sys
import os
import time

from PyCool import cool
from PyCool import coral

#========================================================================================================
def getLog(name, level='INFO', debug=False):

    import logging

    f = logging.Formatter('%(name)s: %(levelname)s - %(message)s')        
    h = logging.StreamHandler(sys.stdout)
    h.setFormatter(f)
    
    log = logging.getLogger(name)
    log.addHandler(h)
    log.setLevel(logging.INFO)

    return log

msg = getLog(os.path.basename(__file__))

#==================================================================================================================
class Folder:
    def __init__(self, dbconnect, name):

        self.name = name
        readonly  = True

        try:
            self.db = cool.DatabaseSvcFactory.databaseService().openDatabase(dbconnect, readonly)
        except Exception, e:
            msg.error('Can not open DB or get chain folder because of exception: %s' %e)
            raise Exception(e)

        if not self.db.existsFolder(name):
            raise Exception('Folder does not exist: "%s"' %name)

        self.folder = self.db.getFolder(name)

#==================================================================================================================
class LumiBlock:
    def __init__(self, cool_obj):

        self.run, self.lb = getRunLBFromObj(cool_obj)

        self.start_time = cool_obj.payload()['StartTime']
        self.end_time   = cool_obj.payload()['EndTime']
        self.key_since  = cool_obj.since()
        self.key_until  = cool_obj.until()
        
        self.inst_lumi_valid   = None
        self.inst_lumi_algid   = None
        self.inst_lumi_offline = None
        
        self.is_atlas_physics  = None
        self.is_stable_beams   = None
        self.l1_live_fraction  = None

        self.beam1_intensity   = -1.0
        self.beam2_intensity   = -1.0        

    def ReadLumiInfo(self, cool_obj):
        self.inst_lumi       = cool_obj.payload()['LBAvInstLumi']
        self.inst_lumi_valid = cool_obj.payload()['Valid']
        self.inst_lumi_algid = cool_obj.payload()['AlgorithmID']
        
    def ReadPhysicsLumiInfo(self, payload):
        if self.is_atlas_physics != None:
            raise Exception('ReadPhysicsLumiInfo - duplicate lb=%d' %self.lb)

        self.is_atlas_physics        = payload['AtlasPhysics']
        self.is_stable_beams         = payload['StableBeams']
        self.l1_live_fraction        = payload['LiveFraction']
        self.beam1_intensity         = payload['Beam1Intensity']
        self.beam2_intensity         = payload['Beam2Intensity']
        self.inst_lumi_offline       = payload['InstLumiAll']
        self.inst_lumi_offline_algid = payload['AlgorithmID']
        
    def IsStableBeams(self):
        return (self.is_stable_beams != None and self.is_stable_beams)

    def IsGoodPhysics(self):
        return (self.IsStableBeams() and self.is_atlas_physics != None and self.is_atlas_physics)
    
    def GetStartGMTime(self):
        return time.gmtime(self.start_time*1.0e-9)

    def GetEndGMTime(self):
        return time.gmtime(self.end_time*1.0e-9)

    def GetLBSecs(self):
        if not (self.end_time > self.start_time):
            raise Exception('GetLBSecs - invalid start/end times')
        return (self.end_time - self.start_time)*1.0e-9

    def GetInstLumi(self):
        if self.inst_lumi_valid == 0:
            return self.inst_lumi

        return 0.0        

    def GetOfflineInstLumi(self):
        if self.inst_lumi_offline != None:
            return self.inst_lumi_offline

        return 0.0
    
    def GetIntLumi(self):
        if self.inst_lumi_valid == 0:
            return self.inst_lumi*self.GetLBSecs()

        return 0.0

    def GetL1LiveFraction(self):
        if self.l1_live_fraction == None:
            return 0.0

        return self.l1_live_fraction
    
    def GetPhysicsIntLumi(self):
        return self.GetL1LiveFraction() * self.GetOfflineInstLumi() * self.GetLBSecs()

    def PrintAsStr(self):
        s  = 'Run %d, lb %4d, '        %(self.run, self.lb)
        s += ' dt=%3d, '               %self.GetLBSecs()
        s += ' IsStableBeams=%4s, '    %self.IsStableBeams()
        s += ' IsGoodPhysics=%4s, '    %self.IsGoodPhysics()
        s += ' inst_lumi_onl=%5d, '    %self.GetInstLumi()
        s += ' inst_lumi_off=%f'       %self.GetOfflineInstLumi()        
        s += ' live_fraction=%.2f'     %self.GetL1LiveFraction()
        s += ' beam 1=%f'              %self.beam1_intensity
        s += ' beam 2=%f'              %self.beam2_intensity

        return s
    
#==================================================================================================================
class Run:
    def __init__(self, run_number, lbs):
        self.run_number = run_number
        self.lbs        = lbs
        self.lb_first   = None
        self.lb_last    = None
        
        self.int_lumi       = 0
        self.peak_inst_lumi = 0.0
        
        for lb in lbs:
            self.peak_inst_lumi = max([self.peak_inst_lumi, lb.GetInstLumi()])
            
            self.int_lumi += lb.GetIntLumi()

            if self.lb_first == None:
                self.lb_first = lb
                self.lb_last  = lb
            else:
                if lb.lb < self.lb_first.lb: self.lb_first = lb
                if lb.lb > self.lb_last.lb:  self.lb_last  = lb

    def GetRunStartTime(self):
        return self.lb_first.start_time

    def GetRunEndTime(self):
        return self.lb_last.end_time
    
    def GetIntLumiInvPB(self):
        return self.int_lumi*1.0e-6

    def GetPeakInstLumi(self):
        return self.peak_inst_lumi
    
    def GetPhysicsIntLumiInvPB(self):
        ilumi = 0.0
        
        for lb in self.lbs:
            if lb.is_atlas_physics:
                ilumi += lb.GetPhysicsIntLumi()

        return ilumi*1.0e-6
    
    def GetPeakPhysicsInstLumi(self):
        peak_lumi = None
        
        for lb in self.lbs:
            #
            # Select lumiblocks of at least 10s, stable beams, ATLAS run in physics mode,
            # and live fraction of at least 0.1
            #
            if not lb.IsGoodPhysics() or lb.GetLBSecs() < 10 or lb.GetL1LiveFraction() < 0.1:
                continue

            if peak_lumi == None:
                peak_lumi = lb.GetOfflineInstLumi()
            else:
                peak_lumi = max([peak_lumi, lb.GetOfflineInstLumi()])

        if peak_lumi == None:
            return 0.0
                
        return peak_lumi

    def GetMeanPhysicsInstLumi(self):
        sum_lumi = 0.0
        sum_time = 0.0        
        
        for lb in self.lbs:
            if not lb.IsGoodPhysics():
                continue

            lb_lumi = lb.GetOfflineInstLumi()
            lb_time = lb.GetL1LiveFraction()*lb.GetLBSecs()
            
            sum_lumi += lb_time*lb_lumi
            sum_time += lb_time

        if sum_time > 0.0:
            return sum_lumi/sum_time

        return 0.0

    def AsStr(self):
        ip = ('%.0f' %(self.GetPeakInstLumi())).rjust(5)
        il = ('%.3f' %(self.GetIntLumiInvPB())).rjust(8)
        ts = getTimeAsStr(self.lb_first.start_time)

        s = 'Run %s peak lumi=%s int lumi=%s %s' %(self.run_number, ip, il, ts)
        
        return s

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
def getTimeAsStr(timestamp, do_simple=False):

    if do_simple:
        gmt = time.gmtime(timestamp*1.0e-9)
        return '%d-%02d-%02d' %(gmt[0], gmt[1], gmt[2])

    return time.asctime(time.gmtime(timestamp*1.0e-9))


#==================================================================================================================
def getRunLBFromObj(obj):
    run = (obj.since() >> 32)
    lb  = (obj.since() & 0xffff)
    return (run, lb)

#==================================================================================================================
def findLumiBlock(runs, run, lb):

    try:
        lbs = runs[run]
    except KeyError:
        return None

    try:
        return lbs[lb]
    except:
        return None

#==================================================================================================================
def getRangeTimeStamps(runs):

    start_time = None
    end_time   = None

    for r in runs:
        for lb in r.lbs:
            if start_time == None:
                start_time = lb.start_time
                end_time   = lb.end_time
            else:
                start_time = min([lb.start_time, start_time])
                end_time   = max([lb.end_time,   end_time])

    return start_time, end_time

#==================================================================================================================
def getIntervalOfValidity(runs):

    key_since = None
    key_until = None

    for r in runs:
        for lb in r.lbs:
            if key_since == None:
                key_since = lb.key_since
                key_until = lb.key_until
            else:
                key_since = min([lb.key_since, key_since])
                key_until = max([lb.key_until,   key_until])

    return key_since, key_until

#==================================================================================================================
def printRunInfo(physics_runs):

    s  = '# \n'    
    s += '# row 0: run number \n'
    s += '# row 1: number of lumiblocks \n'
    s += '# row 2: peak          offline instantenous luminosity in stable beams and in ATLAS physics mode [x10^30 cm^-2*s^-1] \n'
    s += '# row 3: time averaged offline instantenous luminosity in stable beams and in ATLAS physics mode [x10^30 cm^-2*s^-1] \n'
    s += '# row 4: online          integrated luminosity [pb^-1] \n'
    s += '# row 5: offline physics integrated luminosity [pb^-1] \n'
    s += '# row 6: run start timestamp [ns] \n'
    s += '# row 7: run end   timestamp [ns] \n'
    s += '# row 8: date \n'
    s += '# \n'    
    
    for r in physics_runs:
        slb = ('%s' %len(r.lbs)).rjust(4)
        sp1 = ('%.1f' %r.GetPeakPhysicsInstLumi()).rjust(7)
        sp2 = ('%.1f' %r.GetMeanPhysicsInstLumi()).rjust(7)        
        sl1 = ('%.3f' %r.GetIntLumiInvPB       ()).rjust(10)
        sl2 = ('%.3f' %r.GetPhysicsIntLumiInvPB()).rjust(10)        
        sdt = getTimeAsStr(r.lb_first.start_time, True)

        s += '%d ' %r.run_number
        s += '%s ' %slb
        s += '%s ' %sp1
        s += '%s ' %sp2        
        s += '%s ' %sl1
        s += '%s ' %sl2
        s += '%d ' %r.GetRunStartTime()
        s += '%d ' %r.GetRunEndTime()
        s += '%s ' %sdt
        s += '\n'

        print '   Run %s  #lbs=%4d  %s' %(r.run_number, len(r.lbs), getTimeAsStr(r.lb_first.start_time))

    if options.save:
        ofile = open(getOutName('run_info.txt'), 'w')
        ofile.write(s)
        ofile.close()

    print s
    return s
        
#==================================================================================================================
def getRunLB():

    timeStart = time.time()

    msg.info('getRunLB - will search for ATLAS runs in the following range:')
    msg.info('   beg run: %s' %(options.beg_run))
    msg.info('   end run: %s' %(options.end_run))

    f = Folder('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/LBLB')

    fit = f.folder.browseObjects(options.beg_run << 32, options.end_run << 32, cool.ChannelSelection.all())

    runs = {}

    while fit.goToNext():
        lb = LumiBlock(fit.currentRef())

        try:
            runs[lb.run]
        except KeyError:
            runs[lb.run]  = {}

        runs[lb.run][lb.lb] = lb

    msg.info('getRunLB - found %d ATLAS runs' %len(runs))
    msg.info('getRunLB - total time: %.1fs' %(time.time()-timeStart))

    return runs

#==================================================================================================================
def getRunOnlineLumi(runs):

    timeStart = time.time()
    timeCurr  = time.time()

    msg.info('getRunOnlineLumi - will read online luminosity for %d ATLAS runs' %(len(runs)))

    f = Folder('COOLONL_TRIGGER/CONDBR2', '/TRIGGER/LUMI/OnlPrefLumi')
    icount = 0

    for run_, lbs in runs.iteritems():
        
        fit = f.folder.browseObjects(run_ << 32, (run_ << 32) + 100000, cool.ChannelSelection(0))

        while fit.goToNext():
            obj = fit.currentRef()

            run = (obj.since() >> 32)
            lb  = (obj.since() & 0xffff)

            if run != run_:
                raise Exception('getRunOnlineLumi - logic error: run=%s, run_=%s' %(run, run_))

            if lb == 0:
                continue

            try:
                lblock = lbs[lb]        
            except KeyError:
                msg.warning('getRunOnlineLumi - failed to find LumiBlock for run, lb: %s %s' %(run, lb))
                continue

            lblock.ReadLumiInfo(obj)
        
        icount += 1
        
        if icount % 10 == 0:
            msg.info('getRunOnlineLumi - icount=%4d  delta t=%.1fs' %(icount, time.time()-timeCurr))
            timeCurr = time.time()

    msg.info('getRunOnlineLumi - total time: %.1fs' %(time.time()-timeStart))

#==================================================================================================================
def getRunPhysicsLumi(run, debug=options.debug):

    timeStart = time.time()    
    key_since = run.GetRunStartTime() - 100000
    key_until = run.GetRunEndTime()   + 100000
    
    msg.info('getRunPhysicsLumi - read offline luminosity for run = %d, number of lbs=%d' %(run.run_number, len(run.lbs)))
    
    f = Folder('COOLOFL_TRIGGER/CONDBR2', '/TRIGGER/OFLLUMI/LumiAccounting')

    fit = f.folder.browseObjects(key_since, key_until, cool.ChannelSelection.all(), 'OflLumiAcct-001')
    icount = 0

    while fit.goToNext():
        obj = fit.currentRef()
        crun = obj.payload()['Run']        
        clb  = obj.payload()['LumiBlock']

        if crun != run.run_number:
            continue
        
        for lb in run.lbs:
            if lb.lb == clb:
                lb.ReadPhysicsLumiInfo(obj.payload())
                icount += 1                
                break
        
        if debug:
            s  = '%d'               %obj.payload()['Run']
            s += ' lb=%4d -'        %obj.payload()['LumiBlock']
            s +=' AtlasPhysics=%d'  %obj.payload()['AtlasPhysics']
            s +=' InstLumi=%f,'     %obj.payload()['InstLumi']        
            s +=' LiveFraction=%f'  %obj.payload()['LiveFraction']
            print s

    if len(run.lbs) != icount:
        msg.warning('getRunPhysicsLumi - number of missed lbs=%d' %(len(run.lbs) - icount))
        msg.warning('   number of run lbs=%d'     %(len(run.lbs)))
        msg.warning('   number of matched lbs=%d' %(icount))
        
    if debug:
        for lb in run.lbs:
            if lb.inst_lumi_offline == None:                
                msg.info(lb.PrintAsStr() + ' - missing offline lumi')
            else:
                msg.info(lb.PrintAsStr())
            
    msg.info('getRunPhysicsLumi - total time: %.1fs' %(time.time()-timeStart))            
            
#==================================================================================================================
def main():
    
    collect_runs = getRunLB()
    physics_runs = []

    getRunOnlineLumi(collect_runs)

    for run, lbs_ in collect_runs.iteritems():
        lbs = lbs_.values()
        r = Run(run, lbs)

        if r.GetPeakInstLumi() > 100.0 and r.GetIntLumiInvPB() > 1.0:
            physics_runs += [r]
        else:
            msg.info('Ignore %s' %r.AsStr())

    if len(physics_runs) == 0:
        msg.warning('Found 0 physics runs')
        return

    physics_runs.sort(key = lambda x: x.run_number)
            
    period_start_time, period_end_time  = getRangeTimeStamps   (physics_runs)
    period_key_since,  period_key_until = getIntervalOfValidity(physics_runs)

    msg.info('Selected %d physics runs with max instantenous luminosity > 100e30 cm^-2 s^-1' %len(physics_runs))
    msg.info('   period start time: %s' %getTimeAsStr(period_start_time))
    msg.info('   period end time:   %s' %getTimeAsStr(period_end_time))

    for run in physics_runs:
        getRunPhysicsLumi(run)

    printRunInfo(physics_runs)
        
#==================================================================================================================
if __name__ == '__main__':
    main()
