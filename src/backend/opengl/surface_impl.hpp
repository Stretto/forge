/*******************************************************
* Copyright (c) 2015-2019, ArrayFire
* All rights reserved.
*
* This file is distributed under 3-clause BSD license.
* The complete license agreement can be obtained at:
* http://arrayfire.com/licenses/BSD-3-Clause
********************************************************/

#pragma once

#include <common.hpp>

#include <glm/glm.hpp>

#include <memory>
#include <map>

namespace opengl
{

class surface_impl : public AbstractRenderable {
    protected:
        /* plot points characteristics */
        GLuint    mNumXPoints;
        GLuint    mNumYPoints;
        GLenum    mDataType;
        bool      mIsPVCOn;
        bool      mIsPVAOn;
        fg::MarkerType mMarkerType;
        /* OpenGL Objects */
        GLuint    mIBO;
        size_t    mIBOSize;
        GLuint    mMarkerProgram;
        GLuint    mSurfProgram;
        /* shared variable index locations */
        GLuint    mMarkerMatIndex;
        GLuint    mMarkerPointIndex;
        GLuint    mMarkerColorIndex;
        GLuint    mMarkerAlphaIndex;
        GLuint    mMarkerPVCIndex;
        GLuint    mMarkerPVAIndex;
        GLuint    mMarkerTypeIndex;
        GLuint    mMarkerColIndex;

        GLuint    mSurfMatIndex;
        GLuint    mSurfRangeIndex;
        GLuint    mSurfPointIndex;
        GLuint    mSurfColorIndex;
        GLuint    mSurfAlphaIndex;
        GLuint    mSurfPVCIndex;
        GLuint    mSurfPVAIndex;

        std::map<int, GLuint> mVAOMap;

        /* bind and unbind helper functions
         * for rendering resources */
        void bindResources(const int pWindowId);
        void unbindResources() const;
        glm::mat4  computeTransformMat(const glm::mat4& pView);
        virtual void renderGraph(const int pWindowId, const glm::mat4& transform);

    public:
        surface_impl(const uint pNumXpoints, const uint pNumYpoints,
                     const fg::dtype pDataType, const fg::MarkerType pMarkerType);
        ~surface_impl();

        void render(const int pWindowId,
                    const int pX, const int pY, const int pVPW, const int pVPH,
                    const glm::mat4 &pView);

        inline void usePerVertexColors(const bool pFlag=true) {
            mIsPVCOn = pFlag;
        }

        inline void usePerVertexAlphas(const bool pFlag=true) {
            mIsPVAOn = pFlag;
        }
};

class scatter3_impl : public surface_impl {
   private:
        void renderGraph(const int pWindowId, const glm::mat4& transform);

   public:
       scatter3_impl(const uint pNumXPoints, const uint pNumYPoints,
                     const fg::dtype pDataType, const fg::MarkerType pMarkerType=FG_MARKER_NONE)
           : surface_impl(pNumXPoints, pNumYPoints, pDataType, pMarkerType) {}
};

}
