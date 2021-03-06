/*******************************************************
 * Copyright (c) 2015-2019, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

// Parts of this code sourced from SnopyDogy
// https://gist.github.com/SnopyDogy/a9a22497a893ec86aa3e

#include <common.hpp>
#include <err_opengl.hpp>
#include <window_impl.hpp>

#include <algorithm>
#include <memory>
#include <mutex>

#include <FreeImage.h>

using namespace fg;

static GLEWContext* current = nullptr;

GLEWContext* glewGetContext()
{
    return current;
}

/* following function is thread safe */
int getNextUniqueId()
{
    static int wndUnqIdTracker = 0;
    static std::mutex wndUnqIdMutex;

    std::lock_guard<std::mutex> lock(wndUnqIdMutex);
    return wndUnqIdTracker++;
}

class FI_Manager
{
    public:
    static bool initialized;
    FI_Manager()
    {
#ifdef FREEIMAGE_LIB
        FreeImage_Initialise();
#endif
        initialized = true;
    }

    ~FI_Manager()
    {
#ifdef FREEIMAGE_LIB
        FreeImage_DeInitialise();
#endif
    }
};

bool FI_Manager::initialized = false;

static void FI_Init()
{
    static FI_Manager manager = FI_Manager();
}

class FI_BitmapResource
{
public:
    explicit FI_BitmapResource(FIBITMAP * p) :
        pBitmap(p)
    {
    }

    ~FI_BitmapResource()
    {
        FreeImage_Unload(pBitmap);
    }
private:
    FIBITMAP * pBitmap;
};

namespace opengl
{

void MakeContextCurrent(const window_impl* pWindow)
{
    if (pWindow != NULL) {
        pWindow->get()->makeContextCurrent();
        current = pWindow->glewContext();
    }
}

window_impl::window_impl(int pWidth, int pHeight, const char* pTitle,
                        std::weak_ptr<window_impl> pWindow, const bool invisible)
    : mID(getNextUniqueId())
{
    if (auto observe = pWindow.lock()) {
        mWindow = new wtk::Widget(pWidth, pHeight, pTitle, observe->get(), invisible);
		/* create glew context so that it will bind itself to windows */
		mGLEWContext = observe->glewContext();
    } else {
        /* when windows are not sharing any context, just create
         * a dummy wtk::Widget object and pass it on */
        mWindow = new wtk::Widget(pWidth, pHeight, pTitle, nullptr, invisible);
		/* create glew context so that it will bind itself to windows */
		mGLEWContext = new GLEWContext();
		if (mGLEWContext == NULL) {
			std::cerr << "Error: Could not create GLEW Context!\n";
			throw fg::Error("window_impl constructor", __LINE__,
				"GLEW context creation failed", FG_ERR_GL_ERROR);
		}
	}
	/* Set context (before glewInit()) */
	MakeContextCurrent(this);

    /* GLEW Initialization - Must be done */
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        char buffer[128];
        sprintf(buffer, "GLEW init failed: Error: %s\n", glewGetErrorString(err));
        throw fg::Error("window_impl constructor", __LINE__, buffer, FG_ERR_GL_ERROR);
    }
    err = glGetError();
    if (err!=GL_NO_ERROR && err!=GL_INVALID_ENUM) {
        /* ignore this error, as GLEW is known to
         * have this issue with 3.2+ core profiles
         * they are yet to fix this in GLEW
         */
        ForceCheckGL("GLEW initilization failed");
        throw fg::Error("window_impl constructor", __LINE__,
                "GLEW initilization failed", FG_ERR_GL_ERROR);
    }

    mCxt = mWindow->getGLContextHandle();
    mDsp = mWindow->getDisplayHandle();
    /* copy colormap shared pointer if
     * this window shares context with another window
     * */
    if (auto observe = pWindow.lock()) {
        mCMap = observe->colorMapPtr();
    } else {
        mCMap = std::make_shared<colormap_impl>();
    }


    mWindow->resizePixelBuffers();

    /* set the colormap to default */
    mColorMapUBO = mCMap->defaultMap();
    mUBOSize = mCMap->defaultLen();
    glEnable(GL_MULTISAMPLE);

    std::vector<glm::mat4>& mats = mWindow->mViewMatrices;
    mats.resize(mWindow->mRows*mWindow->mCols);
    std::fill(mats.begin(), mats.end(), glm::mat4(1));

    /* setup default window font */
    mFont = std::make_shared<font_impl>();
#ifdef OS_WIN
    mFont->loadSystemFont("Calibri");
#else
    mFont->loadSystemFont("Vera");
#endif
    glEnable(GL_DEPTH_TEST);

    CheckGL("End Window::Window");
}

