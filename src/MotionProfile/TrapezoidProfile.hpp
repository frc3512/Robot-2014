// Copyright (c) 2014-2017 FRC Team 3512. All Rights Reserved.

#pragma once

/* Constant acceleration until target (max) velocity is reached, sets
 * acceleration to zero for a calculated time, then decelerates at a constant
 * acceleration with a slope equal to the negative slope of the initial
 * acceleration.
 */

#include <Synchronized.h>

#include "ProfileBase.hpp"

typedef enum { distance, velocity } SetpointMode;

class TrapezoidProfile : public ProfileBase {
public:
    TrapezoidProfile(double maxV, double timeToMaxV);
    virtual ~TrapezoidProfile() = default;

    /* If distance:
     *     curSetpoint is current distance set to which to travel
     *     curSource is current position (not used)
     *
     * If velocity:
     *     curSetpoint is current velocity at which to travel
     *     curSource is current velocity (not used)
     *
     * curTime is current time
     */
    double updateSetpoint(double curSetpoint, double curSource, double curTime);

    /* goal is a distance to which to travel
     * curSource is the current position
     * t initializes m_lastTime
     */
    double setGoal(double goal, double curSource, double t);

    bool atGoal();

    void setMaxVelocity(double v);
    void setTimeToMaxV(double timeToMaxV);

    void resetTime();

    // Tells algorithm whether to use distance or velocity as setpoint
    void setMode(SetpointMode mode);
    SetpointMode getMode();

private:
    double m_lastTime;
    double m_acceleration;
    double m_velocity;
    double m_timeFromMaxVelocity;
    double m_timeToMaxVelocity;
    double m_sign;
    double m_timeTotal;

    SetpointMode m_mode;

    ReentrantSemaphore m_varSem;
};
