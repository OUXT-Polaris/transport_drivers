// Copyright 2021 Masaya Kataoka.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Developed by Masaya Kataoka, 2021

#ifndef TCP_DRIVER__TCP_SOCKET_HPP_
#define TCP_DRIVER__TCP_SOCKET_HPP_

#include <array>
#include <string>
#include <vector>

#include "io_context/io_context.hpp"
#include "msg_converters/converters.hpp"

using asio::ip::tcp;
using asio::ip::address;
using drivers::common::IoContext;

namespace drivers
{
namespace tcp_driver
{

using Functor = std::function<void (const std::vector<uint8_t> &)>;

class TcpSocket
{
public:
  TcpSocket(const IoContext & ctx, const std::string & ip, uint16_t port);
  ~TcpSocket();

  TcpSocket(const TcpSocket &) = delete;
  TcpSocket & operator=(const TcpSocket &) = delete;

  std::string ip() const;
  uint16_t port() const;

  void open();
  void close();
  bool isOpen() const;
  void bind();

  /*
   * Blocking Send Operation
   */
  bool send(std::vector<uint8_t> & buff);

  /*
   * Blocking Receive Operation
   */
  bool receive(std::vector<uint8_t> & buff);

  /*
   * NonBlocking Send Operation
   */
  void asyncSend(std::vector<uint8_t> & buff);

  /*
   * NonBlocking Receive Operation
   */
  void asyncReceive(Functor func);

private:
  void asyncSendHandler(
    const asio::error_code & error,
    std::size_t bytes_transferred);

  void asyncReceiveHandler(
    const asio::error_code & error,
    std::size_t bytes_transferred);

private:
  const IoContext & m_ctx;
  tcp::socket m_tcp_socket;
  tcp::endpoint m_endpoint;
  Functor m_func;
  static const size_t m_recv_buffer_size{2048};
  std::vector<uint8_t> m_recv_buffer;
};

}  // namespace tcp_driver
}  // namespace drivers

#endif  // TCP_DRIVER__TCP_SOCKET_HPP_
