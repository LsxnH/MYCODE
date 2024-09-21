GetEnv = false
Executable = /home/hengli/testarea/RPCPerf/runcondor.sh 
#Arguments = $(Process)
LOCATION=/moose/AtlUser/liheng/LOGS/rpc
Output =  $(LOCATION)/rpc_1_$(Process).out
Log =  $(LOCATION)/rpc_1_$(Process).log
Error =$(LOCATION)/rpc_1_$(Process).err
Queue 
#Queue 100

