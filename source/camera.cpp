#include "../build/BeeView/BeeView/stdafx.h"

#include <iostream>
#include <eigen\Eigen\Dense>

#include "camera.h"

namespace BeeView {


	void Camera::moveTo(Vec3f newPosition)
	{
		m_position = newPosition;
	}

	void Camera::moveAndSetDirection(Vec3f point)
	{
		lookAt(point);
		m_position = point;
	}

	void Camera::lookAt(Vec3f point)
	{
		m_dir = (m_position - point).normalized();
		recalcViewMatrix();
	}

	void Camera::setDir(Vec3f dir)
	{
		m_dir = dir;
		recalcViewMatrix();
	}
	void Camera::recalcViewMatrix()
	{	

		Vec3f upVector = Vec3f(0, 1, 0); // use this vector for spanning the camera pane

		// vForward, vSide, and vUp are the 3 axis of the Camera coordinate system
		Vec3f vForward = m_dir;
		Vec3f vSide = upVector.cross(vForward).normalized(); // TODO: left or right handed?
		Vec3f vUp = vForward.cross(vSide).normalized();

		viewMatrix = Eigen::Matrix3f::Zero();

		// Eigen default is column major order: use M2*M1*v for transforming vectors, where M1 is the first transformation and m2 is the second transformation.
		viewMatrix <<
			-vSide(0),-vUp(0), -vForward(0),
			vSide(1), vUp(1), vForward(1),
			vSide(2), vUp(2), vForward(2);
		//-vside to fix right hand vs lefthand

		//DEBUG
		std::cout << "Camera Matrx: " << std::endl << viewMatrix << std::endl;

	}
}