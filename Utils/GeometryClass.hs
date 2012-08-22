{-#LANGUAGE TypeFamilies,FlexibleInstances, FlexibleContexts#-}
module Utils.GeometryClass where

import Utils.Rectangle
import Utils.Point

class Point2D a where
   type ELP a :: *
   pt :: a -> (ELP a, ELP a)
   toPt :: (ELP a,ELP a) -> a

instance Point2D (Int,Int) where
   type ELP (Int,Int) = Int
   pt = id
   toPt = id

instance Point2D (Float,Float) where
   type ELP (Float,Float) = Float
   pt = id
   toPt = id

instance Point2D (Double,Double) where
   type ELP (Double,Double) = Double
   pt = id
   toPt = id

-- | Extract integer coordinates of a point
ipt :: (Point2D a,RealFrac (ELP a)) => a -> (Int,Int)
ipt = (\(x,y) -> (round x, round y)) . pt

convertPt :: (Point2D a, Point2D b, ELP a ~ ELP b) => a -> b
convertPt = toPt . pt

class BoundingBox a where
   type ELBB a :: *
   bounds :: a -> Rectangle (ELBB a)

class FromBounds a where
   type ELFB a :: *
   fromBounds :: Rectangle (ELFB a) -> a

instance BoundingBox (Rectangle a) where
   type ELBB (Rectangle a) = a
   bounds = id

instance FromBounds (Rectangle a) where
   type ELFB (Rectangle a) = a
   fromBounds = id

convertBounds :: (BoundingBox a, FromBounds b, ELBB a ~ ELFB b) => a -> b
convertBounds = fromBounds . bounds

-- type IntBounded a = (BoundingBox a,Integral (ELBB a))

class Line2D a where
   type ELL a :: *
   offsetAngle :: a -> (ELL a, Double)

class LineSegment a where
   type ELS a :: *
   startEnd :: a -> ((ELS a, ELS a),(ELS a, ELS a))

-- | Typeclass for elements with a size, such as images and matrices.
class Sized a where
    type Size a :: *
    getSize :: a -> Size a

biggerThan :: (Sized a, Sized b, Size a~(Int,Int), Size b ~Size a) => a -> b -> Bool
biggerThan a b = w1>=w2 && h1>=h2
    where
     (w1,h1) = getSize a
     (w2,h2) = getSize b

