/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/


#include <af/graphics.h>
#include <af/algorithm.h>

#include <common/graphics_common.hpp>
#include <common/err_common.hpp>
#include <backend.hpp>

using af::dim4;
using namespace detail;

#if defined(WITH_GRAPHICS)
using namespace graphics;
#endif


af_err af_create_window(af_window *out, const int width, const int height, const char* const title)
{
#if defined(WITH_GRAPHICS)
    try {
        graphics::ForgeManager& fgMngr = graphics::ForgeManager::getInstance();
        fg_window mainWnd = NULL;

        try {
            mainWnd = fgMngr.getMainWindow();
        } catch(...) {
            std::cerr<<"OpenGL context creation failed"<<std::endl;
        }

        if (mainWnd == 0) {
            std::cerr<<"Not a valid window"<<std::endl;
            return AF_SUCCESS;
        }

        fg_window temp = nullptr;

        FG_CHECK(fg_create_window(&temp, width, height, title, mainWnd, false));

        fgMngr.setWindowChartGrid(temp, 1, 1);

        std::swap(*out, temp);
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(out);
    UNUSED(width);
    UNUSED(height);
    UNUSED(title);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_position(const af_window wind, const unsigned x, const unsigned y)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    FG_CHECK(fg_set_window_position(wind, x, y));
    return AF_SUCCESS;
#else
    UNUSED(wind);
    UNUSED(x);
    UNUSED(y);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_title(const af_window wind, const char* const title)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    FG_CHECK(fg_set_window_title(wind, title));
    return AF_SUCCESS;
#else
    UNUSED(wind);
    UNUSED(title);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_size(const af_window wind, const unsigned w, const unsigned h)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    FG_CHECK(fg_set_window_size(wind, w, h));
    return AF_SUCCESS;
#else
    UNUSED(wind);
    UNUSED(w);
    UNUSED(h);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_grid(const af_window wind, const int rows, const int cols)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager::getInstance().setWindowChartGrid(wind, rows, cols);
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(wind);
    UNUSED(rows);
    UNUSED(cols);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_axes_limits_compute(const af_window window,
                                  const af_array x, const af_array y, const af_array z,
                                  const bool exact, const af_cell* const props)
{
#if defined(WITH_GRAPHICS)
    if(window == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager& fgMngr = ForgeManager::getInstance();

        fg_chart chart = NULL;

        fg_chart_type ctype = (z ? FG_CHART_3D : FG_CHART_2D);

        if (props->col > -1 && props->row > -1)
            chart = fgMngr.getChart(window, props->row, props->col, ctype);
        else
            chart = fgMngr.getChart(window, 0, 0, ctype);

        double xmin = -1, xmax = 1;
        double ymin = -1, ymax = 1;
        double zmin = -1, zmax = 1;
        AF_CHECK(af_min_all(&xmin, NULL, x));
        AF_CHECK(af_max_all(&xmax, NULL, x));
        AF_CHECK(af_min_all(&ymin, NULL, y));
        AF_CHECK(af_max_all(&ymax, NULL, y));

        if(ctype == FG_CHART_3D) {
            AF_CHECK(af_min_all(&zmin, NULL, z));
            AF_CHECK(af_max_all(&zmax, NULL, z));
        }

        if(!exact) {
            xmin = step_round(xmin, false);
            xmax = step_round(xmax, true );
            ymin = step_round(ymin, false);
            ymax = step_round(ymax, true );
            zmin = step_round(zmin, false);
            zmax = step_round(zmax, true );
        }

        fgMngr.setChartAxesOverride(chart);
        FG_CHECK(fg_set_chart_axes_limits(chart, xmin, xmax,
                                          ymin, ymax, zmin, zmax));
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(window);
    UNUSED(x);
    UNUSED(y);
    UNUSED(z);
    UNUSED(exact);
    UNUSED(props);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_axes_limits_2d(const af_window window,
                             const float xmin, const float xmax,
                             const float ymin, const float ymax,
                             const bool exact, const af_cell* const props)
{
#if defined(WITH_GRAPHICS)
    if(window == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager& fgMngr = ForgeManager::getInstance();

        fg_chart chart = NULL;
        // The ctype here below doesn't really matter as it is only fetching
        // the chart. It will not set it.
        // If this is actually being done, then it is extremely bad.
        fg_chart_type ctype = FG_CHART_2D;

        if (props->col > -1 && props->row > -1)
            chart = fgMngr.getChart(window, props->row, props->col, ctype);
        else
            chart = fgMngr.getChart(window, 0, 0, ctype);

        float _xmin = xmin;
        float _xmax = xmax;
        float _ymin = ymin;
        float _ymax = ymax;
        if(!exact) {
            _xmin = step_round(_xmin, false);
            _xmax = step_round(_xmax, true );
            _ymin = step_round(_ymin, false);
            _ymax = step_round(_ymax, true );
        }

        fgMngr.setChartAxesOverride(chart);
        FG_CHECK(fg_set_chart_axes_limits(chart, _xmin, _xmax,
                                          _ymin, _ymax, 0.0f, 0.0f));
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(window);
    UNUSED(xmin);
    UNUSED(xmax);
    UNUSED(ymin);
    UNUSED(ymax);
    UNUSED(exact);
    UNUSED(props);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_axes_limits_3d(const af_window window,
                             const float xmin, const float xmax,
                             const float ymin, const float ymax,
                             const float zmin, const float zmax,
                             const bool exact, const af_cell* const props)
{
#if defined(WITH_GRAPHICS)
    if(window == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager& fgMngr = ForgeManager::getInstance();

        fg_chart chart = NULL;
        // The ctype here below doesn't really matter as it is only fetching
        // the chart. It will not set it.
        // If this is actually being done, then it is extremely bad.
        fg_chart_type ctype = FG_CHART_3D;

        if (props->col > -1 && props->row > -1)
            chart = fgMngr.getChart(window, props->row, props->col, ctype);
        else
            chart = fgMngr.getChart(window, 0, 0, ctype);

        float _xmin = xmin;
        float _xmax = xmax;
        float _ymin = ymin;
        float _ymax = ymax;
        float _zmin = zmin;
        float _zmax = zmax;
        if(!exact) {
            _xmin = step_round(_xmin, false);
            _xmax = step_round(_xmax, true );
            _ymin = step_round(_ymin, false);
            _ymax = step_round(_ymax, true );
            _zmin = step_round(_zmin, false);
            _zmax = step_round(_zmax, true );
        }

        fgMngr.setChartAxesOverride(chart);
        FG_CHECK(fg_set_chart_axes_limits(chart, _xmin, _xmax,
                                          _ymin, _ymax, _zmin, _zmax));
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(window);
    UNUSED(xmin);
    UNUSED(xmax);
    UNUSED(ymin);
    UNUSED(ymax);
    UNUSED(zmin);
    UNUSED(zmax);
    UNUSED(exact);
    UNUSED(props);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_axes_titles(const af_window window,
                          const char * const xtitle,
                          const char * const ytitle,
                          const char * const ztitle,
                          const af_cell* const props)
{
#if defined(WITH_GRAPHICS)
    if(window == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager& fgMngr = ForgeManager::getInstance();

        fg_chart chart = NULL;

        fg_chart_type ctype = (ztitle ? FG_CHART_3D : FG_CHART_2D);

        if (props->col > -1 && props->row > -1)
            chart = fgMngr.getChart(window, props->row, props->col, ctype);
        else
            chart = fgMngr.getChart(window, 0, 0, ctype);

        FG_CHECK(fg_set_chart_axes_titles(chart, xtitle, ytitle, ztitle));
    }
    CATCHALL;
    return AF_SUCCESS;
#else
    UNUSED(window);
    UNUSED(xtitle);
    UNUSED(ytitle);
    UNUSED(ztitle);
    UNUSED(props);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_show(const af_window wind)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    FG_CHECK(fg_swap_window_buffers(wind));
    return AF_SUCCESS;
#else
    UNUSED(wind);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_is_window_closed(bool *out, const af_window wind)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    FG_CHECK(fg_close_window(out, wind));
    return AF_SUCCESS;
#else
    UNUSED(out);
    UNUSED(wind);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_set_visibility(const af_window wind, const bool is_visible)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }
    if (is_visible) {
        FG_CHECK(fg_show_window(wind));
    } else {
        FG_CHECK(fg_hide_window(wind));
    }
    return AF_SUCCESS;
#else
    UNUSED(wind);
    UNUSED(is_visible);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

af_err af_destroy_window(const af_window wind)
{
#if defined(WITH_GRAPHICS)
    if(wind == 0) {
        std::cerr<<"Not a valid window"<<std::endl;
        return AF_SUCCESS;
    }

    try {
        ForgeManager::getInstance().setWindowChartGrid(wind, 0, 0);
    }
    CATCHALL;
    FG_CHECK(fg_release_window(wind));
    return AF_SUCCESS;
#else
    UNUSED(wind);
    AF_RETURN_ERROR("ArrayFire compiled without graphics support", AF_ERR_NO_GFX);
#endif
}

