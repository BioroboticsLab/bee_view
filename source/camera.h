#pragma once

#include <memory>

#include <eigen\Eigen\Core>

#include "bee_eye.h"
#include "utility.h"
#include "sampler.h"

namespace BeeView {

	/* Camera class, holds the render parameters */
	class Camera
	{
	private:
		Vec3f m_dir;
		Vec3f m_position;
		Vec3f m_up;

	public:
		enum class Type
		{
			BEE_EYE,
			PINHOLE,
			PANORAMIC
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

		Camera() : m_position(0,0,0), m_dir(0,0,-1), m_up(Vec3f(0,1,0))
		{
			recalcViewMatrix();
		}

		void moveTo(Vec3f newPosition);

		/* moves the camera and sets the direcation vector to oldPosition-newPosition */
		void moveAndSetDirection(Vec3f newPosition);
		
		/* rotate camera arround x-axis of camera (right axis) */
		void rotateX(float angle);
		/* rotate camera arround y-axis of camera (up axis) */
		void rotateY(float angle);
		/* rotate camera arround z-axis of camera (forward axis) */
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
		Vec3f getDir()
		{
			return m_dir;
		}
	};

	class BeeEyeCamera : public Camera
	{
	private:
		// sample points per ommatidium. actuall number of points: m_sqrtNumSamplePoints*m_sqrtNumSamplePoints + m_sqrtNumSamplePoints
		// this is for evenly distributing the sampling points over a square or a disk.
		int m_sqrtNumSamplePoints;

		// acceptance angle for all ommatidia
		float m_acceptanceAngle;

	public:

		BeeEye::Ptr m_leftEye;
		BeeEye::Ptr m_rightEye;

		int m_ommatidium_size;

		// the sampler to draw directions from
		Sampler m_sampler;

		// precomputed viewing direction offset array (x,y offsets in degrees)
		// precompute gauss weight array

		BeeEyeCamera(BeeEye::Ptr beeEye) : m_ommatidium_size(4)
		{
			m_type = Type::BEE_EYE;
			m_leftEye = beeEye;
			m_rightEye = std::make_shared<BeeEye>(beeEye->createOtherEye());

			m_sampler = Sampler(11, 2.6);

			if (m_leftEye->m_side == Side::RIGHT)
				m_leftEye.swap(m_rightEye);
		}
		

	};

	class PinholeCamera : public Camera
	{
	private:
		float m_fov;
		int m_width = 1;
		int m_height = 1;

		// precompute these parameters for efficiency
		float m_imageAspectRatio = 1;
		float m_scale;
	public:

		PinholeCamera(int width, int height)
		{
			m_type = Type::PINHOLE;
			setWidth(width);
			setHeight(height);
			setFOV(50.f);
		}
		PinholeCamera(int width, int height, float fov) : m_fov(fov), m_width(width), m_height(height) 
		{
			m_type = Type::PINHOLE;
			setWidth(width);
			setHeight(height);
			setFOV(fov);
		}

		int getWidth() { return m_width;  }
		void setWidth(int width)
		{
			if (width == 0)
			{
				std::cerr << "camera width can't be 0!" << std::endl;
				return;
			}
			m_width = width;
			m_imageAspectRatio = m_width / (float)m_height;
		}
		int getHeight() { return m_height;  }
		void setHeight(int height)
		{
			if (height == 0)
			{
				std::cerr << "camera height can't be 0!" << std::endl;
				return;
			}
			m_height = height;
			m_imageAspectRatio = m_width / (float)m_height;
		}
		float getFOV() { return m_fov; }
		void setFOV(float fov)
		{
			m_fov = fov;
			m_scale = tan(deg2rad(m_fov * 0.5));
		}
		float getScale() { return m_scale; } // only getter
		float getImageAspectRatio() { return m_imageAspectRatio; } // only getter

	};

	class PanoramicCamera : public Camera
	{
	public:
		float m_xFov = 360;
		float m_yFov = 90;

		// width in pixels
		int m_width;

		PanoramicCamera(int width) : m_width(width) 
		{
			m_type = Type::PANORAMIC;
		}

		PanoramicCamera(int width, float xFov) : m_width(width), m_xFov(xFov)
		{
			m_type = Type::PANORAMIC;
		}

		PanoramicCamera(int width, float xFov, float yFov) : m_width(width), m_xFov(xFov), m_yFov(yFov)
		{
			m_type = Type::PANORAMIC;
		}
	};

}