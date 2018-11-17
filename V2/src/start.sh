make
cd test5
make clean
make
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./main_do MonNouveauDisque.disk

