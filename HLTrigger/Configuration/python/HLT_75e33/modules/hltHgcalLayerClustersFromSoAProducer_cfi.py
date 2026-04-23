import FWCore.ParameterSet.Config as cms
from HeterogeneousCore.AlpakaCore.functions import makeSerialClone

hltHgCalLayerClustersFromSoAProducer = cms.EDProducer("HGCalLayerClustersFromSoAProducer",
    detector = cms.string('EE'),
    hgcalRecHitsLayerClustersSoA = cms.InputTag("hltHgcalSoARecHitsLayerClustersProducer"),
    hgcalRecHitsSoA = cms.InputTag("hltHgcalSoARecHitsProducer"),
    nHitsTime = cms.uint32(3),
    src = cms.InputTag("hltHgcalSoALayerClustersProducer"),
    timeClname = cms.string('timeLayerCluster')
)

#FIemmi: serial version of LayerClustersFromSoAProducer (takes serial inputs)
hltHgCalLayerClustersFromSoAProducerSerialSync = cms.EDProducer("HGCalLayerClustersFromSoAProducer",
    detector = cms.string('EE'),
    hgcalRecHitsLayerClustersSoA = cms.InputTag("hltHgcalSoARecHitsLayerClustersProducerSerialSync"),
    hgcalRecHitsSoA = cms.InputTag("hltHgcalSoARecHitsProducerSerialSync"),
    nHitsTime = cms.uint32(3),
    src = cms.InputTag("hltHgcalSoALayerClustersProducerSerialSync"),
    timeClname = cms.string('timeLayerCluster')
)
