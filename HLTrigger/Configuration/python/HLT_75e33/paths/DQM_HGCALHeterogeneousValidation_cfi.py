import FWCore.ParameterSet.Config as cms

from ..sequences.HLTBeginSequence_cfi import *
from ..sequences.HLTDQMHGCALReconstruction_cfi import *
from .DQM_TRKHeterogeneousValidation_cfi import *

DQM_HGCALHeterogeneousValidation = cms.Path(
    HLTBeginSequence
    + hltBackend
    + hltStatusOnGPUFilter
    + HLTDQMHGCALReconstruction
)
