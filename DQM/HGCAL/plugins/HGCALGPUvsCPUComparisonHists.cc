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
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterCollection.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"

class HGCALGPUvsCPUComparisonHists : public DQMEDAnalyzer {

public:
  explicit HGCALGPUvsCPUComparisonHists(const edm::ParameterSet&);
  ~HGCALGPUvsCPUComparisonHists() override;

protected:
  void beginJob(const edm::EventSetup& iSetup);
  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;
  void bookHistograms(DQMStore::IBooker& iBooker, edm::Run const& iRun, edm::EventSetup const& iSetup) override;
  
private:
  edm::EDGetTokenT<reco::CaloClusterCollection> tokenMonitoredLayerClusters_;
  edm::EDGetTokenT<reco::CaloClusterCollection> tokenReferenceLayerClusters_;
  MonitorElement* hLayerCluster_x;
  MonitorElement* hLayerCluster_y;
  MonitorElement* hLayerCluster_z;
  MonitorElement* hLayerCluster_eta;
  MonitorElement* hLayerCluster_phi;
  MonitorElement* hLayerCluster_e;
  MonitorElement* hLayerCluster_nRecHits;
  MonitorElement* hLayerCluster2D_x;
  MonitorElement* hLayerCluster2D_y;
  MonitorElement* hLayerCluster2D_z;
  MonitorElement* hLayerCluster2D_eta;
  MonitorElement* hLayerCluster2D_phi;
  MonitorElement* hLayerCluster2D_e;
  MonitorElement* hLayerCluster2D_nRecHits;
};

HGCALGPUvsCPUComparisonHists::HGCALGPUvsCPUComparisonHists(const edm::ParameterSet& iConfig)
  : tokenMonitoredLayerClusters_(consumes<reco::CaloClusterCollection>(iConfig.getParameter<edm::InputTag>("monitoredLayerClusters"))),
    tokenReferenceLayerClusters_(consumes<reco::CaloClusterCollection>(iConfig.getParameter<edm::InputTag>("referenceLayerClusters")))
{
}

HGCALGPUvsCPUComparisonHists::~HGCALGPUvsCPUComparisonHists() {
}

void HGCALGPUvsCPUComparisonHists::beginJob(const edm::EventSetup& iSetup) {}

void HGCALGPUvsCPUComparisonHists::bookHistograms(DQMStore::IBooker& iBooker, edm::Run const&, edm::EventSetup const&) {

  iBooker.setCurrentFolder("HGCAL");
  //For a given variable x, 1D plots store Delta(x), 2D plots show x_GPU vs x_CPU
  //1D
  hLayerCluster_x = iBooker.book1D("hLayerCluster_x", "hLayerCluster_x", 100, -0.01, 0.01);
  hLayerCluster_y = iBooker.book1D("hLayerCluster_y", "hLayerCluster_y", 100, -0.01, 0.01);
  hLayerCluster_z = iBooker.book1D("hLayerCluster_z", "hLayerCluster_z", 100, -0.01, 0.01);
  hLayerCluster_eta = iBooker.book1D("hLayerCluster_eta", "hLayerCluster_eta", 100, -0.01, 0.01);
  hLayerCluster_phi = iBooker.book1D("hLayerCluster_phi", "hLayerCluster_phi", 100, -0.01, 0.01);
  hLayerCluster_e = iBooker.book1D("hLayerCluster_e", "hLayerCluster_e", 100, -0.01, 0.01);
  hLayerCluster_nRecHits = iBooker.book1D("hLayerCluster_nRecHits", "hLayerCluster_nRecHits", 100, -0.01, 0.01);
  //2D
  hLayerCluster2D_x = iBooker.book2D("hLayerCluster2D_x", "hLayerCluster2D_x", 200, -50, 50, 200, -50, 50);
  hLayerCluster2D_y = iBooker.book2D("hLayerCluster2D_y", "hLayerCluster2D_y", 200, -50, 50, 200, -50, 50);
  hLayerCluster2D_z = iBooker.book2D("hLayerCluster2D_z", "hLayerCluster2D_z", 250, -500, 500, 250, -500, 500);
  hLayerCluster2D_eta = iBooker.book2D("hLayerCluster2D_eta", "hLayerCluster2D_eta", 75, 1.0, 3.5, 75, 1.0, 3.5);
  hLayerCluster2D_phi = iBooker.book2D("hLayerCluster2D_phi", "hLayerCluster2D_phi", 200, -3.5, 3.5, 200, -3.5, 3.5);
  hLayerCluster2D_e = iBooker.book2D("hLayerCluster2D_e", "hLayerCluster2D_e", 100, 0, 30, 100, 0, 30);
  hLayerCluster2D_nRecHits = iBooker.book2D("hLayerCluster2D_nRecHits", "hLayerCluster2D_nRecHits", 60, 0, 60, 60, 0, 60);
}

