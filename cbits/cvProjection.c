#include "cvProjection.h"

#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

float calc_sum_9(float *pos, size_t width)
{
  float *pos1, *pos2, *pos3, *pos4, *pos5;
  pos1 = pos - 1 * width - 1;
  pos2 = pos - 1;
  pos3 = pos + width - 1;
  return
    *pos1 + *(pos1+1) + *(pos1+2) +
    *pos2 + *(pos2+1) + *(pos2+2) +
    *pos3 + *(pos3+1) + *(pos3+2);
}

typedef unsigned int uint_t;

IplImage *to_16bit(IplImage *src)
{
  IplImage *dst;
  float *src_data, *src_pos, value;
  unsigned short *dst_data, *dst_pos;
  uint_t x, y, width, height, src_stride, dst_stride;
  int temp;
  CvSize size;

  width = src->width;
  height = src->height;
  printf("hello\n");
  size.width = width;
  size.height = height;
  dst = cvCreateImage(size, IPL_DEPTH_16U, 1);

  src_data = (float*)src->imageData;
  src_stride = (uint_t)(src->widthStep / sizeof(float));
  dst_data = (unsigned short*)dst->imageData;
  dst_stride = (uint_t)(dst->widthStep / sizeof(unsigned short));

  for (y = 0; y < height; y++) {
    src_pos = src_data + y * src_stride;
    dst_pos = dst_data + y * dst_stride;
    for (x = 0; x < width; x++, src_pos++, dst_pos++) {
      value = *src_pos;
      temp = (int)(value);
      if (temp < 0) temp = 0;
      else if (temp > 65535) temp = 65535;
      *dst_pos = (unsigned short)temp;
    }
  }

  return dst;
}

IplImage *project_polar(IplImage *src)
{
  IplImage *dst;
  unsigned int w, h, x, y, px, py, minx, maxx, miny, maxy, sumx, sumy, count;
  unsigned int new_width, new_height, src_stride, dst_stride;
  float value, a, r, rmin, rmax, cx, cy, ratio;
  float *src_data, *dst_data, *src_pos, *dst_pos;

  CvSize size = cvGetSize(src);
  src_data = (float*)src->imageData;
  src_stride = (int)(src->widthStep / sizeof(float));
  w = size.width;
  h = size.height;

  minx = 2000000000;
  maxx = 0;
  miny = 2000000000;
  maxy = 0;
  sumx = 0;
  sumy = 0;
  count = 0;

  /* find the extents of object */
  for (y = 0; y < h; y++) {
    src_pos = src_data + y * src_stride;
    for (x = 0; x < w; x++, src_pos++) {
      value = *src_pos;
      if (value > 0.0001) {
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
        sumx += x;
        sumy += y;
        count += 1;
      }
    }
  }

  cx = (float)sumx / count;
  cy = (float)sumy / count;
  ratio = (float)(maxy - miny) / (float)(maxx - minx);
  printf("w=%d h=%d cx=%f cy=%f ratio=%f\n", w, h, cx, cy, ratio);

  rmin = 2000000000;
  rmax = 0;
  for (y = 0; y < h; y++) {
    src_pos = src_data + y * src_stride;
    for (x = 0; x < w; x++, src_pos++) {
      value = *src_pos;
      if (value > 0.0001) {
        px = floor(x - cx);
        py = floor((y / ratio) - (cy / ratio));
        r = sqrt(py*py + px*px);
        if (r < rmin) rmin = r;
        if (r > rmax) rmax = r;
      }
    }
  }

  new_height = ceil(rmax - rmin);
  new_width = ceil(2 * M_PI * rmax);
  printf("w=%d h=%d rmin=%f rmax=%f\n", new_width, new_height, rmin, rmax);

  size.width = new_width;
  size.height = new_height;
  dst = cvCreateImage(size, IPL_DEPTH_32F, 1);
  dst_data = (float*)dst->imageData;
  dst_stride = (int)(dst->widthStep / sizeof(float));

  for (y = 0; y < new_height; y++) {
    dst_pos = dst_data + y * dst_stride;
    r = (float)(rmax - y);
    for (x = 0; x < new_width; x++, dst_pos++) {
      a = ((float)x / (float)new_width) * 2 * M_PI;
      px = floor(cx + (r * cos(a)));
      py = floor(((cy / ratio) + (r * sin(a))) * ratio);
      if (px < 0 || py < 0 || px >= w || py >= h) {
        /*printf(".");*/
      }
      else {
        src_pos = src_data + py * src_stride + px;
        value = *src_pos;
        if (value < 0.0001) {
          if (px > 0 && py > 0 && px < w-1 && py < h-1) {
            value = calc_sum_9(src_pos, src_stride) / 9;
            if (value < 0.0001) {
              value = 0.5;
            }
          }
          else {
            value = 0.5;
          }
        }
        *dst_pos = value;
      }
    }
  }
  return dst;
}

