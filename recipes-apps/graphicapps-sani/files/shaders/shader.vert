#version 450

// vertex attributes from vertex buffers, retrieved at vertex input stage
layout(location = 0) in vec2 input_position;
layout(location = 1) in vec3 input_color;

// output data which will be fed into fragment shader
layout(location = 0) out vec3 frag_color;

void main()
{
    gl_Position = vec4(input_position, 0.0, 1.0);
    frag_color = input_color;
}
