#include <ecal/ecal.h>
#include <thread>

#include <iostream>

#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>

#include <fill_adma_proto_object.h>

void printHelpAndExit(const std::string& program_name)
{
  std::cout << program_name << " <raw_src_topic> <debug_interpreted_topic>" << std::endl;
  std::cout << "  raw_src_topic:           Name of the raw data topic to subscribe to" << std::endl;
  std::cout << "  debug_net_topic: The name of the target topic to publish the net debug data to" << std::endl;
  std::cout << "  debug_delta_topic: The name of the target topic to publish the delta debug data to" << std::endl;

  exit(1);
}

int main(int argc, char** argv)
{
  if ((argc != 4)
    || ((argc > 1) && ((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help"))))
  {
    if (argc >= 1)
      printHelpAndExit(argv[0]);
    else
      printHelpAndExit("debug_interpreter");
  }

  std::string src_topic_name(argv[1]);
  std::string dst_topic_name_net(argv[2]);
  std::string dst_topic_name_delta(argv[3]);

  std::cout << "Subscribing to \"" << src_topic_name << "\" and publishing to \"" << dst_topic_name_net << "\" and \"" << dst_topic_name_net << "\"" << std::endl;
  eCAL::Initialize();

  eCAL::protobuf::CSubscriber<pb::ADMA::ADMA_raw>      raw_adma_subscriber       (src_topic_name);
  eCAL::protobuf::CPublisher<pb::ADMA::ADMA_delta>     delta_adma_publisher(dst_topic_name_delta);
  eCAL::protobuf::CPublisher<pb::ADMA::ADMA_net>       net_adma_publisher(dst_topic_name_net);

  raw_adma_subscriber.AddReceiveCallback(
            [&delta_adma_publisher, &net_adma_publisher](const char* /*topic_name*/, const pb::ADMA::ADMA_raw& raw_msg, long long time, long long /*clock*/, long long /*id*/) -> void
            {
              std::vector<char> raw_data;
              raw_data.resize(raw_msg.payload().size());

              std::memcpy(&raw_data[0], raw_msg.payload().data(), raw_data.size());

              if (raw_data.size() == 856)
              {
                pb::ADMA::ADMA_net net_adma_msg;
                fill_adma_proto_object(raw_data, net_adma_msg);
                net_adma_publisher.Send(net_adma_msg);
              }
              else if (raw_data.size() == 88)
              {
                pb::ADMA::ADMA_delta delta_adma_msg;
                fill_adma_proto_object(raw_data, delta_adma_msg);
                delta_adma_publisher.Send(delta_adma_msg);
              }
              else
              {
                std::cerr << "ERROR: Payload length is " << raw_data.size() << ", expected 856." << std::endl;
              }

            });


  while (eCAL::Ok())
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

  eCAL::Finalize();
  return 0;
}