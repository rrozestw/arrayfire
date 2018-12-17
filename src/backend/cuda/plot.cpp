/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#if defined (WITH_GRAPHICS)

#include <Array.hpp>
#include <plot.hpp>
#include <err_cuda.hpp>
#include <debug_cuda.hpp>
#include <GraphicsResourceManager.hpp>

using af::dim4;

namespace cuda {

template<typename T>
void copy_plot(const Array<T> &P, fg_plot plot)
{
    ForgeModule& _ = graphics::forgePlugin();
    auto stream = cuda::getActiveStream();
    if(DeviceManager::checkGraphicsInteropCapability()) {
        const T *d_P = P.get();

        auto res = interopManager().getPlotResources(plot);

        size_t bytes = 0;
        T* d_vbo = NULL;
        cudaGraphicsMapResources(1, res[0].get(), stream);
        cudaGraphicsResourceGetMappedPointer((void **)&d_vbo,
                                             &bytes, *(res[0].get()));
        cudaMemcpyAsync(d_vbo, d_P, bytes, cudaMemcpyDeviceToDevice, stream);
        cudaGraphicsUnmapResources(1, res[0].get(), stream);

        CheckGL("After cuda resource copy");

        POST_LAUNCH_CHECK();
    } else {
        unsigned bytes = 0, buffer = 0;
        FG_CHECK(fg_get_plot_vertex_buffer(&buffer, plot));
        FG_CHECK(fg_get_plot_vertex_buffer_size(&bytes, plot));

        CheckGL("Begin CUDA fallback-resource copy");
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        if (ptr) {
            CUDA_CHECK(cudaMemcpyAsync(ptr, P.get(), bytes,
                                       cudaMemcpyDeviceToHost, stream));
            CUDA_CHECK(cudaStreamSynchronize(stream));
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CheckGL("End CUDA fallback-resource copy");
    }
}

#define INSTANTIATE(T)  \
template void copy_plot<T>(const Array<T> &, fg_plot);

INSTANTIATE(float)
INSTANTIATE(double)
INSTANTIATE(int)
INSTANTIATE(uint)
INSTANTIATE(short)
INSTANTIATE(ushort)
INSTANTIATE(uchar)

}

#endif  // WITH_GRAPHICS
