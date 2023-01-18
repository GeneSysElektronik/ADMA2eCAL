#pragma once

#include <memory>

#include <functional>
#include <vector>
#include <string>
#include <stdint.h>

#include <asio.hpp>

class UdpDataReceiver : public std::enable_shared_from_this<UdpDataReceiver>
{
public:
  using PackageCallback = std::function<void(const std::vector<char>& completed_cube)>;

////////////////////////////////////
// Constructor & Destructor
////////////////////////////////////
public:
  static std::shared_ptr<UdpDataReceiver> Create(const std::string& address, uint16_t port, const PackageCallback& callback);
  static std::shared_ptr<UdpDataReceiver> Create(uint16_t port, const PackageCallback& callback);

private:
  // Constructor
  UdpDataReceiver(const std::string& address, uint16_t port, const PackageCallback& callback);

  // Copy
  UdpDataReceiver(const UdpDataReceiver&)             = default;
  UdpDataReceiver& operator=(const UdpDataReceiver&)  = default;

  // Move
  UdpDataReceiver& operator=(UdpDataReceiver&&)       = default;
  UdpDataReceiver(UdpDataReceiver&&)                  = default;

public:
  // Destructor
  ~UdpDataReceiver();

////////////////////////////////////
// Public API
////////////////////////////////////
public:
  bool start(unsigned int thread_pool_size);
  void stop();

////////////////////////////////////
// Receive data
////////////////////////////////////
private:
  void waitForData();
  void receiveData();
  void callCallback(const std::shared_ptr<std::vector<char>>& buffer);

////////////////////////////////////
// Member Variables
////////////////////////////////////
private:
  asio::ip::udp::endpoint  endpoint_;

  std::vector<std::thread> thread_pool_;
  asio::io_service         io_service_;
  asio::ip::udp::socket    socket_;

  PackageCallback          callback_;
  asio::io_service::strand callback_strand_;
};