#pragma once

#include "Adma.pb.h"
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <cstdint>
#include <mutex>

class AdmaSender
{
public:
  AdmaSender(const std::string& raw_data_topic_name);

  void OnAdmaData(const std::vector<char>& data);

  std::uint64_t GetLastTimestamp() const;
private: 
  eCAL::protobuf::CPublisher<pb::ADMA::ADMA_raw> raw_adma_publisher;
  pb::ADMA::ADMA_raw raw_adma_object;
  std::uint64_t last_timestamp;
  mutable std::mutex _mutex;
};
