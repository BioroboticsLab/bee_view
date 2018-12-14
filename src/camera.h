#pragma once

#include "bee_eye.h"
#include "sampler.h"

namespace BeeView {

	/* Camera class, holds the render parameters */

	class Camera
	{

	private:
		Vec3f m_position;
		Vec3f m_dir;
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

		void setPosition(Vec3f newPosition);

		Vec3f getPosition();

		/* moves the camera and sets the direcation vector to oldPosition-newPosition */
		void moveAndSetDirection(Vec3f newPosition);

		void lookAt(Vec3f point);

		void setDir(Vec3f dir);
		Vec3f getDir() { return m_dir; }
		
		/* rotate camera arround x-axis of camera (right axis) */
		void rotateUp(float angle);
		void rotateDown(float angle);

		/* rotate camera arround y-axis of camera (up axis) */
		void rotateRight(float angle);
		void rotateLeft(float angle);

		/* rotate camera arround z-axis of camera (forward axis) */
		void rollRight(float angle);
		void rollLeft(float angle);

		// Not gimbal lock safe!
		/* transform input vector by rotating arround x-axis of camera (right axis) */
		void rotateVecX(Vec3f &vec, float angle);
		/* transform input vector by rotating arround y-axis of camera (up axis) */
		void rotateVecY(Vec3f &vec, float angle);
		/* transform input vector by rotating arround z-axis of camera (foward axis) */
		void rotateVecZ(Vec3f &vec, float angle);

		void recalcViewMatrix();


	};

	class BeeEyeCamera : public Camera
	{

	private:
		int m_ommatidium_size;

		/* the output image width and height */
		int m_image_width;
		int m_image_height;

		void calculateImageDimensions();

	public:

		BeeEye::Ptr m_leftEye;
		BeeEye::Ptr m_rightEye;

		// the sampler to draw directions from
		Sampler m_sampler;

		BeeEyeCamera(BeeEye::Ptr beeEye);


		int getImageHeight();
		int getImageWidth();
		void setOmmatidiumSize(int size);
		int getOmmatidiumSize();
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

		PinholeCamera(int width, int height);

		PinholeCamera(int width, int height, float fov);

		int getWidth();
		void setWidth(int width);

		int getHeight();
		void setHeight(int height);

		float getFOV();
		void setFOV(float fov);

		float getScale();
		float getImageAspectRatio();
	};

	class PanoramicCamera : public Camera
	{
	public:
		// width in pixels

		int m_width = 1000;

		float m_xFov = 360;
		float m_yFov = 180;


		PanoramicCamera()
		{
			m_type = Type::PANORAMIC;
		}

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

		int getHeight()
		{
			return (int)ceil(m_width * m_yFov / m_xFov);
		}
	};

}