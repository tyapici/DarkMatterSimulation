
#include "Pythia8/Pythia.h"
#include <iostream>
#include <fstream>
#include <string>

#include <stdio.h>
#include <string.h>

using namespace std;

using namespace Pythia8;

int main(int argc, char * argv[]) {

  std::string filename = argv[1];
  std::string runnumber = argv[2];
  double numofevents = atof(argv[3]);
  double percent = atof(argv[4]);
  std::string outpath = "./";
  
  if(argc<4) {
    std::cout << " error in number of input arguments " << std::endl;
    return -1;
  }

  //double ECM = 20000.; // check with the cfg file since this number is there too!!!
  double ECM = 0;

  //setting up the generation
  Pythia pythia("/data/disk01/home/tyapici/software/pythia8/pythia8205-install/share/Pythia8/xmldoc/");

  // pythia8 config file
  std::string line;
  std::string delimiter="eCM=";
  size_t pos = 0;
  std::ifstream cfgFile(filename.c_str());
  while(std::getline(cfgFile, line)) {
    std::istringstream iss(line);
    pythia.readString(line);
    if ((pos = line.find(delimiter)) != std::string::npos) {
      while ((pos = line.find(delimiter)) != std::string::npos) {
	line.erase(0, pos + delimiter.length());
      }
      ECM = atof(line.c_str());
    }
  }

  double e_threshold = (1-percent)*ECM/2.;

  char tmpname[500];
  //strcpy(tmpname, outpath.c_str());
  //strcat(tmpname, "dmenergytemp_runNo:");
  strcpy(tmpname, filename.c_str());
  strcat(tmpname, "_histoinfo.txt");
  char outname[500];
  //strcpy(outname, outpath.c_str());
  //strcat(outname, "dmenergy_");
  strcpy(outname, filename.c_str());
  strcat(outname, "_processed.txt");

  std::cout << tmpname << std::endl;
  std::cout << outname << std::endl;

  int i;
  int j;
  int k;
  int num=0;
 
  pythia.init();
  Hist Z( "photon energy spectrum", 1000, -2.0, log10(ECM/2.0)); 
 
  // the event loop and generation 
  
  for (i=1; i<numofevents; ++i) { 
    pythia.next();
    // check the first 10 particles in the stream
    for (k=1;k<=10; ++k) {      
      // if one of the first 10 particles is Z', then
      // it is worth saving this event
      if ( (pythia.event[k].id()==32) ) { 
	// check the energy of the particle
	if ( pythia.event[k].e()>=e_threshold ) {
	  num += 1;
	  // loop over every particle
	  for (j=1;j<pythia.event.size(); ++j) 
	    {
	      // record if the particle won't decay anymore and 
	      // if the particle is a photon
	      if ( (pythia.event[j].statusHepMC()==1) && (pythia.event[j].id()==22) )
		Z.fill(log10(pythia.event[j].eCalc()));
	    }
	}
      }
    }
  }
  cout << "usable events:" << num;
  //energy->Draw();              
  //save hist on file and close file
  //energy->Write();    
  //writing the data to a file
  //myfile.open("dmenergy.txt");
  //Z.operator*=(1.0/numofevents);
  Z.table(tmpname);//dump the histogram to a file
  // setting up the input and poutp 
  ofstream outputfile;
  ifstream inputFile;
  inputFile.open(tmpname,ios::in);
  outputfile.open(outname,ios::out);
  inputFile.exceptions(ifstream::goodbit);
  const char separator    = ' ';
  const int  nameWidth     = 15;
  double     dn,nhits,energy,en;
  //const int numWidth      = 15;

  outputfile << left << num << "\n";
  outputfile << left << setw(nameWidth) << setfill(separator) << "E (GeV)\t";
  outputfile << left << setw(nameWidth) << setfill(separator) << "dN-avg\t"<<"\n";
  while(inputFile>>en>>nhits){
    energy=pow(10,en);
    dn = nhits/num;
    outputfile << left << setw(nameWidth) << setfill(separator) << energy<<"\t";
    outputfile << left << setw(nameWidth) << setfill(separator) << dn<<"\t"<<"\n";
  }
  outputfile.close();
  return 0;
}
