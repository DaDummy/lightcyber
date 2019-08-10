/* Lightcyber by Team210 - 64k intro by Team210 at Solskogen 2k19
* Copyright (C) 2019  Alexander Kraus <nr4@z10.info>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#version 130

uniform float iTime;
uniform vec2 iResolution;
uniform float iFader0;
uniform float iFader1;
uniform float iFader2;
uniform float iFader3;
uniform float iFader4;
uniform float iFader5;
uniform float iFader6;
uniform float iFader7;

// Global constants
const float pi = acos(-1.);
const vec3 c = vec3(1.0, 0.0, -1.0);
float a = 1.0;

float iScale, nbeats;

void rand(in vec2 x, out float n);
void dbox(in vec2 x, in vec2 b, out float d);
void stroke(in float d0, in float s, out float d);
void devoke(in vec2 x, out float d)
{
    x.x += .225;
    x *= 1.1;
    
    // o
    d = length(x+.35*c.xy)-.1;
    stroke(d,.06,d);
    
    // I
    float da;
    dbox(x+.1*c.xy, vec2(.05, .25), da);
    d = min(d, da);
    
    x = 2.*x - vec2(.4,-.2);
    // Mercury
    // Upper part
    dbox(x-.35*c.yx,vec2(.4,.35), da);
    d = min(d,da);
    dbox(x-.7*c.yx, vec2(.2,.2), da);
    d = max(d,-da);
    dbox(x-.25*c.yx,vec2(.2,.05),da);
    d = max(d,-da);
    
    // Lower part
    dbox(x+.1*c.yx,vec2(.1,.2),da);
    d = min(d,da);
    dbox(x+.2*c.yx, vec2(.4,.1),da);
    d = min(d,da);
    
    x = .5*(x + vec2(.4,-.2));
    
    // E
    // Right
    dbox(x-.9*c.xy, vec2(.05, .25), da);
    d = min(d,da);
    
    // Top/bot
    dbox(vec2(x.x-.7, abs(x.y)-.2), vec2(.2, .05), da);
    d = min(d,da);
    
    // Middle
    dbox(x-.7*c.xy, vec2(.2, .05), da);
    d = min(d,da);
    
    // Appendix
    dbox(vec2(x.x-.95,x.y+.2), vec2(.05,.05), da);
    d = min(d,da);
    
    stroke(d,.001,d);
}

float sm(float d)
{
    return smoothstep(1.5/iResolution.y, -1.5/iResolution.y, d);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    a = iResolution.x/iResolution.y;
    
    nbeats = mod(iTime, 60./29.);
    iScale = nbeats-30./29.;
    iScale = smoothstep(-5./29., 0., iScale)*(1.-smoothstep(0., 15./29., iScale));
    
    vec2 uv = fragCoord/iResolution.yy-0.5*vec2(a, 1.0), 
        s;
    vec3 col = vec3(0.20,0.01,0.14), 
        o = c.yyx,
        r = c.xyy, 
        u = c.yxy,
        t = c.yyy, 
        dir,
        n,
        x;
    float d, i, ra;
    t = uv.x * r + uv.y * u;
    dir = normalize(t-o);

    for(i=mix(0., 1.4, smoothstep(0., 1., iTime)); i>=0.; i -= .01)
    {
        rand(i*c.xx, ra);
        
		d = -(o.z-.2+i)/dir.z;
        x = o + d * dir;
        
        devoke(x.xy, s.x);
        s.x -= .01*iScale;
        
        if(ra < .5)
        {
            vec3 c1 = mix(mix(vec3(0.75,0.24,0.31), vec3(1.00,0.87,0.57), smoothstep(1.25,1.4,1.4-i)),vec3(0.20,0.01,0.14),i/1.4);
	        col = mix(col, c1, sm(s.x));
            col = mix(col, mix(col,vec3(.7,.45,.3), mix(.02,.1,iScale)), sm(s.x/64.));
        }
    }
    
    col = mix(col, c.yyy, clamp((d-2.-(o.z-.2)/dir.z)/4.,0.,1.));
    fragColor = vec4(clamp(col,0.,1.),1.0);
}

void main()
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}