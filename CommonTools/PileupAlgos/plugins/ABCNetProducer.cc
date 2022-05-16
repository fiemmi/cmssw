//========================================================================================
// Class:      ABCNetProducer                     ---------------------------------------
//----------------------------------------------------------------------------------------
/**\class ABCNetProducer ABCNetProducer.cc PileupAlgos/plugins/ABCNetProducer.cc
------------------------------------------------------------------------------------------
 Description: This class produces ABCNet weights to be used to mitigate pileup  ---
 -----------------------------------------------------------------------------------------
 Implementation:                                                                       ---
     This EDProducer is meant to be used with CMSSW >= 10_6_26                         ---
*/
//========================================================================================
// Authors:  Fabio Iemmi (IHEP)                                      ---------------------
//         Created:  THU, 5 May 2022 11:00:28 GMT  ---------------------------------------
//========================================================================================

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h" // for edm::ParameterSet
#include "DataFormats/PatCandidates/interface/PackedCandidate.h" //for pat
#include "DataFormats/Candidate/interface/Candidate.h" // for reco
#include "CommonTools/PileupAlgos/interface/PuppiCandidate.h"// for puppi candidates
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "PhysicsTools/TensorFlow/interface/TensorFlow.h" // to use TensorFlow
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"

#include "ABCNetMakeInputs.h"

#include <limits>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <memory>
#include <nlohmann/json.hpp>

using namespace abcnet;

struct ABCNetTFCache {
  ABCNetTFCache() : graphDef(nullptr) {}
  std::atomic<tensorflow::GraphDef*> graphDef;
};

///////////////////////////////////////////////////////////////////////////////////
// Declaring class ----------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////

class ABCNetProducer : public edm::stream::EDProducer<edm::GlobalCache<ABCNetTFCache>> {

public:
  typedef math::XYZTLorentzVector LorentzVector;
  typedef std::vector< pat::PackedCandidate > PackedOutputCollection;
  
  explicit ABCNetProducer(const edm::ParameterSet&, const ABCNetTFCache*);   
  ~ABCNetProducer();

  static void fillDescriptions(edm::ConfigurationDescriptions &);

  static std::unique_ptr<ABCNetTFCache> initializeGlobalCache(const edm::ParameterSet&);
  static void globalEndJob(const ABCNetTFCache*);

private:
  std::unique_ptr< PackedOutputCollection > fPackedPuppiCandidates;
  void produce(edm::Event &, const edm::EventSetup &) override;
  // tokens
  edm::EDGetTokenT<reco::CandidateView> tokenPFCandidates_;
};

//////////////////////////////////////////////////////////////////////////////////
// Defining methods --------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////

// constructors
ABCNetProducer::ABCNetProducer(const edm::ParameterSet& iConfig, const ABCNetTFCache* cache):
  tokenPFCandidates_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candName")))
{
  // Produce a ValueMap of floats linking each PF candidate with its ABCNet weight
  produces<edm::ValueMap<float> > ();
  produces<pat::PackedCandidateCollection>();
};

// destructor
ABCNetProducer::~ABCNetProducer() {
};

void ABCNetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {};

std::unique_ptr<ABCNetTFCache> ABCNetProducer::initializeGlobalCache(const edm::ParameterSet& iConfig) {
  tensorflow::setLogging("3");
  // get the pb file
  std::string pbFile = iConfig.getParameter<edm::FileInPath>("graph_path").fullPath();
  // load the graph definition and save it in the cache
  ABCNetTFCache* cache = new ABCNetTFCache();
  cache->graphDef = tensorflow::loadGraphDef(pbFile);
  return std::unique_ptr<ABCNetTFCache>(cache);
};

void ABCNetProducer::globalEndJob(const ABCNetTFCache* cache) {
  if (cache->graphDef != nullptr) {
    delete cache->graphDef;
  }
};

void ABCNetProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  // Get PFCandidate Collection
  edm::Handle<reco::CandidateView> PFCandidates;
  iEvent.getByToken(tokenPFCandidates_, PFCandidates);
  const reco::CandidateView *pfCol = PFCandidates.product();
  auto features = ABCNetMakeInputs::makeFeatureMap(pfCol, false);
  auto inputs = ABCNetMakeInputs::preprocess(features, false);
  //initialize container for ABCNet weights
  std::vector<float> weights;
  //throw random numbers in [0,1] as ABCNet weights for now
  srand(100);
  //for (unsigned int i = 0; i < pfCol->size(); i++) weights.push_back((float) rand()/RAND_MAX);
  std::vector<PuppiCandidate> lCandidates; // use PuppiCandidates
  int lPackCtr = 0;
  for(auto const& aPF : *pfCol) {  
      const pat::PackedCandidate *lPack = dynamic_cast<const pat::PackedCandidate*>(&aPF);
      float abcweight = -1.;
      if(lPack == nullptr) { 
        // throw error
        throw edm::Exception(edm::errors::LogicError,"ABCNetProducer: cannot get weights since inputs are not PackedCandidates");
      }
      else{
	abcweight = (float) rand()/RAND_MAX;
      }
      weights.push_back(abcweight);
      PuppiCandidate curjet;
      curjet.px = abcweight*lPack->px();
      curjet.py = abcweight*lPack->py();
      curjet.pz = abcweight*lPack->pz();
      curjet.e = abcweight*lPack->energy();
      curjet.pt = abcweight*lPack->pt();
      curjet.eta = lPack->eta();
      curjet.rapidity = lPack->rapidity();
      curjet.phi = lPack->phi();
      curjet.m = abcweight*lPack->mass();
      lCandidates.push_back(curjet);
      lPackCtr++;
   }
  fPackedPuppiCandidates.reset( new PackedOutputCollection );
  int cCand = 0;
  for ( auto const& aCand : *PFCandidates) {
    LorentzVector pVec;
    auto const& thisCand = lCandidates[cCand];
    pVec.SetPxPyPzE(thisCand.px,thisCand.py,thisCand.pz,thisCand.e);
    std::unique_ptr<pat::PackedCandidate> pCand;
    const pat::PackedCandidate *cand = dynamic_cast<const pat::PackedCandidate*>(&aCand);
    pCand.reset( new pat::PackedCandidate(*cand) );
    pCand->setP4(pVec);
    fPackedPuppiCandidates->push_back(*pCand);
    cCand++;
  }
  
  std::unique_ptr<edm::ValueMap<float>> ABCNetOut(new edm::ValueMap<float>());
  edm::ValueMap<float>::Filler  ABCNetFiller(*ABCNetOut);
  ABCNetFiller.insert(PFCandidates,weights.begin(),weights.end());
  ABCNetFiller.fill();
  
  //iEvent.put(std::move(ABCNetOut), "weights");
  iEvent.put(std::move(fPackedPuppiCandidates));

}
//define this as a plug-in
DEFINE_FWK_MODULE(ABCNetProducer);
