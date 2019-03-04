# renderer
A work-in-progress renderer made with Vulkan. It is designed with a data-orientedness in mind and its goal is to try to fit all application logic in a single tree structure. As of still, there is quite a lot of hard-coding in the project, but the core structure of the renderer is slowly molding into its design.

Currently there's one example application that showcases the capabilities of this renderer. It is a procedurally-generated landscape made using a single quad subdivided with tessellation shaders. There's still room for improvement it respect to that, especially in getting the scene geometry to appear less detailed and more of it if observed from far away. The approach of not using any vertex buffers for this was slightly unorthodox, but it payed off, as it adds a certain amount of simpilicity to the programme.

On the to-do list is also making the folder structure clearer and dividing the project into engine code and application-specific code.

Build instructions:

cmake .

make

cd examples/shader

glslangValidator -V landscape.vert

glslangValidator -V landscape.tesc

glslangValidator -V landscape.tese

glslangValidator -V landscape.frag
