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

#ifndef REFEREEUART_H
#define REFEREEUART_H

#include <vector>

#include "librm/core/typedefs.hpp"
#include "librm/hal/serial.hpp"
#include "librm/device/device.hpp"
#include "librm/device/referee/referee.hpp"

namespace rm::device {
class RefereeUART : public Device {
public:
  RefereeUART() = delete;
  explicit RefereeUART(hal::SerialInterface &serial, Referee<RefereeRevision::kNewV120> &referee);

  void Begin();
  void RxCallback(const std::vector<u8> &data, u16 rx_len);

private:
  hal::SerialInterface *serial_;
  rm::device::Referee<RefereeRevision::kNewV120> &referee_;
};
} // namespace rm::device

#endif