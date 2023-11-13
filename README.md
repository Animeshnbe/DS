A simple relational algebra project for operations on tables and matrices:

# For Int_RA_Engine
## Compilation Instructions

We use ```make``` to compile all the files and creste the server executable. ```make``` is used primarily in Linux systems, so those of you who want to use Windows will probably have to look up alternatives (I hear there are ways to install ```make``` on Windows). To compile

```cd``` into the SimpleRA directory
```
cd SimpleRA
```
```cd``` into the soure directory (called ```src```)
```
cd src
```
To compile
```
make clean
make
```

## To run

Post compilation, an executable names ```server``` will be created in the ```src``` directory
```
./server
```

More information in docs/Overview.md and docs/Report.md with class diagrams.
Sample test data as csv files in data/

# For BP_Tree
## Compilation Instructions
cd to build/ and run `cmake build ..`
then run `make`


## To run
run `./BPTree`
- enter commands either through `SOURCE <filename>` or directly as `INSERT`, `DELETE` or `EXPORT` one by one.
- More info @ docs/bptree.pdf and data/analysis.pdf. Sample test files included in data/


If we have the fanout value = 'k', then a leaf node will consist of a maximum of
'k' Keys and 'k' corresponding Record pointers. Similarly, internal nodes will contain 'k-1'
keys and 'k' tree/block pointers when at maximum capacity