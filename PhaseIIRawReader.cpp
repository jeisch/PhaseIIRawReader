#include "PhaseIIRawReader.h"

PhaseIIRawReader::PhaseIIRawReader():Tool(){}


bool PhaseIIRawReader::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////

  m_variables.Get("verbose",verbose);
  m_variables.Get("InputFile",inputfile);
  
  Log("RawDataStore");
  RawDataStore = new BoostStore(false,0);
  RawDataStore->Initialise(inputfile.c_str());
  RawDataStore->Print(false);
  Log("RunInfoStore");
  m_data->Stores["RunInfoStore"] = new BoostStore(false,0);
  RawDataStore->Get("RunInformation",*m_data->Stores["RunInfoStore"]);
  m_data->Stores["RunInfoStore"]->Print(false);
  PMTDataStore=new BoostStore(false,2);
  if (RawDataStore->Get("PMTData",*PMTDataStore)) {
    Log("Got PMTData");
  }else{
    Log("failed getting PMTData");
    m_data->vars.Set("StopLoop",1);
    return false;
  }
  PMTDataStore->Header->Print(false); 
  PMTDataStore->Header->Get("TotalEntries",PMTEntries); 
  cout << "PMTEntries " << PMTEntries << endl;
  TrigDataStore=new BoostStore(false,2);
  RawDataStore->Get("TrigData",*TrigDataStore);
  TrigDataStore->Header->Get("TotalEntries",TrigEntries); 
  cout << "TrigEntries " << TrigEntries << endl;
  currentPMTentry=0;
  currentTrigentry=0;





  return true;
}


bool PhaseIIRawReader::Execute(){
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
  if (currentTrigentry < TrigEntries) {
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
  if ((currentTrigentry == TrigEntries)&&(currentPMTentry == PMTEntries)) {
    Log("StopLoop");
    m_data->vars.Set("StopLoop",1);
  }

  return true;
}


bool PhaseIIRawReader::Finalise(){
  Log("Close RunInfo");
  m_data->Stores["RunInfoStore"]->Close();
  Log("Delete RunInfoStore");
  delete m_data->Stores["RunInfoStore"];
  Log("Close PMTDataStore");
  PMTDataStore->Close();
  Log("Delete PMTDataStore");
  delete PMTDataStore;
  Log("Close TrigDataStore");
  TrigDataStore->Close();
  Log("Delete TrigDataStore");
  delete TrigDataStore;
  Log("Close RawDataStore");
  RawDataStore->Close();
  Log("Delete RawDataStore");
  delete RawDataStore;
  Log("Done with Finalise()");
  return true;
}
