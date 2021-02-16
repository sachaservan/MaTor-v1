# MaTor(s) Tool (Version 1; 2014)
This is an old version of the MaTor tool. 
The original version of this code was obtained from [this repo](https://github.com/CheckResearch/confccsBackesKMM14_Experiment_01) where it is contained in a .zip file (don't ask me why). 
The original code *almost* works but needed some fixing to compile/run.
In addition to fixing those bugs, I've added some scripts (found in ```/scripts```) for downloading the Tor consensus files and constructing the database. 

After some tweaks and bug fixing, I got it to run on macOs (BigSur). 

## Installation

### Downloading the Tor data
First, we must download the necessary Tor data that is used by the MaTor tool to compute anonymity bounds.

The scripts in ```scripts/``` are used for this purpose. 

1. Run ```cd scripts```
2. Run ```python download_consensus.py -sm 01 -sy 2014 -em 12 -ey 2014``` (this will download all Tor consensus data from 2014)
3. Run ```python construct_database.py -sm 04 -em 04 -sy 2014 -ey 2014```

### Compiling MaTor 

To compile and build the MaTor executable (tested on macOS): 

1. Install dependencies: 
		- boost
		- sqlite3

2. To enable optimization type: 
	  ```export CXXFLAGS=-O2```

3. go to the build directory: 
    ```cd build```
	  ```cd [your_path]/MATor/build```
	  ```cmake .. ```
    ```make```

At this point the ```mator``` executable should be located in ```[your_path]/MATor/build/Release/bin/```

### Running MaTor 
You can now run ```./mator``` which will run a client using the consensus data specified in ```/src/config.cfg``` (defaults to 2014-02-05-03-00-00-consensus). 
Changing the config file requires recompiling the executable. 

### Running MaTor for plots
The ```mator``` executable can also be used to generate plot data. 
Run ```./mator largegraph``` or ```./mator graph``` to generate a text file (contained in the same directory) of the form: (# corruptions, anonymity bound delta). 



