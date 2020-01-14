#ifndef PhaseIIRawReader_H
#define PhaseIIRawReader_H

#include <string>
#include <iostream>

#include "Tool.h"

#include "CardData.h"
#include "TriggerData.h"
#include "BoostStore.h"
#include "Store.h"
#include <queue>
#include <string>

#include <boost/serialization/vector.hpp>

/**
 * \class PhaseIIRawReader
 *
 * This is a blank template for a Tool used by the script to generate a new custom tool. Please fill out the description and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
* Contact: b.richards@qmul.ac.uk
*/
class PhaseIIRawReader: public Tool {


 public:

  PhaseIIRawReader(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resources. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Execute function used to perform Tool purpose.
  bool Finalise(); ///< Finalise function used to clean up resources.
  bool OpenFile(std::string);
  bool CloseFile();

 private:
  bool FileOpen;
  std::string currentFile;
  BoostStore *RawDataStore = nullptr;
  BoostStore *PMTDataStore = nullptr;
  BoostStore *TrigDataStore = nullptr;
  unsigned long PMTEntries;
  unsigned long TrigStoreEntries;
  unsigned long currentPMTentry;
  unsigned long currentTrigentry;
  int verbose;
  std::queue<std::string> inputfiles;



};


#endif
