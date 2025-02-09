#ifndef CondTools_SiPixel_SiPixelCondObjBuilder_H
#define CondTools_SiPixel_SiPixelCondObjBuilder_H
// -*- C++ -*-
//
// Package:    SiPixelCondObjBuilder
// Class:      SiPixelCondObjBuilder
//
/**\class SiPixelCondObjBuilder SiPixelCondObjBuilder.h SiPixel/test/SiPixelCondObjBuilder.h

 Description: Test analyzer for writing pixel calibration in the DB

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Vincenzo CHIOCHIA
//         Created:  Tue Oct 17 17:40:56 CEST 2006
// $Id: SiPixelCondObjBuilder.h,v 1.9 2009/05/28 22:12:54 dlange Exp $
//
//
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
//#include "CondFormats/SiPixelObjects/interface/SiPixelGainCalibration.h"
#include "CalibTracker/SiPixelESProducers/interface/SiPixelGainCalibrationService.h"
#include "CondFormats/SiPixelObjects/interface/PixelIndices.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include <string>

namespace cms {
  class SiPixelCondObjBuilder : public edm::one::EDAnalyzer<> {
  public:
    explicit SiPixelCondObjBuilder(const edm::ParameterSet& iConfig);

    ~SiPixelCondObjBuilder() override = default;
    void beginJob() override;
    void analyze(const edm::Event&, const edm::EventSetup&) override;
    bool loadFromFile();

  private:
    edm::ParameterSet conf_;
    bool appendMode_;
    edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> pddToken_;
    SiPixelGainCalibration* SiPixelGainCalibration_;
    SiPixelGainCalibrationService SiPixelGainCalibrationService_;
    std::string recordName_;

    double meanPed_;
    double rmsPed_;
    double meanGain_;
    double rmsGain_;
    double secondRocRowGainOffset_;
    double secondRocRowPedOffset_;
    int numberOfModules_;
    bool fromFile_;
    std::string fileName_;

    // Internal class
    class CalParameters {
    public:
      float p0;
      float p1;
    };
    // Map for storing calibration constants
    std::map<int, CalParameters, std::less<int> > calmap_;
    PixelIndices* pIndexConverter_;  // Pointer to the index converter
  };
}  // namespace cms
#endif
