#include <adma_sender.h>

AdmaSender::AdmaSender(const std::string& raw_data_topic_name)
  : raw_adma_publisher(raw_data_topic_name), last_timestamp(0)
{}

void AdmaSender::OnAdmaData(const std::vector<char>& data)
{
  {
    std::lock_guard<std::mutex> lock(_mutex);
    last_timestamp = static_cast<std::uint64_t>(eCAL::Time::GetMicroSeconds());
  }
  // TODO: use adma timestamp instead
  uint64_t u_TimeStamp = eCAL::Time::GetMicroSeconds();
  raw_adma_object.mutable_header()->set_timestamp(u_TimeStamp);

  std::string* protobuf_cube = raw_adma_object.mutable_payload();
  protobuf_cube->resize(data.size());
  std::copy(data.begin(), data.end(), protobuf_cube->begin());
  
  
  raw_adma_publisher.Send(raw_adma_object);
}

std::uint64_t AdmaSender::GetLastTimestamp() const
{
  std::lock_guard<std::mutex> lock(_mutex);
  return last_timestamp;
}