IplImage *project_polar_partial(IplImage *src, float cx, float cy, float afrom, float ato, float rfrom, float rto)
{
  IplImage *dst, *tmp;
  unsigned int w, h, x, y, px, py;
  unsigned int new_width, new_height, src_stride, dst_stride;
  float value, adiff, a, r;
  float *src_data, *dst_data, *src_pos, *dst_pos;

  CvSize size = cvGetSize(src);
  src_data = (float*)src->imageData;
  src_stride = (int)(src->widthStep / sizeof(float));
  w = size.width;
  h = size.height;

  adiff = ato - afrom;
  new_height = ceil(rto - rfrom);
  new_width = ceil(adiff * rto);

  size.width = new_width;
  size.height = new_height;
  dst = cvCreateImage(size, IPL_DEPTH_32F, 1);
  dst_data = (float*)dst->imageData;
  dst_stride = (int)(dst->widthStep / sizeof(float));

  for (y = 0; y < new_height; y++) {
    dst_pos = dst_data + y * dst_stride;
    r = (float)(rto - y);
    for (x = 0; x < new_width; x++, dst_pos++) {
      a = afrom + ((float)x / (float)new_width) * adiff;
      px = floor(cx + (r * cos(a)));
      py = floor(cy+ (r * sin(a)));
      if (px < 0 || py < 0 || px >= w || py >= h) {
        /*printf(".");*/
      }
      else {
        src_pos = src_data + py * src_stride + px;
        *dst_pos = *src_pos;
      }
    }
  }

  tmp = to_16bit(dst);
  cvSaveImage("r.tiff", tmp, 0);
  cvReleaseImage(&tmp);
  return dst;
}

IplImage *draw_projection_guide(IplImage *src)
{
  IplImage *dst;
  unsigned int w, h, x, y, px, py, minx, maxx, miny, maxy, sumx, sumy, count;
  unsigned int new_width, new_height, src_stride;
  float value, a, r, rmin, rmax, cx, cy, ratio;
  float *src_data, *src_pos;

  CvSize size = cvGetSize(src);
  dst = cvCreateImage(size, src->depth, 3);
  cvCvtColor(src, dst, CV_GRAY2BGR);
  src_data = (float*)src->imageData;
  src_stride = (int)(src->widthStep / sizeof(float));
  w = size.width;
  h = size.height;

  minx = 2000000000;
  maxx = 0;
  miny = 2000000000;
  maxy = 0;
  sumx = 0;
  sumy = 0;
  count = 0;

  /* find the extents of object */
  for (y = 0; y < h; y++) {
    src_pos = src_data + y * src_stride;
    for (x = 0; x < w; x++, src_pos++) {
      value = *src_pos;
      if (value > 0.0001) {
        if (x < minx) minx = x;
        if (x > maxx) maxx = x;
        if (y < miny) miny = y;
        if (y > maxy) maxy = y;
        sumx += x;
        sumy += y;
        count += 1;
      }
    }
  }

  cx = (float)sumx / count;
  cy = (float)sumy / count;
  ratio = (float)(maxy - miny) / (float)(maxx - minx);
  printf("w=%d h=%d cx=%f cy=%f ratio=%f\n", w, h, cx, cy, ratio);

  rmin = 2000000000;
  rmax = 0;
  for (y = 0; y < h; y++) {
    src_pos = src_data + y * src_stride;
    for (x = 0; x < w; x++, src_pos++) {
      value = *src_pos;
      if (value > 0.0001) {
        px = floor(x - cx);
        py = floor((y / ratio) - (cy / ratio));
        r = sqrt(py*py + px*px);
        if (r < rmin) rmin = r;
        if (r > rmax) rmax = r;
      }
    }
  }
  printf("rmin=%f rmax=%f\n", rmin, rmax);
  cvLine(dst, cvPoint((int)cx-5, (int)cy), cvPoint((int)cx+5, (int)cy), CV_RGB(1,0,0), 2, 8, 0);
  cvLine(dst, cvPoint((int)cx, (int)cy-5), cvPoint((int)cx, (int)cy+5), CV_RGB(1,0,0), 2, 8, 0);
  cvCircle(dst, cvPoint((int)cx,(int)cy), (int)rmin, CV_RGB(1,0,0), 2, 8, 0);
  cvCircle(dst, cvPoint((int)cx,(int)cy), (int)rmax, CV_RGB(1,0,0), 2, 8, 0);
  px = floor(cx + (r * cos(M_PI/2)));
  py = floor(((cy / ratio) + (r * sin(a))) * ratio);
  /*cvSaveImage("tcr_guide.png", dst, 0);*/

  return dst;
}
