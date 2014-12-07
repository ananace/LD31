uniform vec4 center;
uniform vec4 color;

void main() {
    float distance = distance(gl_FragCoord.xy, center.xy);

    if (distance > center.z)
    	gl_FragColor = mix(gl_Color, color, 1.0 - (distance / center.w));
    else
    	gl_FragColor = mix(gl_Color, color, 1.0 - (center.z / center.w));
}