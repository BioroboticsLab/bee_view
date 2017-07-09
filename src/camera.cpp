#include "stdafx.h"

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

	/* transform input vector by rotating arround x-axis of camera (right axis), normalizes vector 
	 * positive values rotate up */
	void Camera::rotateVecX(Vec3f &vec, float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), m_viewMatrix.linear().col(0));

		vec = t.linear() * vec;
		vec.normalize();
	}

	/* transform input vector by rotating arround y-axis of camera (up axis), normalizes vector
	 * positive values rotate to the right when looking forward */
	void Camera::rotateVecY(Vec3f &vec, float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), m_viewMatrix.linear().col(1));

		vec = t.linear()* vec;
		vec.normalize();
	}

	/* transform input vector by rotating arround z-axis of camera (foward axis), normalizes vector
	 * doesnt work on dir-vector of camera, to roll the camera use up-vector of camera instead of dir-vector,
	 * or use camera.rollRight(). */
	void Camera::rotateVecZ(Vec3f &vec, float angle)
	{
		
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), m_viewMatrix.linear().col(2));

		vec = t.linear() * vec;
		vec.normalize();

	}

	/* rotate camera arround x-axis of world (right axis) */
	void Camera::rotateUp(float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), Vec3f::UnitX());

		m_dir = t.linear() * m_dir;
		m_dir.normalize();
		recalcViewMatrix();
	}

	void Camera::rotateDown(float angle)
	{
		rotateUp(-angle);
	}

	void Camera::rotateRight(float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), Vec3f::UnitY());

		m_dir = t.linear() * m_dir;
		m_dir.normalize();
		recalcViewMatrix();
	}

	void Camera::rotateLeft(float angle)
	{
		rotateRight(-angle);
	}

	void Camera::rollRight(float angle)
	{
		Eigen::Transform<float, 3, Eigen::Affine> t;
		t = Eigen::AngleAxisf(deg2rad(angle), Vec3f::UnitZ());

		m_up = t.linear() * m_up;
		m_up.normalize();
		recalcViewMatrix();
	}

	void Camera::rollLeft(float angle)
	{
		rollRight(-angle);
	}


	void Camera::recalcViewMatrix()
	{	

		// use m_up vector for spanning the camera pane, (take some vektor not parallel to m_dir) 

		// vForward, vSide, and vUp are the 3 axis of the Camera coordinate system
		Vec3f vForward = m_dir;
		Vec3f vSide = vForward.cross(m_up).normalized();
		Vec3f vUp = vSide.cross(vForward).normalized();

		Eigen::Matrix3f linearPart = Eigen::Matrix3f::Zero();

		// Eigen default is column major order: use M2*M1*v for transforming vectors, where M1 is the first transformation and m2 is the second transformation.
		linearPart <<
			vSide(0),vUp(0), vForward(0),
			vSide(1), vUp(1), vForward(1),
			vSide(2), vUp(2), vForward(2);
		//-x to fix right hand vs lefthand? should reflect on y -> diagonal -1, 1 , 1, dont do it! breaks things

		m_viewMatrix.linear() = linearPart;

		m_viewMatrix.translation() = m_position;

		//DEBUG
		//std::cout << "Camera Matrix: " << std::endl << m_viewMatrix.matrix() << std::endl;

	}

	BeeEyeCamera::BeeEyeCamera(BeeEye::Ptr beeEye) : m_ommatidium_size(4)
	{
		m_type = Type::BEE_EYE;
		m_leftEye = beeEye;
		m_rightEye = std::make_shared<BeeEye>(beeEye->createOtherEye());

		if (m_leftEye->m_side == Side::RIGHT)
			m_leftEye.swap(m_rightEye);

		m_sampler = Sampler(11, 2.6);

	}

	PinholeCamera::PinholeCamera(int width, int height)
	{
		m_type = Type::PINHOLE;
		setWidth(width);
		setHeight(height);
		setFOV(50.f);
	}

	PinholeCamera::PinholeCamera(int width, int height, float fov) : m_fov(fov), m_width(width), m_height(height)
	{
		m_type = Type::PINHOLE;
		setWidth(width);
		setHeight(height);
		setFOV(fov);
	}

	int PinholeCamera::getWidth() { return m_width; }

	void PinholeCamera::setWidth(int width)
	{
		if (width == 0)
		{
			std::cerr << "camera width can't be 0!" << std::endl;
			return;
		}
		m_width = width;
		m_imageAspectRatio = m_width / (float)m_height;
	}

	int PinholeCamera::getHeight() { return m_height; }

	void PinholeCamera::setHeight(int height)
	{
		if (height == 0)
		{
			std::cerr << "camera height can't be 0!" << std::endl;
			return;
		}
		m_height = height;
		m_imageAspectRatio = m_width / (float)m_height;
	}

	float PinholeCamera::getFOV() { return m_fov; }

	void PinholeCamera::setFOV(float fov)
	{
		m_fov = fov;
		m_scale = tan(deg2rad(m_fov * 0.5));
	}

	float PinholeCamera::getScale() { return m_scale; } // only getter

	float PinholeCamera::getImageAspectRatio() { return m_imageAspectRatio; } // only getter
}