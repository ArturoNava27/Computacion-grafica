#version 330

out vec4 color;
uniform vec3 uniformColor; // Esta variable debe coincidir con el nombre en C++

void main() {
    color = vec4(uniformColor, 1.0); 
}