window_impl::~window_impl()
{
    delete mWindow;
}

void window_impl::setFont(const std::shared_ptr<font_impl>& pFont)
{
    mFont = pFont;
}

void window_impl::setTitle(const char* pTitle)
{
    mWindow->setTitle(pTitle);
}

void window_impl::setPos(int pX, int pY)
{
    mWindow->setPos(pX, pY);
}

void window_impl::setSize(unsigned pW, unsigned pH)
{
    mWindow->setSize(pW, pH);
}

void window_impl::setColorMap(fg::ColorMap cmap)
{
    switch(cmap) {
        case FG_COLOR_MAP_DEFAULT:
            mColorMapUBO = mCMap->defaultMap();
            mUBOSize     = mCMap->defaultLen();
            break;
        case FG_COLOR_MAP_SPECTRUM:
            mColorMapUBO = mCMap->spectrum();
            mUBOSize     = mCMap->spectrumLen();
            break;
        case FG_COLOR_MAP_COLORS:
            mColorMapUBO = mCMap->colors();
            mUBOSize     = mCMap->colorsLen();
            break;
        case FG_COLOR_MAP_RED:
            mColorMapUBO = mCMap->red();
            mUBOSize     = mCMap->redLen();
            break;
        case FG_COLOR_MAP_MOOD:
            mColorMapUBO = mCMap->mood();
            mUBOSize     = mCMap->moodLen();
            break;
        case FG_COLOR_MAP_HEAT:
            mColorMapUBO = mCMap->heat();
            mUBOSize     = mCMap->heatLen();
            break;
        case FG_COLOR_MAP_BLUE:
            mColorMapUBO = mCMap->blue();
            mUBOSize     = mCMap->blueLen();
            break;
    }
}

int window_impl::getID() const
{
    return mID;
}

long long  window_impl::context() const
{
    return mCxt;
}

long long  window_impl::display() const
{
    return mDsp;
}

int window_impl::width() const
{
    return mWindow->mWidth;
}

int window_impl::height() const
{
    return mWindow->mHeight;
}

GLEWContext* window_impl::glewContext() const
{
    return mGLEWContext;
}

const wtk::Widget* window_impl::get() const
{
    return mWindow;
}

const std::shared_ptr<colormap_impl>& window_impl::colorMapPtr() const
{
    return mCMap;
}

void window_impl::hide()
{
    mWindow->hide();
}

void window_impl::show()
{
    mWindow->show();
}

bool window_impl::close()
{
    return mWindow->close();
}

