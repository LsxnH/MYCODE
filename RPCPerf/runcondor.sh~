#!/bin/bash
#cd /home/hengli

cd /home/hengli/testarea/RPCPerf
source setup_atlas_analysis_release.sh
#./compile.csh sim2016_3
python PhysicsNtuple/PhysicsAnpRPC/macros/getRunInfoFromCOOL.py -s --beg-run=303814 --end-run=303815
python PhysicsNtuple/PhysicsAnpRPC/macros/getHistFilesFromWeb.py -i /home/hengli/testarea/RPCPerf/PhysicsNtuple/PhysicsAnpRPC/data/run_info_2016.txt -o PhysicsNtuple/PhysicsAnpRPC/rpc-hists
python PhysicsNtuple/PhysicsAnpRPC/macros/plotRPCDetEff.py PhysicsNtuple/PhysicsAnpRPC/rpc-hists -i /home/hengli/testarea/RPCPerf/PhysicsNtuple/PhysicsAnpRPC/data/run_info_2016.txt -s -w --draw-all

