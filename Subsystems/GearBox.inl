//=============================================================================
//File Name: GearBox.inl
//Description: Represents a gear box with up to 3 motors and an encoder
//Author: FRC Team 3512, Spartatroniks
//=============================================================================

#include "GearBox.hpp"

#include <cmath>
#include <Encoder.h>
#include <Solenoid.h>
#include <SpeedController.h>

#include "../PIDController.hpp"

template <class T>
GearBox<T>::GearBox( unsigned int shifterChan , unsigned int encA ,
        unsigned int encB , unsigned int motor1 , unsigned int motor2 ,
        unsigned int motor3 ) {
    if ( encA != 0 && encB != 0 ) {
        m_encoder = new Encoder( encA , encB );
        m_pid = new PIDController( 0 , 0 , 0 , 0 , m_encoder , this );

        m_havePID = true;
    }
    else {
        m_encoder = NULL;
        m_pid = NULL;

        m_havePID = false;
    }

    if ( shifterChan != 0 ) {
        m_shifter = new Solenoid( shifterChan );
    }
    else {
        m_shifter = NULL;
    }

    m_isReversed = false;

    // Create motor controllers of specified template type
    if ( motor1 != 0 ) {
        m_motors.push_back( new T( motor1 ) );
    }
    if ( motor2 != 0 ) {
        m_motors.push_back( new T( motor2 ) );
    }
    if ( motor3 != 0 ) {
        m_motors.push_back( new T( motor3 ) );
    }

    if ( m_havePID ) {
        // c = PI * 10.16cm [wheel diameter]
        // dPerP = c / pulses
        m_encoder->SetDistancePerPulse( 3.14159265 * 10.16 / 360.0 );

        m_encoder->SetPIDSourceParameter( Encoder::kDistance );

        m_encoder->Start();
        m_pid->Enable();
    }
}

template <class T>
GearBox<T>::~GearBox() {
    if ( m_havePID ) {
        delete m_pid;

        m_encoder->Stop();
        delete m_encoder;
    }

    if ( m_shifter != NULL ) {
        delete m_shifter;
    }

    // Free motors
    for ( unsigned int i = 0 ; i < m_motors.size() ; i++ ) {
        delete m_motors[i];
    }
    m_motors.clear();
}

template <class T>
void GearBox<T>::setSetpoint( float setpoint ) {
    if ( !m_pid->IsEnabled() && m_havePID ) {
        m_pid->Enable();

        m_pid->SetSetpoint( setpoint );
    }
}

template <class T>
float GearBox<T>::getSetpoint() const {
    if ( m_havePID ) {
        return m_pid->GetSetpoint();
    }
    else {
        return 0.f;
    }
}

template <class T>
void GearBox<T>::setManual( float value ) {
    if ( m_havePID ) {
        if ( m_pid->IsEnabled() ) {
            m_pid->Disable();
        }
    }

    PIDWrite( value );
}

template <class T>
void GearBox<T>::setPID( float p , float i , float d ) {
    if ( m_havePID ) {
        m_pid->SetPID( p , i , d );
    }
}

template <class T>
void GearBox<T>::setF( float f ) {
    if ( m_havePID ) {
        m_pid->SetPID( m_pid->GetP() , m_pid->GetI() , m_pid->GetD() , f );
    }
}

template <class T>
void GearBox<T>::resetEncoder() {
    if ( m_havePID ) {
        m_encoder->Reset();
    }
}

template <class T>
double GearBox<T>::getDistance() const {
    if ( m_havePID ) {
        return m_encoder->GetDistance();
    }
    else {
        return 0.f;
    }
}

template <class T>
double GearBox<T>::getRate() const {
    if ( m_havePID ) {
        return m_encoder->GetRate();
    }
    else {
        return 0.f;
    }
}

template <class T>
void GearBox<T>::setReversed( bool reverse ) {
    m_isReversed = reverse;
}

template <class T>
bool GearBox<T>::isReversed() const {
    return m_isReversed;
}

template <class T>
void GearBox<T>::setGear( bool gear ) {
    if ( m_shifter != NULL ) {
        if ( m_havePID ) {
            /* If no manual override and going fast enough, shift since shifter
             * won't get stuck in neutral. The PID controller being disabled counts
             * as a manual override.
             * TODO find ideal minimum speed for shifting
             */
            if ( (m_pid->IsEnabled() && m_encoder->GetRate() > 4) || !m_pid->IsEnabled() ) {
                m_shifter->Set( gear );
            }
        }
        else {
            // No encoder, so change gear anyway
            m_shifter->Set( gear );
        }
    }
}

template <class T>
bool GearBox<T>::getGear() const {
    if ( m_shifter != NULL ) {
        return m_shifter->Get();
    }
    else {
        return false;
    }
}

template <class T>
void GearBox<T>::PIDWrite( float output ) {
    for ( unsigned int i = 0 ; i < m_motors.size() ; i++ ) {
        if ( !m_isReversed ) {
            m_motors[i]->Set( output );
        }
        else {
            m_motors[i]->Set( -output );
        }
    }
}