void window_impl::draw(const std::shared_ptr<AbstractRenderable>& pRenderable)
{
    CheckGL("Begin window_impl::draw");
    MakeContextCurrent(this);
    mWindow->resetCloseFlag();
    glViewport(0, 0, mWindow->mWidth, mWindow->mHeight);

    const glm::mat4& viewMatrix = mWindow->mViewMatrices[0];
    // clear color and depth buffers
    glClearColor(WHITE[0], WHITE[1], WHITE[2], WHITE[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set colormap call is equivalent to noop for non-image renderables
    pRenderable->setColorMapUBOParams(mColorMapUBO, mUBOSize);
    pRenderable->render(mID, 0, 0, mWindow->mWidth, mWindow->mHeight, viewMatrix);

    mWindow->swapBuffers();
    mWindow->pollEvents();
    CheckGL("End window_impl::draw");
}

void window_impl::grid(int pRows, int pCols)
{
    glViewport(0, 0, mWindow->mWidth, mWindow->mHeight);
    glClearColor(WHITE[0], WHITE[1], WHITE[2], WHITE[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mWindow->mRows       = pRows;
    mWindow->mCols       = pCols;
    mWindow->mCellWidth  = mWindow->mWidth  / mWindow->mCols;
    mWindow->mCellHeight = mWindow->mHeight / mWindow->mRows;

    // resize viewMatrix array for views to appropriate size
    std::vector<glm::mat4>& mats = mWindow->mViewMatrices;
    mats.resize(mWindow->mRows*mWindow->mCols);
    std::fill(mats.begin(), mats.end(), glm::mat4(1));
}

void window_impl::draw(int pColId, int pRowId,
                       const std::shared_ptr<AbstractRenderable>& pRenderable,
                       const char* pTitle)
{
    CheckGL("Begin draw(column, row)");
    MakeContextCurrent(this);
    mWindow->resetCloseFlag();

    float pos[2] = {0.0, 0.0};
    int c     = pColId;
    int r     = pRowId;
    int x_off = c * mWindow->mCellWidth;
    int y_off = (mWindow->mRows - 1 - r) * mWindow->mCellHeight;

    const glm::mat4& viewMatrix = mWindow->mViewMatrices[r+c*mWindow->mRows];
    /* following margins are tested out for various
     * aspect ratios and are working fine. DO NOT CHANGE.
     * */
    int top_margin = int(0.06f*mWindow->mCellHeight);
    int bot_margin = int(0.02f*mWindow->mCellHeight);
    int lef_margin = int(0.02f*mWindow->mCellWidth);
    int rig_margin = int(0.02f*mWindow->mCellWidth);
    // set viewport to render sub image
    glViewport(x_off + lef_margin, y_off + bot_margin,
            mWindow->mCellWidth - 2 * rig_margin, mWindow->mCellHeight - 2 * top_margin);
    glScissor(x_off + lef_margin, y_off + bot_margin,
            mWindow->mCellWidth - 2 * rig_margin, mWindow->mCellHeight - 2 * top_margin);
    glEnable(GL_SCISSOR_TEST);

    // set colormap call is equivalent to noop for non-image renderables
    pRenderable->setColorMapUBOParams(mColorMapUBO, mUBOSize);
    pRenderable->render(mID, x_off, y_off, mWindow->mCellWidth, mWindow->mCellHeight, viewMatrix);

    glDisable(GL_SCISSOR_TEST);
    glViewport(x_off, y_off, mWindow->mCellWidth, mWindow->mCellHeight);

    if (pTitle!=NULL) {
        mFont->setOthro2D(mWindow->mCellWidth, mWindow->mCellHeight);
        pos[0] = mWindow->mCellWidth / 3.0f;
        pos[1] = mWindow->mCellHeight*0.92f;
        mFont->render(mID, pos, AF_BLUE, pTitle, 16);
    }

    CheckGL("End draw(column, row)");
}

void window_impl::swapBuffers()
{
    mWindow->swapBuffers();
    mWindow->pollEvents();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_impl::saveFrameBuffer(const char* pFullPath)
{
    if (!pFullPath) {
        throw fg::ArgumentError("window_impl::saveFrameBuffer", __LINE__, 1,
                                "Empty path string");
    }

    FI_Init();

    auto FIErrorHandler = [](FREE_IMAGE_FORMAT pOutputFIFormat, const char* pMessage) {
        printf("FreeImage Error Handler: %s\n", pMessage);
    };

    FreeImage_SetOutputMessage(FIErrorHandler);

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(pFullPath);
    if (format == FIF_UNKNOWN) {
        format = FreeImage_GetFIFFromFilename(pFullPath);
    }
    if (format == FIF_UNKNOWN) {
        throw fg::Error("window_impl::saveFrameBuffer", __LINE__,
                        "Freeimage: unrecognized image format", FG_ERR_FREEIMAGE_UNKNOWN_FORMAT);
    }

    if (!(format==FIF_BMP || format==FIF_PNG)) {
        throw fg::ArgumentError("window_impl::saveFrameBuffer", __LINE__, 1,
                                "Supports only bmp and png as of now");
    }

    uint w = mWindow->mWidth;
    uint h = mWindow->mHeight;
    uint c = 4;
    uint d = c * 8;

    FIBITMAP* bmp = FreeImage_Allocate(w, h, d);
    if (!bmp) {
        throw fg::Error("window_impl::saveFrameBuffer", __LINE__,
                        "Freeimage: allocation failed", FG_ERR_FREEIMAGE_BAD_ALLOC);
    }

    FI_BitmapResource bmpUnloader(bmp);

    uint pitch = FreeImage_GetPitch(bmp);
    uchar* dst = FreeImage_GetBits(bmp);

    /* as glReadPixels was called using PBO earlier, hopefully
     * it was async call(which it should be unless vendor driver
     * is doing something fishy) and the transfer is over by now
     * */
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mWindow->mFramePBO);

    uchar* src = (uchar*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

    if (src) {
        // copy data from mapped memory location
        uint w = mWindow->mWidth;
        uint h = mWindow->mHeight;
        uint i = 0;

        for (uint y = 0; y < h; ++y) {
            for (uint x = 0; x < w; ++x) {
                *(dst + x * c + FI_RGBA_RED  ) = (uchar) src[4*i+0]; // r
                *(dst + x * c + FI_RGBA_GREEN) = (uchar) src[4*i+1]; // g
                *(dst + x * c + FI_RGBA_BLUE ) = (uchar) src[4*i+2]; // b
                *(dst + x * c + FI_RGBA_ALPHA) = (uchar) src[4*i+3]; // a
                ++i;
            }
            dst += pitch;
        }

        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

    int flags = 0;
    if (format == FIF_JPEG)
        flags = flags | JPEG_QUALITYSUPERB;

    if (!(FreeImage_Save(format,bmp, pFullPath, flags) == TRUE)) {
    }
}

}
