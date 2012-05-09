module CV.Projection where

import CV.Bindings.Projection
import CV.Image

import Foreign.Ptr
import System.IO.Unsafe

projectPolar :: Image GrayScale D32 -> Image GrayScale D32
projectPolar i = unsafePerformIO $ creatingImage $
  withImage i $ \i_ptr ->
    c'project_polar (castPtr i_ptr)

projectPolarPartial :: Float -> Float -> Float -> Float -> Float -> Float -> Image GrayScale D32 -> Image GrayScale D32
projectPolarPartial cx cy afrom ato rfrom rto i = unsafePerformIO $ creatingImage $
  withImage i $ \i_ptr ->
    c'project_polar_partial (castPtr i_ptr) (realToFrac cx) (realToFrac cy) (realToFrac afrom) (realToFrac ato) (realToFrac rfrom) (realToFrac rto)

drawProjectionGuide :: Image GrayScale D32 -> Image RGB D32
drawProjectionGuide i = unsafePerformIO $ creatingImage $
  withImage i $ \i_ptr ->
    c'draw_projection_guide (castPtr i_ptr)
