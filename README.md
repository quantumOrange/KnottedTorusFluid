# A fluid simulation running on a torus tied in a knot.

I’ve thickened out a trefoil knot, and made it move and breath with a rhythmic pulse. The surface of the knot is textured with a fluid simulation. Particles float around in the fluid and leak coloured ink, which swirls around delightfully in the liquid surface of the knot.

The real time fluid simulation uses the CURL of Perlin noise to generate a flow field that shifts the fluid around. The flow field itself is also used as a bump map to texture the surface.

Made with openFrameworks and GLSL.
