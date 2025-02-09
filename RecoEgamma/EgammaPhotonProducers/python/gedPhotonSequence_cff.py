import FWCore.ParameterSet.Config as cms

#
# sequence to make photons from clusters in ECAL
#
# photon producer
from RecoEgamma.EgammaPhotonProducers.gedPhotonCore_cfi import *
from RecoEgamma.EgammaPhotonProducers.gedPhotons_cfi import *

import RecoEgamma.EgammaPhotonProducers.gedPhotons_cfi 

gedPhotonsTmp = RecoEgamma.EgammaPhotonProducers.gedPhotons_cfi.gedPhotons.clone(
    photonProducer         = "gedPhotonCore",
    candidateP4type        = "fromEcalEnergy",
    outputPhotonCollection = "",
    reconstructionStep     = "tmp",
    PhotonDNNPFid = dict(
        modelsFiles = [ "RecoEgamma/PhotonIdentification/data/Photon_PFID_dnn/EB/EB_modelDNN.pb",
                        "RecoEgamma/PhotonIdentification/data/Photon_PFID_dnn/EE/EE_modelDNN.pb"],
        scalersFiles = [
                    "RecoEgamma/PhotonIdentification/data/Photon_PFID_dnn/EB/EB_scaler.txt",
                    "RecoEgamma/PhotonIdentification/data/Photon_PFID_dnn/EE/EE_scaler.txt"]
    )
)
del gedPhotonsTmp.regressionConfig

gedPhotonTaskTmp = cms.Task(gedPhotonCore, gedPhotonsTmp)
gedPhotonSequenceTmp = cms.Sequence(gedPhotonTaskTmp)

gedPhotons = RecoEgamma.EgammaPhotonProducers.gedPhotons_cfi.gedPhotons.clone(
    photonProducer         = "gedPhotonsTmp",
    outputPhotonCollection = "",
    reconstructionStep     = "final",
    pfECALClusIsolation    = cms.InputTag("photonEcalPFClusterIsolationProducer"),
    pfHCALClusIsolation    = cms.InputTag("photonHcalPFClusterIsolationProducer"),
    pfIsolCfg = cms.PSet(
        chargedHadronIso = cms.InputTag("photonIDValueMaps","phoChargedIsolation"),
        neutralHadronIso = cms.InputTag("photonIDValueMaps","phoNeutralHadronIsolation"),
        photonIso        = cms.InputTag("photonIDValueMaps","phoPhotonIsolation"),
        chargedHadronWorstVtxIso = cms.InputTag("photonIDValueMaps","phoWorstChargedIsolation"),
        chargedHadronWorstVtxGeomVetoIso = cms.InputTag("photonIDValueMaps","phoWorstChargedIsolationConeVeto"),
        chargedHadronPFPVIso     = cms.InputTag("egmPhotonIsolationCITK:h+-DR030-"),
        ),
)
gedPhotonTask    = cms.Task(gedPhotons)
gedPhotonSequence    = cms.Sequence(gedPhotonTask)

from Configuration.ProcessModifiers.egamma_lowPt_exclusive_cff import egamma_lowPt_exclusive
egamma_lowPt_exclusive.toModify(gedPhotons,
                           minSCEtBarrel = 1.0,
                           minSCEtEndcap = 1.0)
egamma_lowPt_exclusive.toModify(gedPhotonsTmp,
                           minSCEtBarrel = 1.0,
                           minSCEtEndcap = 1.0)


# Activate the Egamma PFID dnn only for Run3
from Configuration.Eras.Modifier_run3_common_cff import run3_common
run3_common.toModify(gedPhotonsTmp.PhotonDNNPFid,
    enabled = True
)