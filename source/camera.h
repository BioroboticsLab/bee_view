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
		Vec3f m_position;

	public:
		enum class Type
		{
			BEE_EYE,
			PINHOLE
		};

		Type m_type;


		/* the camera rotation matrix, checkout http://ksimek.github.io/2012/08/22/extrinsic/ for infos.
		 use affinespace for easy 3d transformations. 
		 (homogenous coordinates, linear + affine part.
		 linear for rotation, scaling, sheering. 
		 affine for translation. 
		 vektors only linear, points linear and affine.)
		*/
		Eigen::Affine3f m_viewMatrix;

		Camera() : m_position(0,0,0), m_dir(0,0,-1), m_upVector(Vec3f(0,0,1))
		{
			recalcViewMatrix();
		}

		void moveTo(Vec3f newPosition);

		/* moves the camera and sets the direcation vector to oldPosition-newPosition */
		void moveAndSetDirection(Vec3f newPosition);
		
		/* not implemented, rotate camera arround x-axis of camera (right axis) */
		void rotateX(float angle);
		/* not implemented, rotate camera arround y-axis of camera (up axis) */
		void rotateY(float angle);
		/* not implemented, rotate camera arround z-axis of camera (forward axis) */
		void rotateZ(float angle);

		/* transform input vector by rotating arround x-axis of camera (right axis) */
		void rotateVecX(Vec3f &vec, float angle);
		/* transform input vector by rotating arround y-axis of camera (up axis) */
		void rotateVecY(Vec3f &vec, float angle);
		/* transform input vector by rotating arround z-axis of camera (foward axis) */
		void rotateVecZ(Vec3f &vec, float angle);

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