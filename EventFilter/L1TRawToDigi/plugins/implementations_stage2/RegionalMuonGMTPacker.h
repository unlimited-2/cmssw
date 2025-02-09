#ifndef L1T_PACKER_STAGE2_REGIONALMUONGMTPACKER_H
#define L1T_PACKER_STAGE2_REGIONALMUONGMTPACKER_H

#include <vector>
#include <map>
#include "EventFilter/L1TRawToDigi/interface/PackerTokens.h"
#include "EventFilter/L1TRawToDigi/interface/Block.h"
#include "DataFormats/L1TMuon/interface/RegionalMuonCand.h"
#include "FWCore/Framework/interface/Event.h"

namespace l1t {
  namespace stage2 {
    class RegionalMuonGMTPacker : public Packer {
    public:
      Blocks pack(const edm::Event&, const PackerTokens*) override;
      void setIsKbmtf() { isKbmtf_ = true; };
      void setUseEmtfDisplacementInfo() { useEmtfDisplacementInfo_ = true; };

    private:
      typedef std::map<unsigned int, std::vector<uint32_t>> PayloadMap;
      void packTF(const edm::Event&, const edm::EDGetTokenT<RegionalMuonCandBxCollection>&, Blocks&);

      bool isKbmtf_{false};
      bool useEmtfDisplacementInfo_{false};
    };
  }  // namespace stage2
}  // namespace l1t

#endif
