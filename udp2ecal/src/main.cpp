#include <ecal/ecal.h>
#include <adma_sender.h>
#include <udp_data_receiver.h>
#include <thread>

void printHelpAndExit(const std::string& program_name)
{
  std::cout << program_name << " <Port> [topic]" << std::endl;
  std::cout << "  Port:  The UDP port to listen to" << std::endl;
  std::cout << "  topic: The eCAL Topic to send the data to. Default to \"adma_raw\"" << std::endl;

  exit(1);
}

int main(int argc, char** argv)
{
  if ((argc < 2) || (argc > 3) || (std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help"))
  {
    if (argc >= 1)
      printHelpAndExit(argv[0]);
    else
      printHelpAndExit("adma2ecal");
  }

  std::string port_string(argv[1]);
  uint16_t port(0);
  try
  {
    unsigned long port_long = std::stoul(port_string);
    if (port_long <= std::numeric_limits<uint16_t>::max())
    {
      port = static_cast<uint16_t>(port_long);
    }
    else
    {
      std::cerr << "ERROR: Port too large" << std::endl;
      exit(1);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: Unable to parse port: " << e.what() << std::endl;
  }

  std::string ecal_topic_name;
  if (argc >= 3)
    ecal_topic_name = argv[2];
  else
    ecal_topic_name = "adma_raw";

  std::cout << "Listening to UDP Port " << port << " and publishing to eCAL \"" << ecal_topic_name << "\"" << std::endl;
  
 
  
  eCAL::Initialize();
  {
    AdmaSender sender(ecal_topic_name);

    bool stop_health_check{ false };
    std::thread health_check([&stop_health_check, &sender] {
      while (!stop_health_check)
      {
        if (sender.GetLastTimestamp() == 0)
        {
          eCAL::Process::SetState(eCAL_Process_eSeverity::proc_sev_warning, eCAL_Process_eSeverity_Level::proc_sev_level1, "No adma frame has been received so far. ");
        }
        else
        {
          if (eCAL::Time::GetMicroSeconds() - sender.GetLastTimestamp() > 500000)
          {
            eCAL::Process::SetState(eCAL_Process_eSeverity::proc_sev_critical, eCAL_Process_eSeverity_Level::proc_sev_level1, "Timeout occured.");
          }
          else
          {
            eCAL::Process::SetState(eCAL_Process_eSeverity::proc_sev_healthy, eCAL_Process_eSeverity_Level::proc_sev_level1, "Everything seems to work fine.");
          }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      });

    
    UdpDataReceiver::PackageCallback callback = std::bind(&AdmaSender::OnAdmaData, &sender, std::placeholders::_1);

    auto receiver = UdpDataReceiver::Create(port, callback);
    receiver->start(4);

    while (eCAL::Ok())
      std::this_thread::sleep_for(std::chrono::milliseconds(100));

    receiver->stop();
    stop_health_check = true;
    health_check.join();
  }
  eCAL::Finalize();
  return 0;
}