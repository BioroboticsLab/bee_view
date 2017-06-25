#include "../build/BeeView/BeeView/stdafx.h"

#include <iostream>
#include <eigen\Eigen\Dense>

#include "camera.h"

namespace BeeView {


	void Camera::moveTo(Vec3f newPosition)
	{
		m_position = newPosition;
		recalcViewMatrix();
	}

	void Camera::moveAndSetDirection(Vec3f point)
	{
		m_dir = (m_position - point).normalized();
		m_position = point;
		recalcViewMatrix();

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

	/* transform input vector by rotating arround x-axis of camera (right axis) */
	void Camera::rotateVecX(Vec3f &vec, float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), m_viewMatrix.linear().col(0));

		vec = t * vec;
	}
	/* transform input vector by rotating arround y-axis of camera (up axis) */
	void Camera::rotateVecY(Vec3f &vec, float angle)
	{

	}
	/* transform input vector by rotating arround z-axis of camera (foward axis) */
	void Camera::rotateVecZ(Vec3f &vec, float angle)
	{

	}

	void Camera::recalcViewMatrix()
	{	

		Vec3f temp = Vec3f(0, 1, 0); // use this vector for spanning the camera pane, (take some vektor not parellel to m_dir) 

		// vForward, vSide, and vUp are the 3 axis of the Camera coordinate system
		Vec3f vForward = m_dir;
		Vec3f vSide = temp.cross(vForward).normalized(); // TODO: left or right handed?
		Vec3f vUp = vForward.cross(vSide).normalized();

		Eigen::Matrix3f linearPart = Eigen::Matrix3f::Zero();

		// Eigen default is column major order: use M2*M1*v for transforming vectors, where M1 is the first transformation and m2 is the second transformation.
		linearPart <<
			vSide(0),vUp(0), vForward(0),
			vSide(1), vUp(1), vForward(1),
			vSide(2), vUp(2), vForward(2);
		//-x to fix right hand vs lefthand? TODO: check, should reflect on y -> diagonal -1, 1 , 1

		m_viewMatrix.linear() = linearPart;

		m_viewMatrix.translation() = m_position;

		//DEBUG
		std::cout << "Camera Matrx: " << std::endl << m_viewMatrix.matrix() << std::endl;

	}
}