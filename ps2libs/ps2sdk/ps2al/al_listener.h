
#include "al/al.h"
#include "al/al_internal.h"

class CListener
{
	public:
		static CListener& GetListener()
		{
			static CListener s_listener;
			return s_listener;
		}

		void SetValue( ALenum param, ALfloat value1, ALfloat value2, ALfloat value3 );
		void GetValue( ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3 );
		void GetValue( ALenum param, ALint *value1, ALint *value2, ALint *value3 );
	
	protected:
		void SetGain( ALfloat value);
		void GetGain( ALfloat* value);

		void SetVelocity ( ALfloat x, ALfloat y, ALfloat z); 
		void GetVelocity ( ALfloat *x, ALfloat *y, ALfloat *z); 

		void SetPosition ( ALfloat x, ALfloat y, ALfloat z); 
		void GetPosition ( ALfloat *x, ALfloat *y, ALfloat *z); 

		void SetOrientation ( ALfloat x, ALfloat y, ALfloat z); 
		void GetOrientation ( ALfloat *x, ALfloat *y, ALfloat *z); 

	protected:
		ALfloat m_gain;
		ALfloat m_velocity_x, m_velocity_y, m_velocity_z;
		ALfloat m_position_x, m_position_y, m_position_z;
		ALfloat m_orientation_x, m_orientation_y, m_orientation_z;

};




