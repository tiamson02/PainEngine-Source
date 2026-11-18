#include "al/al.h"
#include "al_listener.h"




void CListener::SetValue( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 )
{
	switch (param)
	{
		case AL_POSITION:
			SetPosition(value1, value2, value3);
			break;

		case AL_GAIN:
			SetGain(value1);
			break;

		case AL_VELOCITY:
			SetVelocity(value1, value2, value3);
			break;

		case AL_ORIENTATION:
			SetOrientation(value1, value2, value3);
			break;

		default:
			break;
	}
}


void CListener::GetValue( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 )
{
	switch (param)
	{
		case AL_POSITION:
			GetPosition(value1, value2, value3);
			break;

		case AL_GAIN:
			GetGain(value1);
			break;

		case AL_VELOCITY:
			GetVelocity(value1, value2, value3);
			break;

		case AL_ORIENTATION:
			GetOrientation(value1, value2, value3);
			break;

		default:
			break;
	}
}
void CListener::GetValue( ALenum param, ALint *value1, ALint *value2, ALint *value3 )
{
	ALfloat value1temp, value2temp, value3temp;
	GetValue(param, &value1temp, &value2temp, &value3temp);
	*value1 = (ALint)value1temp;
	*value2 = (ALint)value2temp;
	*value3 = (ALint)value3temp;
	
	

}

void CListener::SetGain( ALfloat value)
{
	m_gain = value;
}

void CListener::GetGain( ALfloat* value)
{
	*value = m_gain;
}


void CListener::SetVelocity ( ALfloat x, ALfloat y, ALfloat z)
{
	m_velocity_x = x;
	m_velocity_y = y;
	m_velocity_z = z;
}
 
void CListener::GetVelocity ( ALfloat *x, ALfloat *y, ALfloat *z)
{
	*x = m_velocity_x;
	*y = m_velocity_y;
	*z = m_velocity_z;
}
 

void CListener::SetPosition ( ALfloat x, ALfloat y, ALfloat z)
{
	m_position_x = x;
	m_position_y = y;
	m_position_z = z;
}
 
void CListener::GetPosition ( ALfloat *x, ALfloat *y, ALfloat *z)
{
	*x = m_position_x;
	*y = m_position_y;
	*z = m_position_z;
}
 

void CListener::SetOrientation ( ALfloat x, ALfloat y, ALfloat z)
{
	m_orientation_x = x;
	m_orientation_y = y;
	m_orientation_z = z;
}
 
void CListener::GetOrientation ( ALfloat *x, ALfloat *y, ALfloat *z)
{
	*x = m_orientation_x;
	*y = m_orientation_y;
	*y = m_orientation_z;
}
 
