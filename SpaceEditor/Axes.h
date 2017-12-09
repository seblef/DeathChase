
#ifndef _AXES_H_
#define _AXES_H_

#include "../DeathEngine/Core/Matrix.h"
#include "../DeathEngine/Core/Handles.h"

class Axes
{
	public:

		Axes() {}
		~Axes() {}

		static void Draw(const Core::Matrix4& mTrans);

	protected:

		static Core::HShader m_hShader;

};

#endif