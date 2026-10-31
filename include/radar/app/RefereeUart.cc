/*
  Copyright (c) 2026 XDU-IRobot

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

/**
 * @file  RefereeUart.cc
 * @brief 裁判系统串口
 */

#include "RefereeUart.hpp"

#include "librm/device/remote/dr16.hpp"

namespace rm::device {

/**
 * @param serial 串口对象
 * @param referee
 */
RefereeUART::RefereeUART(hal::SerialInterface &serial, Referee<RefereeRevision::kNewV120> &referee) : serial_(&serial), referee_(referee) {
  static hal::SerialRxCallbackFunction rx_callback =
      std::bind(&RefereeUART::RxCallback, this, std::placeholders::_1, std::placeholders::_2);
  this->serial_->AttachRxCallback(rx_callback);
}

/**
 * @brief 开始接收遥控器数据
 */
void RefereeUART::Begin() { this->serial_->Begin(); }

/**
 * @brief 串口接收完成中断回调函数
 * @param data      接收到的数据
 * @param rx_len    接收到的数据长度
 */
void RefereeUART::RxCallback(const std::vector<u8> &data, u16 rx_len) {
  for (auto i = 0; i < rx_len; i++) {
    referee_ << data.at(i);
  }
}

}  // namespace rm::device
