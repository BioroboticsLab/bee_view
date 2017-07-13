from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

cdef extern from "beeview_api.h" namespace "BeeView":

    cdef cppclass BeeViewApplication:
        BeeViewApplication(string, string, bool) except +

        #Methods
        vector[vector[vector[float]]] render()

        void setCameraPosition(float, float, float)
        void getCameraPosition(float &, float &, float &)
        void setCameraDirVector(float, float, float)
        void getCameraDirVector(float &, float &, float &)
        void setRenderModeBeeEye()
        void setRenderModePinhole()
        void setRenderModePanoramic()
        float heightAboveGround()
        
        void setPanoramicCameraXfov(float)
        void setPanoramicCameraYfov(float)
        void setPanoramicCameraWidth(int)

        void rotateCameraUp(float)
        void rotateCameraDown(float)
        void rotateCameraRight(float)
        void rotateCameraLeft(float)
        void rollCameraRight(float)
        void rollCameraLeft(float)

        void setPinholeCameraFov(float)
        void setPinholeCameraWidth(int)
        void setPinholeCameraHeight(int)

        void setBeeEyeCameraAcceptanceAngle(float)
        void setBeeEyeCameraNumSamplePoints(int)
        void setBeeEyeCameraOmmatidiumSize(int)
        