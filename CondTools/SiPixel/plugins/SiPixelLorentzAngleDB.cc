#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include "SiPixelLorentzAngleDB.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"
#include "Geometry/CommonDetUnit/interface/PixelGeomDetUnit.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelLorentzAngle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementError.h"
#include "DataFormats/GeometrySurface/interface/GloballyPositioned.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"

using namespace std;
using namespace edm;

//Constructor

SiPixelLorentzAngleDB::SiPixelLorentzAngleDB(edm::ParameterSet const& conf)
    : tkGeomToken_(esConsumes()), tkTopoToken_(esConsumes()), conf_(conf) {
  magneticField_ = conf_.getParameter<double>("magneticField");
  recordName_ = conf_.getUntrackedParameter<std::string>("record", "SiPixelLorentzAngleRcd");
  useFile_ = conf_.getParameter<bool>("useFile");
  fileName_ = conf_.getParameter<string>("fileName");

  BPixParameters_ = conf_.getUntrackedParameter<Parameters>("BPixParameters");
  FPixParameters_ = conf_.getUntrackedParameter<Parameters>("FPixParameters");
  ModuleParameters_ = conf_.getUntrackedParameter<Parameters>("ModuleParameters");
}

// Virtual destructor needed.
SiPixelLorentzAngleDB::~SiPixelLorentzAngleDB() = default;

// Analyzer: Functions that gets called by framework every event

