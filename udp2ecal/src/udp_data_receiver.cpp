#include <udp_data_receiver.h>

#include <iostream>

std::shared_ptr<UdpDataReceiver> UdpDataReceiver::Create(const std::string& address, uint16_t port, const PackageCallback& callback)
  { return std::shared_ptr<UdpDataReceiver>(new UdpDataReceiver(address, port, callback)); }

std::shared_ptr<UdpDataReceiver> UdpDataReceiver::Create(uint16_t port, const PackageCallback& callback)
  { return Create("0.0.0.0", port, callback); }

UdpDataReceiver::UdpDataReceiver(const std::string& address, uint16_t port, const PackageCallback& callback)
  : socket_         (io_service_)
  , endpoint_       (asio::ip::make_address(address), port)   // This constructor will throw an exception on invalid data
  , callback_       (callback)
  , callback_strand_(io_service_)
{
  socket_.open(asio::ip::udp::v4());                              // Will throw on fail
  socket_.bind(endpoint_);                                        // Will throw on fail
  socket_.set_option(asio::ip::udp::socket::reuse_address(true)); // Will throw on fail
}

UdpDataReceiver::~UdpDataReceiver()
{
  for (int i = 0; i < thread_pool_.size(); i++)
  {
    thread_pool_[i].detach();
  }
  thread_pool_.clear();
}

bool UdpDataReceiver::start(unsigned int thread_pool_size)
{
  // Give asio something to work on
  waitForData();

  // Create worker threads
  thread_pool_.reserve(thread_pool_size);
  for (unsigned int i = 0; i < thread_pool_size; i++)
  {
    thread_pool_.emplace_back([me = shared_from_this()]
                              {
                                me->io_service_.run();
                              });
  }

  return true;
}

void UdpDataReceiver::stop()
{
  io_service_.stop();
}

void UdpDataReceiver::waitForData()
{
  socket_.async_wait(asio::ip::udp::socket::wait_read
                  , [me = shared_from_this()](asio::error_code ec)->void
                    {
                      if (ec)
                      {
                        std::cerr << "Error waiting for data: " << ec.message() << std::endl;
                        return;
                      }

                      me->receiveData();
                    });
}

void UdpDataReceiver::receiveData()
{
  asio::error_code ec;
  auto bytes_to_read = socket_.available(ec);
  if (ec)
  {
    std::cerr << "Error checking how many bytes are available: " << ec.message() << std::endl;
    return;
  }

  std::shared_ptr<std::vector<char>> buffer = std::make_shared<std::vector<char>>();
  buffer->resize(bytes_to_read);

  socket_.async_receive(asio::buffer(buffer->data(), buffer->size())
                      , [me = shared_from_this(), buffer](asio::error_code ec, auto bytes_transferred)
                        {
                          if (ec)
                          {
                            std::cerr << "Error receiving datagram: " << ec.message() << std::endl;
                            return;
                          }

                          buffer->resize(bytes_transferred);

                          me->callCallback(buffer);
                          me->waitForData();
                        });
}

void UdpDataReceiver::callCallback(const std::shared_ptr<std::vector<char>>& buffer)
{
  callback_strand_.post([me = shared_from_this(), buffer]()
                        {
                          me->callback_(*buffer);
                        });
}