void HGCALGPUvsCPUComparisonHists::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

  //Get monitored (GPU) and reference (CPU) LayerCluster collections
  edm::Handle<reco::CaloClusterCollection> monitoredLayerClusters_, referenceLayerClusters_;
  iEvent.getByToken(tokenMonitoredLayerClusters_, monitoredLayerClusters_);
  iEvent.getByToken(tokenReferenceLayerClusters_, referenceLayerClusters_);
  const std::vector<reco::CaloCluster>* monitoredLayerClusters = monitoredLayerClusters_.product();
  const std::vector<reco::CaloCluster>* referenceLayerClusters = referenceLayerClusters_.product();

  //look for GPU and CPU LayerClusters whose seeds match
  for (unsigned int i = 0; i < monitoredLayerClusters->size(); i++) {
    unsigned monitoredIdx = 0, referenceIdx = 0; 
    int matchCounter = 0;
    for (unsigned int j = 0; j < referenceLayerClusters->size(); j++) {
      if (monitoredLayerClusters->at(i).seed() == referenceLayerClusters->at(j).seed()) {
	monitoredIdx = i;
	referenceIdx = j;
	matchCounter++;
      }
    }
    if (matchCounter == 1) {
      hLayerCluster_x->Fill(monitoredLayerClusters->at(monitoredIdx).x() - referenceLayerClusters->at(referenceIdx).x());
      hLayerCluster_y->Fill(monitoredLayerClusters->at(monitoredIdx).y() - referenceLayerClusters->at(referenceIdx).y());
      hLayerCluster_z->Fill(monitoredLayerClusters->at(monitoredIdx).z() - referenceLayerClusters->at(referenceIdx).z());
      hLayerCluster_eta->Fill(monitoredLayerClusters->at(monitoredIdx).eta() - referenceLayerClusters->at(referenceIdx).eta());
      hLayerCluster_phi->Fill(monitoredLayerClusters->at(monitoredIdx).phi() - referenceLayerClusters->at(referenceIdx).phi());
      hLayerCluster_e->Fill(monitoredLayerClusters->at(monitoredIdx).energy() - referenceLayerClusters->at(referenceIdx).energy());
      hLayerCluster_nRecHits->Fill(monitoredLayerClusters->at(monitoredIdx).size() - referenceLayerClusters->at(referenceIdx).size());

      hLayerCluster2D_x->Fill(referenceLayerClusters->at(referenceIdx).x(), monitoredLayerClusters->at(monitoredIdx).x());
      hLayerCluster2D_y->Fill(referenceLayerClusters->at(referenceIdx).y(), monitoredLayerClusters->at(monitoredIdx).y());
      hLayerCluster2D_z->Fill(referenceLayerClusters->at(referenceIdx).z(), monitoredLayerClusters->at(monitoredIdx).z());
      hLayerCluster2D_eta->Fill(referenceLayerClusters->at(referenceIdx).eta(), monitoredLayerClusters->at(monitoredIdx).eta());
      hLayerCluster2D_phi->Fill(referenceLayerClusters->at(referenceIdx).phi(), monitoredLayerClusters->at(monitoredIdx).phi());
      hLayerCluster2D_e->Fill(referenceLayerClusters->at(referenceIdx).energy(), monitoredLayerClusters->at(monitoredIdx).energy());
      hLayerCluster2D_nRecHits->Fill(referenceLayerClusters->at(referenceIdx).size(), monitoredLayerClusters->at(monitoredIdx).size());
      }
      else {
	edm::LogWarning("HGCALGPUvsCPUComparisonHists") << "Found duplicate match. Won't fill histogram for this event.";
	return;
      }
  }
}


DEFINE_FWK_MODULE(HGCALGPUvsCPUComparisonHists);
