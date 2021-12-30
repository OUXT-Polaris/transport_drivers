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

#include "tcp_driver/tcp_socket.hpp"

#include <boost/system/error_code.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <utility>
#include <string>
#include <system_error>
#include <vector>

#include "asio.hpp"
#include "rclcpp/logging.hpp"

namespace drivers
{
namespace tcp_driver
{

TcpSocket::TcpSocket(
  const IoContext & ctx,
  const std::string & ip,
  uint16_t port)
: m_ctx(ctx),
  m_tcp_socket(ctx.ios()),
  m_endpoint(address::from_string(ip), port)
{
  m_tcp_socket.connect(m_endpoint);
  m_recv_buffer.resize(m_recv_buffer_size);
}

TcpSocket::~TcpSocket()
{
  close();
}

bool TcpSocket::send(std::vector<uint8_t> & buff)
{
  boost::system::error_code error;
  /*
  asio::write(socket, asio::buffer(buff), error);
  if (error) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("TcpSocket::send"), error.message());
    return false;
  }
  */
  return true;
}

bool TcpSocket::receive(std::vector<uint8_t> & buff)
{
  asio::io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port()));
  acceptor.accept(m_tcp_socket);
  asio::streambuf receive_buffer;
  boost::system::error_code error;
  /*
  asio::read(m_tcp_socket, receive_buffer, asio::transfer_all(), error);
  if (error) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("TcpSocket::receive"), error.message());
    return false;
  }
  buff.clear();
  // buff = asio::buffer_cast<std::vector<uint8_t> >(receive_buffer.data());
  */
  return true;
}

void TcpSocket::asyncSend(std::vector<uint8_t> & buff)
{
  asio::async_write(
    m_tcp_socket,
    asio::buffer(buff),
    boost::bind(
      &TcpSocket::asyncSendHandler, this,
      asio::placeholders::error,
      asio::placeholders::bytes_transferred));
}

void TcpSocket::asyncReceive(Functor func)
{
  m_func = std::move(func);
  /*
  m_tcp_socket.async_receive_from(
    asio::buffer(m_recv_buffer),
    m_endpoint,
    [this](std::error_code error, std::size_t bytes_transferred)
    {
      asyncReceiveHandler(error, bytes_transferred);
    });
  */
}

void TcpSocket::asyncSendHandler(
  const asio::error_code & error,
  std::size_t bytes_transferred)
{
  (void)bytes_transferred;
  if (error) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("TcpSocket::asyncSendHandler"), error.message());
  }
}

void TcpSocket::asyncReceiveHandler(
  const asio::error_code & error,
  std::size_t bytes_transferred)
{
  (void)bytes_transferred;
  if (error) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("TcpSocket::asyncReceiveHandler"), error.message());
    return;
  }

  if (bytes_transferred > 0 && m_func) {
    m_recv_buffer.resize(bytes_transferred);
    m_func(m_recv_buffer);
    m_recv_buffer.resize(m_recv_buffer_size);
    /*
    m_tcp_socket.async_receive_from(
      asio::buffer(m_recv_buffer),
      m_endpoint,
      [this](std::error_code error, std::size_t bytes_tf)
      {
        m_recv_buffer.resize(bytes_tf);
        asyncReceiveHandler(error, bytes_tf);
      });
    */
  }
}

std::string TcpSocket::ip() const
{
  return m_endpoint.address().to_string();
}

uint16_t TcpSocket::port() const
{
  return m_endpoint.port();
}

void TcpSocket::open()
{
  m_tcp_socket.open(tcp::v4());
  m_tcp_socket.set_option(tcp::socket::reuse_address(true));
}

void TcpSocket::close()
{
  asio::error_code error;
  m_tcp_socket.close(error);
  if (error) {
    RCLCPP_ERROR_STREAM(rclcpp::get_logger("TcpSocket::close"), error.message());
  }
}

bool TcpSocket::isOpen() const
{
  return m_tcp_socket.is_open();
}

void TcpSocket::bind()
{
  m_tcp_socket.bind(m_endpoint);
}

}  // namespace tcp_driver
}  // namespace drivers
