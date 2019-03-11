SJM, Cloned from [SJM](https://github.com/StanfordBioinformatics/SJM) with the following modifications  
1. remove boost dependency, use embeded [CLI](https://github.com/CLIUtils/CLI11) instead  
2. remove TCLAP dependency, use std::regex instead  
3. remove perl scripts run_with_env, keep perl scripts away  
4. allow compile and executaion under environment without sge/lfs
5. support local job submit and monitoring natively if no sge/lfs found  

Installation && usage 
1. clone repo  
   `git clone https://github.com/vanNul/SJM.git`
2. compile  
   `cd SJM`  
   `./autogen.sh`  
   `./configure --prefix=/path/to/install/dir/`  
   `make`  
   `make install`  
3. usage  
   prepare a sjm jobfile such as [example.sjm](https://github.com/vanNul/SJM/tree/master/doc/example.sjm) and run  
   `sjm example.sjm`  
   you'd better check [manual](https://github.com/vanNul/SJM/tree/master/doc/MANUAL.txt) for more help 
