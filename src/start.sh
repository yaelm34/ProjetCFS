make clean 
make
cd test1
make clean
make
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./main_do im

