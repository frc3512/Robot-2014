// Copyright (c) 2014-2017 FRC Team 3512. All Rights Reserved.

#pragma once

#include "LogSinkBase.h"

/*!
  A sink for writing logged events to standard output.
 */
class LogConsoleSink : public LogSinkBase {
public:
    virtual ~LogConsoleSink() = default;

    /*!
      Write an event to standard output.
      \param event The event to log.
     */
    void logEvent(LogEvent event);
};