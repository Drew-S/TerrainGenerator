#### Input

- [Texture](07_texture.md) &mdash; Create a texture node that can be drawn on, link to existing images in the computer.
- [Simplex Noise](08_noise.md) &mdash; Create a generated smooth noise texture.
- [Constant Value](09_constantvalue.md) &mdash; Input for a single constant decimal number.
- [Constant Vector](10_constantvector.md) &mdash; Input for a single constant vector comprised of four constant decimal numbers.

#### Converters

- [Curve and Slope](12_bezier.md) &mdash; Convert a height map using a function defined by a complex bézier curve.
- [Clamp](13_clamp.md) &mdash; Clamps values between two given values.
- [Colour Combine](14_colorcombine.md) &mdash; Combines four separate mono channels into a single rgba output.
- [Colour Split](15_colorsplit.md) &mdash; Splits a single rgba input into four separate mono channels.
- [Erosion Simulation](16_erosion.md) &mdash; Applies rainfall erosion simulation.
- [Invert](17_invert.md) &mdash; Inverts the input value.
- [Math](18_math.md) &mdash; Apply various mathematical functions to two input height maps.
- [Normalize](19_normalize.md) &mdash; Normalizes the input vector.
- [Smooth](20_smooth.md) &mdash; Apply a smoothing function to the intensity map. Smooth is done through a simple blur kernel.
- [Vector Dot Product](21_vectordot.md) &mdash; Applies vector dot product to two input vectors producing a single mono value output.
- [Vector To Intensity](22_vectorintensity.md) &mdash; Convert a vector value into a mono value using a selected method.
- [Vector Math](23_vectormath.md) &mdash; Apply element-wise mathematical functions to two vector maps.

#### Output

- [Output](24_output.md) &mdash; The output of the node where the final height map is inputted to.