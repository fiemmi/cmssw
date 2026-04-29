import FWCore.ParameterSet.Config as cms

hltHGCALGPUvsCPUComparisonHists = cms.EDProducer("HGCALGPUvsCPUComparisonHists",
                                                    #FIemmi: add params here when analyzer is developed
                                                    testInputString = cms.string("TEST INPUT STRING"),

                                                    )
