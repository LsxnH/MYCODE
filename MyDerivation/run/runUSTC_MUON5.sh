#!/bin/bash

#===================================================================================
# This macro will use subCERN.py python script to prepare batch jobs for fakestudy 
# generating and preparing histograms.
#
# Usage for preparing histograms :
#   $ source runUSTC_MUON5.sh Anp_v3 ""
#
#===================================================================================

(
# input data/mc ntuples for generating mini-ntuples in step 1.
testarea="${HOME}/testarea/MyDerivation"

input_ntuple_list="${testarea}/run/ZmmPP8AOD.txt"

# path of AnpBatch package
pathBatch="/home/hengli/testarea/AnpBatch"

#===================================================================================
run_minintp()
{
    exe="python ${pathBatch}/macros/prepJobs.py"
    pre_macro="${pathBatch}/macros/procJob.py"
    run_macro="Reco_tf.py"

    opt_exe="-q 8nh -n $4"
    opt_pre_macro="--max-nfile=10,--wait-good-copy=5"
    opt_run_macro="--outputDAODFile out.root --reductionConf MUON5 $5"

    inp="$1"
    tag="$2"
    version="$3"
    outdir="MUON5_DAOD-"`date +%Y-%m-%d`"-${tag}_${version}"

    # we will save output hist-roots at "/luster/" directory
    lstout="/lustre/AtlUser/liheng/data/DxAOD/${outdir}"

    # path of submit scripts
    config="/lustre/AtlUser/liheng/data/DxAOD/config-${tag}_${version}"

    $exe $inp --job-config-dir=${config} \
              --job-output-dir=${lstout} \
              --do-condor \
              --proc-macro=${pre_macro} \
              --setup=${testarea}/source/setup_atlas_analysis_release.sh \
              --command="${run_macro} --inputAODFile local_input/* ${opt_run_macro}" \
              ${opt_exe} \
              --proc-prep-arg="${opt_pre_macro}"

}

#===================================================================================

    echo '----------------------------------------------------------------------------'
    echo "Executing runCERN_MUON5.sh script"
    echo "Current date: "`date`
    echo "Current host: "`hostname`
    echo "Current wdir: "`pwd`
    echo "Arguments: \""`echo $@`"\""

    run_minintp "$input_ntuple_list" "MUON5_DAOD" "$1" "10000" "$2"

)
