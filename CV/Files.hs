module CV.Files where

import CV.Bindings.Files
import CV.Image

import Foreign.C.String
import Foreign.Ptr(nullPtr, castPtr)
import System.IO.Unsafe

readFromTcr :: String -> Image GrayScale D32
readFromTcr p = unsafePerformIO $ creatingImage $
  withCString p $ \path ->
    c'read_from_tcr path nullPtr

readFromTcrRectified :: String -> Image GrayScale D32
readFromTcrRectified p = unsafePerformIO $ creatingImage $
  withCString p $ \path ->
    c'read_from_tcr_rectified path

readFromTcrMag :: String -> Image GrayScale D32
readFromTcrMag p = unsafePerformIO $ creatingImage $
  withCString p $ \path ->
    c'read_from_tcr_mag path

calcDerivativeDirection :: Image GrayScale D32 -> Image GrayScale D32
calcDerivativeDirection image = unsafePerformIO $ creatingImage $
  withImage image $ \pimage ->
    c'calc_derivative_direction (castPtr pimage)
