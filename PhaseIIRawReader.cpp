#include "PhaseIIRawReader.h"

PhaseIIRawReader::PhaseIIRawReader():Tool(){}


bool PhaseIIRawReader::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  m_variables.Get("verbose",verbose);
  RawDataStore = new BoostStore(false,0);
  PMTDataStore=new BoostStore(false,2);
  TrigDataStore=new BoostStore(false,2);
  m_data->Stores["RunInfoStore"] = new BoostStore(false,0);
  FileOpen = false;
  TrigStoreEntries = 0;
  PMTEntries = 0;
  
  std::string inputfile;
  if (m_variables.Get("InputFile",inputfile)) {
    inputfiles.push(inputfile);
  }
  else if (m_variables.Get("InFileList",inputfile)) {
    std::ifstream filelist(inputfile.c_str());
    std::string fname;
    while (filelist >> fname) {
      inputfiles.push(fname);
      fname.clear();
    }
  }
  else {
    Log("No input file specified, quitting.");
    m_data->vars.Set("StopLoop",1);
    return false;
  }
  return true;
}

bool PhaseIIRawReader::OpenFile(std::string inputfile) { 
  Log("RawDataStore");
  currentFile = inputfile;
  Log("initrawstore");
  RawDataStore->Initialise(currentFile.c_str());
  Log("Print raw store");
  RawDataStore->Print(false);
  RawDataStore->Get("RunInformation",*m_data->Stores["RunInfoStore"]);
  m_data->Stores["RunInfoStore"]->Print(false);
  if (RawDataStore->Get("PMTData",*PMTDataStore)) {
    Log("Got PMTData");
    PMTDataStore->Header->Get("TotalEntries",PMTEntries); 
    cout << "PMTEntries " << PMTEntries << endl;
  }else{
    Log("Failed getting PMTData");
    PMTEntries = 0;
  }
  if (RawDataStore->Get("TrigData",*TrigDataStore)) {
    Log("Got TrigData");
    TrigDataStore->Header->Get("TotalEntries",TrigStoreEntries);
    cout << "TrigStoreEntries " << TrigStoreEntries << endl;
  }else{
    Log("Failed getting TrigStore.");
    TrigStoreEntries = 0;
  }
  currentPMTentry=0;
  currentTrigentry=0;
  FileOpen = true;
  return true;
}

bool PhaseIIRawReader::CloseFile() {
  Log("Close RunInfo");
  m_data->Stores["RunInfoStore"]->Close();
  m_data->Stores["RunInfoStore"]->Delete();
  Log("Close PMTDataStore");
  if (m_data->CStore.Has("CardDataVector")) m_data->CStore.Remove("CardDataVector");
  PMTDataStore->Close();
  PMTDataStore->Delete();
  Log("Close TrigDataStore");
  if (m_data->CStore.Has("TrigData")) m_data->CStore.Remove("TrigData");
  TrigDataStore->Close();
  TrigDataStore->Delete();
  Log("Close RawDataStore");
  RawDataStore->Close();
  Log("Delete RawDataStore");
  RawDataStore->Delete();
  Log("c++ delete RawDataStore");
  delete RawDataStore;
  Log("new RawDataStore");
  RawDataStore = new BoostStore(false,0);
  FileOpen = false;
  return true;
}


bool PhaseIIRawReader::Execute(){
  if (FileOpen && (currentTrigentry == TrigStoreEntries)&&(currentPMTentry == PMTEntries)) {
    CloseFile();
  }
  if (!FileOpen) {
    if (!inputfiles.empty()) {
      OpenFile(inputfiles.front());
      inputfiles.pop();
    }else{
      m_data->vars.Set("StopLoop",1);
      return true;
    }
  }
  if (currentPMTentry < PMTEntries) {
    Log("PMTDataStore");
    PMTDataStore->GetEntry(currentPMTentry);
    ++currentPMTentry;
    if (PMTDataStore->Has("CardData")) {
      std::vector<CardData> cdv;
      PMTDataStore->Get("CardData",cdv);
      m_data->CStore.Set("CardDataVector",cdv);
    }else{
      if (m_data->CStore.Has("CardDataVector")) m_data->CStore.Remove("CardDataVector");
    }
  }else{
    if (m_data->CStore.Has("CardDataVector")) m_data->CStore.Remove("CardDataVector");
  }
  if (currentTrigentry < TrigStoreEntries) {
    Log("TrigDataStore");
    TrigDataStore->GetEntry(currentTrigentry);
    ++currentTrigentry;
    if (TrigDataStore->Has("TrigData")) {
      TriggerData td;
      TrigDataStore->Get("TrigData",td);
      m_data->CStore.Set("TrigData",td);
    }else{
      if (m_data->CStore.Has("TrigData")) m_data->CStore.Remove("TrigData");
    }
  }else{
    if (m_data->CStore.Has("TrigData")) m_data->CStore.Remove("TrigData");
  }

  return true;
}


bool PhaseIIRawReader::Finalise(){
  if (FileOpen) {
    CloseFile();
  }
  delete m_data->Stores["RunInfoStore"];
  Log("Delete PMTDataStore");
  delete PMTDataStore;
  Log("Delete TrigDataStore");
  delete TrigDataStore;
  Log("Delete RawDataStore");
  RawDataStore->Delete();
  delete RawDataStore;
  Log("Done with Finalise()");
  return true;
}
