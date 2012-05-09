#ifndef __CVITERATORS__
#define __CVITERATORS__

#include <opencv2/core/core_c.h>

IplImage *project_polar(IplImage *src);

IplImage *project_polar_partial(IplImage *src, float cx, float cy, float afrom, float ato, float rfrom, float rto);

IplImage *draw_projection_guide(IplImage *src);

#endif
