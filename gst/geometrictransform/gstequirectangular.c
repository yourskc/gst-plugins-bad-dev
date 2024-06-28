/*
 * GStreamer
 * Copyright (C) 2010 Filippo Argiolas <filippo.argiolas@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:element-equirectangular
 * @title: equirectangular
 * @see_also: geometrictransform
 *
 * Equirectangular is a geometric image transform element. It transform a MOIL
 * fisheye to an equirectangular image .
 *
 * ## Example launch line
 * |[
 * gst-launch-1.0 -v videotestsrc ! equirectangular ! videoconvert !
 * autovideosink
 * ]|
 *
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <math.h>

#include "gstequirectangular.h"

GST_DEBUG_CATEGORY_STATIC(gst_equirectangular_debug);
#define GST_CAT_DEFAULT gst_equirectangular_debug

#define gst_equirectangular_parent_class parent_class
G_DEFINE_TYPE(GstEquirectangular, gst_equirectangular,
              GST_TYPE_GEOMETRIC_TRANSFORM);

#ifndef GST_RENESAS
// 1.19.2
GST_ELEMENT_REGISTER_DEFINE_WITH_CODE(
    equirectangular, "equirectangular", GST_RANK_NONE, GST_TYPE_EQUIRECTANGULAR,
    GST_DEBUG_CATEGORY_INIT(gst_equirectangular_debug, "equirectangular", 0,
                            "equirectangular"));
#endif

#define BUF_SZ 1920*1080;
float *buf_x;
float *buf_y;
FILE *fptr_x;
FILE *fptr_y;
int mat_width = 0;
int mat_height = 0;

static gboolean equirectangular_map(GstGeometricTransform *gt, gint x, gint y,
                                    gdouble *in_x, gdouble *in_y) {
#ifndef GST_DISABLE_GST_DEBUG
    GstEquirectangular *equirectangular = GST_EQUIRECTANGULAR_CAST(gt);
#endif
    gdouble norm_x;
    gdouble norm_y;
    gdouble r;

    gdouble width = gt->width;
    gdouble height = gt->height;

    if ((mat_width > 0) && (width == mat_width) && (mat_height > 0) &&
        (height == mat_height)) {
        float *p_x = buf_x + ((y - 1) * (int)width + x);
        float *p_y = buf_y + ((y - 1) * (int)width + x);

        *in_x = (gdouble)*p_x;
        *in_y = (gdouble)*p_y;

        GST_DEBUG_OBJECT(equirectangular, "Inversely mapped %d %d into %lf %lf",
                         x, y, *in_x, *in_y);
    }
    return TRUE;
}

static void gst_equirectangular_class_init(GstEquirectangularClass *klass) {
   
    GstElementClass *gstelement_class;
    GstGeometricTransformClass *gstgt_class;

    gstelement_class = (GstElementClass *)klass;
    gstgt_class = (GstGeometricTransformClass *)klass;

    gst_element_class_set_static_metadata(
        gstelement_class, "equirectangular", "Transform/Effect/Video",
        "Simulate a equirectangular image",
        "Filippo Argiolas <filippo.argiolas@gmail.com>");

    gstgt_class->map_func = equirectangular_map;

    fptr_x = fopen("EquimatX", "rb");
    fptr_y = fopen("EquimatY", "rb");
   
    if ((fptr_x != NULL) && (fptr_y != NULL)) {
        int Buf_Size = BUF_SZ;
        buf_x = malloc(sizeof(float) * Buf_Size);
        buf_y = malloc(sizeof(float) * Buf_Size);
        int rows, cols, type, channels;
        fread(&mat_height, 1, sizeof(int), fptr_x);
        fread(&mat_width, 1, sizeof(int), fptr_x);
        fread(&type, 1, sizeof(int), fptr_x);
        fread(&channels, 1, sizeof(int), fptr_x);
        fread(buf_x, Buf_Size, sizeof(float), fptr_x);
        fread(&rows, 1, sizeof(int), fptr_y);
        fread(&cols, 1, sizeof(int), fptr_y);
        fread(&type, 1, sizeof(int), fptr_y);
        fread(&channels, 1, sizeof(int), fptr_y);
        fread(buf_y, Buf_Size, sizeof(float), fptr_y);
        
        if (( mat_height == rows ) && ( mat_width == cols )){
        fprintf(stdout, "( rows, cols ) = ( %d, %d ) \n", rows, cols);
        fprintf(stdout, "X, Y Mats Loaded! \n");
    } else {
        if (errno != 0)
            fprintf(stderr, "Could not open mat file, for this reason: %s\n!",
                    strerror(errno));
        else
            fprintf(stderr, "Unknown errors.\n");
    }    
}
}

static void gst_equirectangular_init(GstEquirectangular *filter) {
    GstGeometricTransform *gt = GST_GEOMETRIC_TRANSFORM(filter);

    gt->off_edge_pixels = GST_GT_OFF_EDGES_PIXELS_CLAMP;
}

gboolean gst_equirectangular_plugin_init(GstPlugin *plugin) {
    GST_DEBUG_CATEGORY_INIT(gst_equirectangular_debug, "equirectangular", 0,
                            "equirectangular");

    return gst_element_register(plugin, "equirectangular", GST_RANK_NONE,
                                GST_TYPE_EQUIRECTANGULAR);
}
