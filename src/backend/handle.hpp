/*******************************************************
* Copyright (c) 2015-2019, ArrayFire
* All rights reserved.
*
* This file is distributed under 3-clause BSD license.
* The complete license agreement can be obtained at:
* http://arrayfire.com/licenses/BSD-3-Clause
********************************************************/

#pragma once

#include <fg/exception.h>

#include <window.hpp>
#include <font.hpp>
#include <image.hpp>
#include <chart.hpp>
#include <chart_renderables.hpp>

fg_window getHandle(common::Window* pValue);

fg_font getHandle(common::Font* pValue);

fg_image getHandle(common::Image* pValue);

fg_chart getHandle(common::Chart* pValue);

fg_histogram getHandle(common::Histogram* pValue);

fg_plot getHandle(common::Plot* pValue);

fg_surface getHandle(common::Surface* pValue);

fg_vector_field getHandle(common::VectorField* pValue);

common::Window* getWindow(const fg_window& pValue);

common::Font* getFont(const fg_font& pValue);

common::Image* getImage(const fg_image& pValue);

common::Chart* getChart(const fg_chart& pValue);

common::Histogram* getHistogram(const fg_histogram& pValue);

common::Plot* getPlot(const fg_plot& pValue);

common::Surface* getSurface(const fg_surface& pValue);

common::VectorField* getVectorField(const fg_vector_field& pValue);
