/*******************************************************
 * Copyright (c) 2015-2019, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <fg/histogram.h>

#include <handle.hpp>
#include <chart_renderables.hpp>

fg_err fg_create_histogram(fg_histogram *pHistogram,
        const uint pNBins, const fg_dtype pType)
{
    try {
        *pHistogram = getHandle(new common::Histogram(pNBins, (fg::dtype)pType));
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_destroy_histogram(fg_histogram pHistogram)
{
    try {
        delete getHistogram(pHistogram);
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_set_histogram_color(fg_histogram pHistogram,
        const float pRed, const float pGreen,
        const float pBlue, const float pAlpha)
{
    try {
        getHistogram(pHistogram)->setColor(pRed, pGreen, pBlue, pAlpha);
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_set_histogram_legend(fg_histogram pHistogram, const char* pLegend)
{
    try {
        getHistogram(pHistogram)->setLegend(pLegend);
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_vbo(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = getHistogram(pHistogram)->vbo();
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_cbo(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = getHistogram(pHistogram)->cbo();
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_abo(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = getHistogram(pHistogram)->abo();
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_vbo_size(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = (uint)getHistogram(pHistogram)->vboSize();
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_cbo_size(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = (uint)getHistogram(pHistogram)->cboSize();
    }
    CATCHALL

    return FG_ERR_NONE;
}

fg_err fg_get_histogram_abo_size(uint* pOut, const fg_histogram pHistogram)
{
    try {
        *pOut = (uint)getHistogram(pHistogram)->aboSize();
    }
    CATCHALL

    return FG_ERR_NONE;
}
