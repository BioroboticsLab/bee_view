#pragma once

#include <memory>

#include <eigen\Eigen\Core>

#include "bee_eye.h"
#include "utility.h"

namespace BeeView {

	class Camera
	{
	private:
		Vec3f m_dir;
		Vec3f m_upVector;
	public:
		enum class Type
		{
			BEE_EYE,
			PINHOLE
		};

		Vec3f m_position;
		Type m_type;


		/* the camera rotation matrix, checkout http://ksimek.github.io/2012/08/22/extrinsic/ for infos */
		Eigen::Matrix3f viewMatrix;

		Camera() : m_position(0,0,0), m_dir(0,0,-1), m_upVector(Vec3f(0,0,1))
		{
			recalcViewMatrix();
		}

		void moveTo(Vec3f newPosition);

		/* moves the camera and sets the direcation vector to oldPosition-newPosition */
		void moveAndSetDirection(Vec3f newPosition);
		
		/* not implemented*/
		void rotate(float roll);

		void lookAt(Vec3f point);

		void recalcViewMatrix();

		void setDir(Vec3f dir);
	};

	class BeeEyeCamera : public Camera
	{

	public:

		BeeEye::Ptr m_leftEye;
		BeeEye::Ptr m_rightEye;

		int m_ommatidium_size;

		// precomputed viewing direction offset array (x,y offsets in degrees)
		// precompute gauss weight array

		BeeEyeCamera(BeeEye::Ptr beeEye) : m_ommatidium_size(4)
		{
			m_type = Type::BEE_EYE;
			m_leftEye = beeEye;
			m_rightEye = std::make_shared<BeeEye>(beeEye->createOtherEye());

			if (m_leftEye->m_side == Side::RIGHT)
				m_leftEye.swap(m_rightEye);
		}

	};

	class PinholeCamera : public Camera
	{
	public:
		float m_fov;
		int m_width;
		int m_height;

		PinholeCamera(int width, int height) : m_fov(50), m_width(width), m_height(height)
		{
			m_type = Type::PINHOLE;
		}
		PinholeCamera(int width, int height, float fov) : m_fov(fov), m_width(width), m_height(height) 
		{
			m_type = Type::PINHOLE;
		}

	};
}