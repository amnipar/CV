{-# LANGUAGE ForeignFunctionInterface #-}
module CV.Bindings.Projection where

import Foreign.Ptr
import CV.Bindings.Types
import CV.Image(BareImage)

#strict_import

#include <bindings.dsl.h>
#include "cvProjection.h"

#ccall project_polar , Ptr BareImage -> IO (Ptr BareImage)

#ccall project_polar_partial , Ptr BareImage -> Float -> Float -> Float -> Float -> Float -> Float -> IO (Ptr BareImage)

#ccall draw_projection_guide , Ptr BareImage -> IO (Ptr BareImage)
