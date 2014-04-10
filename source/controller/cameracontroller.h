#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "../defines.h"

class CameraController
{
	public:
		CameraController();
		~CameraController();

		void SetCamera(Camera *cam);
		void SetArea(const Rect4f &area);
		void LookAt(const vec3 &pos);

	protected:
		Camera		*pCamera;
		vec3		cOffset;
		Rect4f		cArea;
};

#endif // CAMERACONTROLLER_H
