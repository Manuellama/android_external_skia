/*
 * Copyright 2011 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef GrGLTexture_DEFINED
#define GrGLTexture_DEFINED

#include "GrGpu.h"
#include "GrGLRenderTarget.h"

/**
 * A ref counted tex id that deletes the texture in its destructor.
 */
class GrGLTexID : public GrRefCnt {
public:
    SK_DECLARE_INST_COUNT(GrGLTexID)

    GrGLTexID(const GrGLInterface* gl, GrGLuint texID, bool isWrapped)
        : fGL(gl)
        , fTexID(texID)
        , fIsWrapped(isWrapped) {
    }

    virtual ~GrGLTexID() {
        if (0 != fTexID && !fIsWrapped) {
            GR_GL_CALL(fGL, DeleteTextures(1, &fTexID));
        }
    }

    void abandon() { fTexID = 0; }
    GrGLuint id() const { return fTexID; }

private:
    const GrGLInterface* fGL;
    GrGLuint             fTexID;
    bool                 fIsWrapped;

    typedef GrRefCnt INHERITED;
};

////////////////////////////////////////////////////////////////////////////////


class GrGLTexture : public GrTexture {

public:
    struct TexParams {
        GrGLenum fFilter;
        GrGLenum fWrapS;
        GrGLenum fWrapT;
        GrGLenum fSwizzleRGBA[4];
        void invalidate() { memset(this, 0xff, sizeof(TexParams)); }
    };

    struct Desc : public GrTextureDesc {
        GrGLuint        fTextureID;
        bool            fIsWrapped;
        GrSurfaceOrigin fOrigin;
    };

    // creates a texture that is also an RT
    GrGLTexture(GrGpuGL* gpu,
                const Desc& textureDesc,
                const GrGLRenderTarget::Desc& rtDesc);

    // creates a non-RT texture
    GrGLTexture(GrGpuGL* gpu,
                const Desc& textureDesc);


    virtual ~GrGLTexture() { this->release(); }

    virtual GrBackendObject getTextureHandle() const SK_OVERRIDE;

    virtual void invalidateCachedState() SK_OVERRIDE { fTexParams.invalidate(); }

    // these functions
    const TexParams& getCachedTexParams(GrGpu::ResetTimestamp* timestamp) const {
        *timestamp = fTexParamsTimestamp;
        return fTexParams;
    }
    void setCachedTexParams(const TexParams& texParams,
                            GrGpu::ResetTimestamp timestamp) {
        fTexParams = texParams;
        fTexParamsTimestamp = timestamp;
    }
    GrGLuint textureID() const { return fTexIDObj->id(); }

protected:

    // overrides of GrTexture
    virtual void onAbandon() SK_OVERRIDE;
    virtual void onRelease() SK_OVERRIDE;

private:
    TexParams                       fTexParams;
    GrGpu::ResetTimestamp           fTexParamsTimestamp;
    GrGLTexID*                      fTexIDObj;

    void init(GrGpuGL* gpu,
              const Desc& textureDesc,
              const GrGLRenderTarget::Desc* rtDesc);

    typedef GrTexture INHERITED;
};

#endif
