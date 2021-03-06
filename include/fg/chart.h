/*******************************************************
 * Copyright (c) 2015-2019, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#pragma once

#include <fg/defines.h>
#include <fg/image.h>
#include <fg/plot.h>
#include <fg/surface.h>
#include <fg/vector_field.h>
#include <fg/histogram.h>


#ifdef __cplusplus
extern "C" {
#endif

FGAPI fg_err fg_create_chart(fg_chart *pHandle,
                             const fg_chart_type pChartType);

FGAPI fg_err fg_destroy_chart(fg_chart pHandle);

FGAPI fg_err fg_set_chart_axes_titles(fg_chart pHandle,
                                      const char* pX,
                                      const char* pY,
                                      const char* pZ);

FGAPI fg_err fg_set_chart_axes_limits(fg_chart pHandle,
                                      const float pXmin, const float pXmax,
                                      const float pYmin, const float pYmax,
                                      const float pZmin, const float pZmax);

FGAPI fg_err fg_set_chart_legend_position(fg_chart pHandle, const float pX, const float pY);

FGAPI fg_err fg_add_image_to_chart(fg_image* pImage, fg_chart pHandle,
                                   const uint pWidth, const uint pHeight,
                                   const fg_channel_format pFormat,
                                   const fg_dtype pType);

FGAPI fg_err fg_add_histogram_to_chart(fg_histogram* pHistogram, fg_chart pHandle,
                                       const uint pNBins, const fg_dtype pType);

FGAPI fg_err fg_add_plot_to_chart(fg_plot* pPlot, fg_chart pHandle,
                                  const uint pNPoints, const fg_dtype pType,
                                  const fg_plot_type pPlotType, const fg_marker_type pMarkerType);

FGAPI fg_err fg_add_surface_to_chart(fg_surface* pSurface, fg_chart pHandle,
                                     const uint pXPoints, const uint pYPoints, const fg_dtype pType,
                                     const fg_plot_type pPlotType, const fg_marker_type pMarkerType);

FGAPI fg_err fg_add_vector_field_to_chart(fg_vector_field* pField, fg_chart pHandle,
                                          const uint pNPoints, const fg_dtype pType);

FGAPI fg_err fg_render_chart(const fg_window pWindow,
                             const fg_chart pChart,
                             const int pX, const int pY, const int pWidth, const int pHeight);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

namespace fg
{

/**
   \class Chart

   \brief Chart is base canvas where other plottable objects are rendered.

   Charts come in two types:
        - \ref FG_2D - Two dimensional charts
        - \ref FG_3D - Three dimensional charts
 */
class Chart {
    private:
        fg_chart mValue;

    public:
        /**
           Creates a Chart object with given dimensional property

           \param[in] cType is chart dimension property
         */
        FGAPI Chart(const ChartType cType);

        /**
           Chart copy constructor
         */
        FGAPI Chart(const Chart& pOther);

        /**
           Chart destructor
         */
        FGAPI ~Chart();

        /**
           Set axes titles for the chart

           \param[in] pX is x-axis title label
           \param[in] pY is y-axis title label
           \param[in] pZ is z-axis title label
         */
        FGAPI void setAxesTitles(const char* pX,
                                 const char* pY,
                                 const char* pZ=NULL);

        /**
           Set axes data ranges

           \param[in] pXmin is x-axis minimum data value
           \param[in] pXmax is x-axis maximum data value
           \param[in] pYmin is y-axis minimum data value
           \param[in] pYmax is y-axis maximum data value
           \param[in] pZmin is z-axis minimum data value
           \param[in] pZmax is z-axis maximum data value
         */
        FGAPI void setAxesLimits(const float pXmin, const float pXmax,
                                 const float pYmin, const float pYmax,
                                 const float pZmin=-1, const float pZmax=1);

        /**
           Set legend position for Chart

           \param[in] pX is horizontal position in normalized coordinates
           \param[in] pY is vertical position in normalized coordinates

           \note By normalized coordinates, the range of these coordinates is expected to be [0-1].
           (0,0) is the bottom hand left corner.
         */
        FGAPI void setLegendPosition(const float pX, const float pY);

        /**
           Add an existing Image object to the current chart

           \param[in] pImage is the Image to render on the chart
         */
        FGAPI void add(const Image& pImage);

        /**
           Add an existing Histogram object to the current chart

           \param[in] pHistogram is the Histogram to render on the chart
         */
        FGAPI void add(const Histogram& pHistogram);

        /**
           Add an existing Plot object to the current chart

           \param[in] pPlot is the Plot to render on the chart
         */
        FGAPI void add(const Plot& pPlot);

        /**
           Add an existing Surface object to the current chart

           \param[in] pSurface is the Surface to render on the chart
         */
        FGAPI void add(const Surface& pSurface);

        /**
           Create and add an Image object to the current chart

           \param[in] pWidth Width of the image
           \param[in] pHeight Height of the image
           \param[in] pFormat Color channel format of image, uses one of the values
                      of \ref ChannelFormat
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of histogram data
         */
        FGAPI Image image(const uint pWidth, const uint pHeight,
                          const ChannelFormat pFormat=FG_RGBA, const dtype pDataType=f32);

        /**
           Create and add an Histogram object to the current chart

           \param[in] pNBins is number of bins the data is sorted out
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of histogram data
         */
        FGAPI Histogram histogram(const uint pNBins, const dtype pDataType);

        /**
           Create and add an Plot object to the current chart

           \param[in] pNumPoints is number of data points to display
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of plot data
           \param[in] pPlotType dictates the type of plot/graph,
                      it can take one of the values of \ref PlotType
           \param[in] pMarkerType indicates which symbol is rendered as marker. It can take one of
                      the values of \ref MarkerType.
         */
        FGAPI Plot plot(const uint pNumPoints, const dtype pDataType,
                        const PlotType pPlotType=FG_PLOT_LINE, const MarkerType pMarkerType=FG_MARKER_NONE);

        /**
           Create and add an Plot object to the current chart

           \param[in] pNumXPoints is number of data points along X dimension
           \param[in] pNumYPoints is number of data points along Y dimension
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of plot data
           \param[in] pPlotType is the render type which can be one of \ref PlotType (valid choices
                      are FG_SURFACE and FG_SCATTER)
           \param[in] pMarkerType is the type of \ref MarkerType to draw for \ref FG_SCATTER plot type
         */
        FGAPI Surface surface(const uint pNumXPoints, const uint pNumYPoints, const dtype pDataType,
                              const PlotType pPlotType=FG_PLOT_SURFACE, const MarkerType pMarkerType=FG_MARKER_NONE);

        /**
           Create and add an Vector Field object to the current chart

           \param[in] pNumPoints is number of data points to display
           \param[in] pDataType takes one of the values of \ref dtype that indicates
                      the integral data type of vector field data
         */
        FGAPI VectorField vectorField(const uint pNumPoints, const dtype pDataType);

        /**
           Render the chart to given window

           \param[in] pWindow is target window to where chart will be rendered
           \param[in] pX is x coordinate of origin of viewport in window coordinates
           \param[in] pY is y coordinate of origin of viewport in window coordinates
           \param[in] pVPW is the width of the viewport
           \param[in] pVPH is the height of the viewport
         */
        FGAPI void render(const Window& pWindow,
                          const int pX, const int pY, const int pVPW, const int pVPH) const;

        /**
           Get the handle to internal implementation of Chart
         */
        FGAPI fg_chart get() const;
};

}

#endif