void SiPixelLorentzAngleDB::analyze(const edm::Event& e, const edm::EventSetup& es) {
  SiPixelLorentzAngle* LorentzAngle = new SiPixelLorentzAngle();

  //Retrieve tracker topology from geometry
  const TrackerTopology* tTopo = &es.getData(tkTopoToken_);

  //Retrieve old style tracker geometry from geometry
  const TrackerGeometry* pDD = &es.getData(tkGeomToken_);
  edm::LogInfo("SiPixelLorentzAngle (old)")
      << " There are " << pDD->detUnits().size() << " detectors (old)" << std::endl;

  for (const auto& it : pDD->detUnits()) {
    if (dynamic_cast<PixelGeomDetUnit const*>(it) != nullptr) {
      DetId detid = it->geographicalId();
      const DetId detidc = it->geographicalId();

      // fill bpix values for LA
      if (detid.subdetId() == static_cast<int>(PixelSubdetector::PixelBarrel)) {
        edm::LogPrint("SiPixelLorentzAngleDB")
            << " pixel barrel:"
            << "  layer=" << tTopo->pxbLayer(detidc.rawId()) << "  ladder=" << tTopo->pxbLadder(detidc.rawId())
            << "  module=" << tTopo->pxbModule(detidc.rawId()) << "  rawId=" << detidc.rawId() << endl;

        if (!useFile_) {
          //first individuals are put
          for (Parameters::iterator it = ModuleParameters_.begin(); it != ModuleParameters_.end(); ++it) {
            if (it->getParameter<unsigned int>("rawid") == detidc.rawId()) {
              float lorentzangle = (float)it->getParameter<double>("angle");
              LorentzAngle->putLorentzAngle(detid.rawId(), lorentzangle);
              edm::LogPrint("SiPixelLorentzAngleDB")
                  << " individual value=" << lorentzangle << " put into rawid=" << detid.rawId() << endl;
            }
          }

          //modules already put are automatically skipped
          for (Parameters::iterator it = BPixParameters_.begin(); it != BPixParameters_.end(); ++it) {
            if (it->getParameter<unsigned int>("module") == tTopo->pxbModule(detidc.rawId()) &&
                it->getParameter<unsigned int>("layer") == tTopo->pxbLayer(detidc.rawId())) {
              float lorentzangle = (float)it->getParameter<double>("angle");
              LorentzAngle->putLorentzAngle(detid.rawId(), lorentzangle);
            }
          }

        } else {
          edm::LogError("SiPixelLorentzAngleDB")
              << "[SiPixelLorentzAngleDB::analyze] method for reading file not implemented yet" << std::endl;
        }

        // fill fpix values for LA
      } else if (detid.subdetId() == static_cast<int>(PixelSubdetector::PixelEndcap)) {
        edm::LogPrint("SiPixelLorentzAngleDB")
            << " pixel endcap:"
            << "  side=" << tTopo->pxfSide(detidc.rawId()) << "  disk=" << tTopo->pxfDisk(detidc.rawId())
            << "  blade=" << tTopo->pxfBlade(detidc.rawId()) << "  panel=" << tTopo->pxfPanel(detidc.rawId())
            << "  module=" << tTopo->pxfModule(detidc.rawId()) << "  rawId=" << detidc.rawId() << endl;

        //first individuals are put
        for (Parameters::iterator it = ModuleParameters_.begin(); it != ModuleParameters_.end(); ++it) {
          if (it->getParameter<unsigned int>("rawid") == detidc.rawId()) {
            float lorentzangle = (float)it->getParameter<double>("angle");
            LorentzAngle->putLorentzAngle(detid.rawId(), lorentzangle);
            edm::LogPrint("SiPixelLorentzAngleDB")
                << " individual value=" << lorentzangle << " put into rawid=" << detid.rawId() << endl;
          }
        }

        //modules already put are automatically skipped
        for (Parameters::iterator it = FPixParameters_.begin(); it != FPixParameters_.end(); ++it) {
          if (it->getParameter<unsigned int>("side") == tTopo->pxfSide(detidc.rawId()) &&
              it->getParameter<unsigned int>("disk") == tTopo->pxfDisk(detidc.rawId()) &&
              it->getParameter<unsigned int>("HVgroup") ==
                  HVgroup(tTopo->pxfPanel(detidc.rawId()), tTopo->pxfModule(detidc.rawId()))) {
            float lorentzangle = (float)it->getParameter<double>("angle");
            LorentzAngle->putLorentzAngle(detid.rawId(), lorentzangle);
          }
        }

      } else {
        edm::LogError("SiPixelLorentzAngleDB")
            << "[SiPixelLorentzAngleDB::analyze] detid is Pixel but neither bpix nor fpix" << std::endl;
      }
    }
  }

  edm::Service<cond::service::PoolDBOutputService> mydbservice;
  if (mydbservice.isAvailable()) {
    try {
      if (mydbservice->isNewTagRequest(recordName_)) {
        mydbservice->createNewIOV<SiPixelLorentzAngle>(
            LorentzAngle, mydbservice->beginOfTime(), mydbservice->endOfTime(), recordName_);
      } else {
        mydbservice->appendSinceTime<SiPixelLorentzAngle>(LorentzAngle, mydbservice->currentTime(), recordName_);
      }
    } catch (const cond::Exception& er) {
      edm::LogError("SiPixelLorentzAngleDB") << er.what() << std::endl;
    } catch (const std::exception& er) {
      edm::LogError("SiPixelLorentzAngleDB") << "caught std::exception " << er.what() << std::endl;
    } catch (...) {
      edm::LogError("SiPixelLorentzAngleDB") << "Funny error" << std::endl;
    }
  } else {
    edm::LogError("SiPixelLorentzAngleDB") << "Service is unavailable" << std::endl;
  }
}

unsigned int SiPixelLorentzAngleDB::HVgroup(unsigned int panel, unsigned int module) {
  if (1 == panel && (1 == module || 2 == module)) {
    return 1;
  } else if (1 == panel && (3 == module || 4 == module)) {
    return 2;
  } else if (2 == panel && 1 == module) {
    return 1;
  } else if (2 == panel && (2 == module || 3 == module)) {
    return 2;
  } else {
    edm::LogPrint("SiPixelLorentzAngleDB") << " *** error *** in SiPixelLorentzAngleDB::HVgroup(...), panel = " << panel
                                           << ", module = " << module << endl;
    return 0;
  }
}
DEFINE_FWK_MODULE(SiPixelLorentzAngleDB);
