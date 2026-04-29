//============================================================================================================================
// Class:      HGCALGPUvsCPUComparisonHists                     --------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------
/**\class HGCALGPUvsCPUComparisonHists HGCALGPUvsCPUComparisonHists.cc DQM/HGCAL/plugins/HGCALGPUvsCPUComparisonHists.cc
------------------------------------------------------------------------------------------------------------------------------
 Description: This class produces histograms to compare GPU- and CPU-based HGCAL reconstruction  ---
 -----------------------------------------------------------------------------------------------------------------------------
 Implementation:                                                                          ---
     This DQMEDAnalyzer is meant to be used with CMSSW >= 16_1_0                          ---
*/
//========================================================================================
// Authors:  Fabio Iemmi (IHEP)                                      ---------------------
//         Created:  TUE, 28 Arp 2026 16:05:28 GMT  --------------------------------------
//========================================================================================

#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"

class HGCALGPUvsCPUComparisonHists : public DQMEDAnalyzer {

public:
  explicit HGCALGPUvsCPUComparisonHists(const edm::ParameterSet&);
  ~HGCALGPUvsCPUComparisonHists() override;

protected:
  void beginJob(const edm::EventSetup& iSetup);
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  void bookHistograms(DQMStore::IBooker& iBooker, edm::Run const& iRun, edm::EventSetup const& iSetup) override;
  
private:
  std::string test_input_string;
};

HGCALGPUvsCPUComparisonHists::HGCALGPUvsCPUComparisonHists(const edm::ParameterSet& iConfig)
  : test_input_string(iConfig.getParameter<std::string>("testInputString"))
{
  std::cout << "Calling constructor" << std::endl;
  std::cout << "Input test string received from config file is " << test_input_string << std::endl;
}

HGCALGPUvsCPUComparisonHists::~HGCALGPUvsCPUComparisonHists() {
}

void HGCALGPUvsCPUComparisonHists::beginJob(const edm::EventSetup& iSetup) {}

void HGCALGPUvsCPUComparisonHists::bookHistograms(DQMStore::IBooker& iBooker, edm::Run const&, edm::EventSetup const&) {

  iBooker.setCurrentFolder("HGCAL/SomeFolder");

}

void HGCALGPUvsCPUComparisonHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

}

DEFINE_FWK_MODULE(HGCALGPUvsCPUComparisonHists);
