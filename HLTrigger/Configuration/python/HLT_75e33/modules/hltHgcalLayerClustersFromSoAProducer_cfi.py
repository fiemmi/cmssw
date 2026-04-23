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

#FIemmi: serial clone of LayerClustersFromSoAProducer
hltHgCalLayerClustersFromSoAProducer = makeSerialClone(hltHgCalLayerClustersFromSoAProducer
                                                       #feed upstream serial modules in
                                                       hgcalRecHitsLayerClustersSoA = "hltHgcalSoARecHitsLayerClustersProducerSerialSync",
                                                       hgcalRecHitsSoA = "hltHgcalSoARecHitsProducerSerialSync"
)
