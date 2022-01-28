/*
WARNING: THIS FILE IS GENERATED BY 'generatebuiltinsl.c'.
YOU SHOULD NOT EDIT THIS FILE BY HAND
*/

#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/lightmapped",
"!!ver 110\n"
"!!permu FOG\n"
"!!permu BUMP\n"
"!!permu LIGHTSTYLED\n"
"!!permu REFLECTCUBEMASK\n"
"!!samps diffuse\n"

"!!samps lightmap\n"
"!!samps =LIGHTSTYLED lightmap1 lightmap2 lightmap3\n"

"!!samps =BUMP normalmap\n"

// envmaps only
"!!samps =REFLECTCUBEMASK reflectmask reflectcube\n"

"!!permu FAKESHADOWS\n"
"!!cvardf r_glsl_pcf\n"
"!!samps =FAKESHADOWS shadowmap\n"

"#include \"sys/defs.h\"\n"

"varying vec2 tex_c;\n"

"varying vec2 lm0;\n"

"#ifdef LIGHTSTYLED\n"
"varying vec2 lm1, lm2, lm3;\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"varying vec4 vtexprojcoord;\n"
"#endif\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"varying vec3 eyevector;\n"
"varying mat3 invsurface;\n"
"#endif\n"

"#ifdef VERTEX_SHADER\n"
"void lightmapped_init(void)\n"
"{\n"
"lm0 = v_lmcoord;\n"
"#ifdef LIGHTSTYLED\n"
"lm1 = v_lmcoord2;\n"
"lm2 = v_lmcoord3;\n"
"lm3 = v_lmcoord4;\n"
"#endif\n"
"}\n"

"void main ()\n"
"{\n"
"lightmapped_init();\n"
"tex_c = v_texcoord;\n"
"gl_Position = ftetransform();\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"invsurface = mat3(v_svector, v_tvector, v_normal);\n"

"vec3 eyeminusvertex = e_eyepos - v_position.xyz;\n"
"eyevector.x = dot(eyeminusvertex, v_svector.xyz);\n"
"eyevector.y = dot(eyeminusvertex, v_tvector.xyz);\n"
"eyevector.z = dot(eyeminusvertex, v_normal.xyz);\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"vtexprojcoord = (l_cubematrix*vec4(v_position.xyz, 1.0));\n"
"#endif\n"
"}\n"
"#endif\n"

"#ifdef FRAGMENT_SHADER\n"
"#include \"sys/fog.h\"\n"

"#ifdef FAKESHADOWS\n"
"#include \"sys/pcf.h\"\n"
"#endif\n"

"#ifdef LIGHTSTYLED\n"
"#define LIGHTMAP0 texture2D(s_lightmap0, lm0).rgb\n"
"#define LIGHTMAP1 texture2D(s_lightmap1, lm1).rgb\n"
"#define LIGHTMAP2 texture2D(s_lightmap2, lm2).rgb\n"
"#define LIGHTMAP3 texture2D(s_lightmap3, lm3).rgb\n"
"#else\n"
"#define LIGHTMAP texture2D(s_lightmap, lm0).rgb \n"
"#endif\n"

"vec3 lightmap_fragment()\n"
"{\n"
"vec3 lightmaps;\n"

"#ifdef LIGHTSTYLED\n"
"lightmaps  = LIGHTMAP0 * e_lmscale[0].rgb;\n"
"lightmaps += LIGHTMAP1 * e_lmscale[1].rgb;\n"
"lightmaps += LIGHTMAP2 * e_lmscale[2].rgb;\n"
"lightmaps += LIGHTMAP3 * e_lmscale[3].rgb;\n"
"#else\n"
"lightmaps  = LIGHTMAP * e_lmscale.rgb;\n"
"#endif\n"

/* the light we're getting is always too bright */
"lightmaps *= 0.75;\n"

/* clamp at 1.5 */
"if (lightmaps.r > 1.5)\n"
"lightmaps.r = 1.5;\n"
"if (lightmaps.g > 1.5)\n"
"lightmaps.g = 1.5;\n"
"if (lightmaps.b > 1.5)\n"
"lightmaps.b = 1.5;\n"

"return lightmaps;\n"
"}\n"

"void main (void)\n"
"{\n"
"vec4 diffuse_f;\n"

"diffuse_f = texture2D(s_diffuse, tex_c);\n"

"#ifdef MASKLT\n"
"if (diffuse_f.a < float(MASK))\n"
"discard;\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"diffuse_f.rgb *= ShadowmapFilter(s_shadowmap, vtexprojcoord);\n"
"#endif\n"

/* deluxemapping isn't working on Source BSP yet */
"diffuse_f.rgb *= lightmap_fragment();\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
/* We currently only use the normal/bumpmap for cubemap warping. move this block out once we do proper radiosity normalmapping */
"#ifdef BUMP\n"
/* Source's normalmaps are in the DX format where the green channel is flipped */
"vec4 normal_f = texture2D(s_normalmap, tex_c);\n"
"normal_f.g *= -1.0;\n"
"normal_f.rgb = normalize(normal_f.rgb - 0.5);\n"
"#else\n"
"vec4 normal_f = vec4(0.0,0.0,1.0,0.0);\n"
"#endif\n"

"#if defined(ENVFROMMASK)\n"
/* We have a dedicated reflectmask */
"#define refl texture2D(s_reflectmask, tex_c).r\n"
"#else\n"
/* when ENVFROMBASE is set or a normal isn't present, we're getting the reflectivity info from the diffusemap's alpha channel */
"#if defined(ENVFROMBASE) || !defined(BUMP)\n"
"#define refl 1.0 - diffuse_f.a\n"
"#else\n"
"#define refl normal_f.a * 0.5\n"
"#endif\n"
"#endif\n"

"vec3 cube_c = reflect(normalize(-eyevector), normal_f.rgb);\n"
"cube_c = cube_c.x * invsurface[0] + cube_c.y * invsurface[1] + cube_c.z * invsurface[2];\n"
"cube_c = (m_model * vec4(cube_c.xyz, 0.0)).xyz;\n"
"diffuse_f.rgb += (textureCube(s_reflectcube, cube_c).rgb * vec3(refl,refl,refl));\n"
"#endif\n"

"gl_FragColor = fog4(diffuse_f);\n"
"}\n"
"#endif\n"
},
#endif
#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/refract",
"!!ver 110\n"
"!!samps diffuse\n"
"!!samps =BUMP normalmap\n"
"!!samps =REFLECTCUBEMASK reflectmask reflectcube\n"
"!!samps refraction=0\n"

"#include \"sys/defs.h\"\n"

"varying vec2 tex_c;\n"
"varying mat3 invsurface;\n"
"varying vec4 tf_c;\n"
"varying vec3 eyeminusvertex;\n"

"#ifdef VERTEX_SHADER\n"
"void main ()\n"
"{\n"
"invsurface[0] = v_svector;\n"
"invsurface[1] = v_tvector;\n"
"invsurface[2] = v_normal;\n"
"tf_c = ftetransform();\n"
"tex_c = v_texcoord;\n"
"gl_Position = tf_c;\n"
"}\n"
"#endif\n"

"#ifdef FRAGMENT_SHADER\n"
"#include \"sys/fog.h\"\n"
"void main ( void )\n"
"{\n"
"vec2 refl_c;\n"
"vec3 refr_f;\n"
"vec3 norm_f;\n"
"vec4 out_f = vec4( 1.0, 1.0, 1.0, 1.0 );\n"

"norm_f = ( texture2D( s_normalmap, tex_c).xyz);\n"
"norm_f.g *= -1.0;\n"
"norm_f = normalize( norm_f );\n"

// Reflection/View coordinates
"refl_c = ( 1.0 + ( tf_c.xy / tf_c.w ) ) * 0.5;\n"

"refr_f = texture2D(s_refraction, refl_c + (norm_f.st) ).rgb;\n"
"out_f.rgb = refr_f * texture2D(s_diffuse, tex_c).rgb;\n"

"gl_FragColor = out_f;\n"
"}\n"
"#endif\n"
},
#endif
#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/transition",
"!!ver 110\n"
"!!permu FOG\n"
"!!permu BUMP\n"
"!!permu LIGHTSTYLED\n"
"!!permu REFLECTCUBEMASK\n"
"!!permu UPPERLOWER\n"
"!!samps diffuse upper\n"

"!!samps lightmap\n"
"!!samps =LIGHTSTYLED lightmap1 lightmap2 lightmap3\n"

"!!samps =BUMP normalmap\n"

// envmaps only
"!!samps =REFLECTCUBEMASK reflectmask reflectcube\n"

"!!permu FAKESHADOWS\n"
"!!cvardf r_glsl_pcf\n"
"!!samps =FAKESHADOWS shadowmap\n"

"#include \"sys/defs.h\"\n"

"varying vec2 tex_c;\n"
"varying vec4 vex_color;\n"

"varying vec2 lm0;\n"

"#ifdef LIGHTSTYLED\n"
"varying vec2 lm1, lm2, lm3;\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"varying vec4 vtexprojcoord;\n"
"#endif\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"varying vec3 eyevector;\n"
"varying mat3 invsurface;\n"
"#endif\n"

"#ifdef VERTEX_SHADER\n"
"void lightmapped_init(void)\n"
"{\n"
"lm0 = v_lmcoord;\n"
"#ifdef LIGHTSTYLED\n"
"lm1 = v_lmcoord2;\n"
"lm2 = v_lmcoord3;\n"
"lm3 = v_lmcoord4;\n"
"#endif\n"
"}\n"

"void main ()\n"
"{\n"
"lightmapped_init();\n"
"tex_c = v_texcoord;\n"
"gl_Position = ftetransform();\n"
"vex_color = v_colour;\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"invsurface = mat3(v_svector, v_tvector, v_normal);\n"

"vec3 eyeminusvertex = e_eyepos - v_position.xyz;\n"
"eyevector.x = dot(eyeminusvertex, v_svector.xyz);\n"
"eyevector.y = dot(eyeminusvertex, v_tvector.xyz);\n"
"eyevector.z = dot(eyeminusvertex, v_normal.xyz);\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"vtexprojcoord = (l_cubematrix*vec4(v_position.xyz, 1.0));\n"
"#endif\n"
"}\n"
"#endif\n"

"#ifdef FRAGMENT_SHADER\n"
"#include \"sys/fog.h\"\n"

"#ifdef FAKESHADOWS\n"
"#include \"sys/pcf.h\"\n"
"#endif\n"

"#ifdef LIGHTSTYLED\n"
"#define LIGHTMAP0 texture2D(s_lightmap0, lm0).rgb\n"
"#define LIGHTMAP1 texture2D(s_lightmap1, lm1).rgb\n"
"#define LIGHTMAP2 texture2D(s_lightmap2, lm2).rgb\n"
"#define LIGHTMAP3 texture2D(s_lightmap3, lm3).rgb\n"
"#else\n"
"#define LIGHTMAP texture2D(s_lightmap, lm0).rgb \n"
"#endif\n"

"vec3 lightmap_fragment()\n"
"{\n"
"vec3 lightmaps;\n"

"#ifdef LIGHTSTYLED\n"
"lightmaps  = LIGHTMAP0 * e_lmscale[0].rgb;\n"
"lightmaps += LIGHTMAP1 * e_lmscale[1].rgb;\n"
"lightmaps += LIGHTMAP2 * e_lmscale[2].rgb;\n"
"lightmaps += LIGHTMAP3 * e_lmscale[3].rgb;\n"
"#else\n"
"lightmaps  = LIGHTMAP * e_lmscale.rgb;\n"
"#endif\n"

/* the light we're getting is always too bright */
"lightmaps *= 0.75;\n"

/* clamp at 1.5 */
"if (lightmaps.r > 1.5)\n"
"lightmaps.r = 1.5;\n"
"if (lightmaps.g > 1.5)\n"
"lightmaps.g = 1.5;\n"
"if (lightmaps.b > 1.5)\n"
"lightmaps.b = 1.5;\n"

"return lightmaps;\n"
"}\n"

"void main (void)\n"
"{\n"
"vec4 diffuse_f;\n"
"diffuse_f.rgb = mix(texture2D(s_diffuse, tex_c).rgb,  texture2D(s_upper, tex_c).rgb, vex_color.a);\n"
"diffuse_f.a = 1.0;\n"

/* deluxemapping isn't working on Source BSP yet, FIXME */
"diffuse_f.rgb *= lightmap_fragment();\n"

"#ifdef FAKESHADOWS\n"
"diffuse_f.rgb *= ShadowmapFilter(s_shadowmap, vtexprojcoord);\n"
"#endif\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
/* We currently only use the normal/bumpmap for cubemap warping. move this block out once we do proper radiosity normalmapping */
"#ifdef BUMP\n"
/* Source's normalmaps are in the DX format where the green channel is flipped */
"vec4 normal_f = texture2D(s_normalmap, tex_c);\n"
"normal_f.g *= -1.0;\n"
"normal_f.rgb = normalize(normal_f.rgb - 0.5);\n"
"#else\n"
"vec4 normal_f = vec4(0.0,0.0,1.0,0.0);\n"
"#endif\n"

/* when ENVFROMBASE is set or a normal isn't present, we're getting the reflectivity info from the diffusemap's alpha channel */
"#if defined(ENVFROMBASE) || !defined(BUMP)\n"
/* since we're sampling from the diffuse = 1.0 fully visible, 0.0 = fully reflective */
"#define refl 1.0 - diffuse_f.a\n"
"#else\n"
"#define refl normal_f.a * 0.5\n"
"#endif\n"

"vec3 cube_c = reflect(normalize(-eyevector), normal_f.rgb);\n"
"cube_c = cube_c.x * invsurface[0] + cube_c.y * invsurface[1] + cube_c.z * invsurface[2];\n"
"cube_c = (m_model * vec4(cube_c.xyz, 0.0)).xyz;\n"
"diffuse_f.rgb += (textureCube(s_reflectcube, cube_c).rgb * vec3(refl,refl,refl));\n"
"#endif\n"

"gl_FragColor = fog4(diffuse_f);\n"
"}\n"
"#endif\n"
},
#endif
#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/unlit",
"!!ver 110\n"
"!!permu FOG\n"
"!!samps diffuse\n"

"#include \"sys/defs.h\"\n"
"#include \"sys/fog.h\"\n"

"varying vec2 tex_c;\n"

"#ifdef VERTEX_SHADER\n"
"void main ()\n"
"{\n"
"tex_c = v_texcoord;\n"
"gl_Position = ftetransform();\n"
"}\n"
"#endif\n"

"#ifdef FRAGMENT_SHADER\n"
"void main ()\n"
"{\n"
"vec4 diffuse_f = texture2D( s_diffuse, tex_c );\n"

"#ifdef MASKLT\n"
"if (diffuse_f.a < float(MASK))\n"
"discard;\n"
"#endif\n"

"gl_FragColor = fog4( diffuse_f );\n"
"}\n"
"#endif\n"
},
#endif
#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/vertexlit",
"!!ver 110\n"
"!!permu FRAMEBLEND\n"
"!!permu BUMP\n"
"!!permu FOG\n"
"!!permu SKELETAL\n"
"!!permu AMBIENTCUBE\n"
"!!samps diffuse fullbright normalmap\n"
"!!permu FAKESHADOWS\n"
"!!cvardf r_glsl_pcf\n"
"!!samps =FAKESHADOWS shadowmap\n"

// envmaps only
"!!samps =REFLECTCUBEMASK reflectmask reflectcube\n"

"!!cvardf r_skipDiffuse\n"

"#include \"sys/defs.h\"\n"

"varying vec2 tex_c;\n"
"varying vec3 norm;\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"varying vec3 eyevector;\n"
"varying mat3 invsurface;\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"varying vec4 vtexprojcoord;\n"
"#endif\n"

"#ifdef VERTEX_SHADER\n"
"#include \"sys/skeletal.h\"\n"

"void main (void)\n"
"{\n"
"vec3 n, s, t, w;\n"
"tex_c = v_texcoord;\n"
"gl_Position = skeletaltransform_wnst(w,n,s,t);\n"
"norm = n;\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
"invsurface = mat3(v_svector, v_tvector, v_normal);\n"

"vec3 eyeminusvertex = e_eyepos - v_position.xyz;\n"
"eyevector.x = dot(eyeminusvertex, v_svector.xyz);\n"
"eyevector.y = dot(eyeminusvertex, v_tvector.xyz);\n"
"eyevector.z = dot(eyeminusvertex, v_normal.xyz);\n"
"#endif\n"

"#ifdef FAKESHADOWS\n"
"vtexprojcoord = (l_cubematrix*vec4(v_position.xyz, 1.0));\n"
"#endif\n"
"}\n"
"#endif\n"


"#ifdef FRAGMENT_SHADER\n"
"#include \"sys/fog.h\"\n"
"#include \"sys/pcf.h\"\n"

"float lambert(vec3 normal, vec3 dir)\n"
"{\n"
"return max(dot(normal, dir), 0.0);\n"
"}\n"

"float halflambert(vec3 normal, vec3 dir)\n"
"{\n"
"return (lambert(normal, dir) * 0.5) + 0.5;\n"
"}\n"

"void main (void)\n"
"{\n"
"vec4 diffuse_f = texture2D(s_diffuse, tex_c);\n"
"vec3 light;\n"

"#ifdef MASKLT\n"
"if (diffuse_f.a < float(MASK))\n"
"discard;\n"
"#endif\n"

/* Normal/Bumpmap Shenanigans */
"#ifdef BUMP\n"
/* Source's normalmaps are in the DX format where the green channel is flipped */
"vec3 normal_f = texture2D(s_normalmap, tex_c).rgb;\n"
"normal_f.g *= -1.0;\n"
"normal_f = normalize(normal_f.rgb - 0.5);\n"
"#else\n"
"vec3 normal_f = vec3(0.0,0.0,1.0);\n"
"#endif\n"

/* CUBEMAPS ONLY */
"#ifdef REFLECTCUBEMASK\n"
/* when ENVFROMBASE is set or a normal isn't present, we're getting the reflectivity info from the diffusemap's alpha channel */
"#if defined(ENVFROMBASE) || !defined(BUMP)\n"
"#define refl 1.0 - diffuse_f.a\n"
"#else\n"
"#define refl texture2D(s_normalmap, tex_c).a\n"
"#endif\n"
"vec3 cube_c = reflect(normalize(-eyevector), normal_f.rgb);\n"
"cube_c = cube_c.x * invsurface[0] + cube_c.y * invsurface[1] + cube_c.z * invsurface[2];\n"
"cube_c = (m_model * vec4(cube_c.xyz, 0.0)).xyz;\n"
"diffuse_f.rgb += (textureCube(s_reflectcube, cube_c).rgb * vec3(refl,refl,refl));\n"
"#endif\n"

"#ifdef AMBIENTCUBE\n"
//no specular effect here. use rtlights for that.
"vec3 nn = norm*norm; //FIXME: should be worldspace normal.\n"
"light = nn.x * e_light_ambientcube[(norm.x<0.0)?1:0] +\n"
"nn.y * e_light_ambientcube[(norm.y<0.0)?3:2] +\n"
"nn.z * e_light_ambientcube[(norm.z<0.0)?5:4];\n"
"#else\n"
"#ifdef HALFLAMBERT\n"
"light = e_light_ambient + (e_light_mul * halflambert(norm, e_light_dir));\n"
"#else\n"
"light = e_light_ambient + (e_light_mul * lambert(norm, e_light_dir));\n"
"#endif\n"

/* the light we're getting is always too bright */
"light *= 0.75;\n"

/* clamp at 1.5 */
"if (light.r > 1.5)\n"
"light.r = 1.5;\n"
"if (light.g > 1.5)\n"
"light.g = 1.5;\n"
"if (light.b > 1.5)\n"
"light.b = 1.5;\n"
"#endif\n"

"diffuse_f.rgb *= light;\n"

"#ifdef FAKESHADOWS\n"
"diffuse_f.rgb *= ShadowmapFilter(s_shadowmap, vtexprojcoord);\n"
"#endif\n"

"gl_FragColor = fog4(diffuse_f * e_colourident) * e_lmscale;\n"
"}\n"
"#endif\n"
},
#endif
#ifdef GLQUAKE
{QR_OPENGL, 110, "vmt/water",
"!!cvardf r_glsl_turbscale_reflect=1 //simpler scaler\n"
"!!cvardf r_glsl_turbscale_refract=1 //simpler scaler\n"
"!!samps diffuse normalmap\n"
"!!samps refract=0 //always present\n"
"!!samps =REFLECT reflect=1\n"
"!!samps !REFLECT reflectcube\n"
"!!permu FOG\n"

"#include \"sys/defs.h\"\n"

//modifier: REFLECT		(s_t2 is a reflection instead of diffusemap)
//modifier: STRENGTH_REFL	(distortion strength - 0.1 = fairly gentle, 0.2 = big waves)
//modifier: STRENGTH_REFL	(distortion strength - 0.1 = fairly gentle, 0.2 = big waves)
//modifier: FRESNEL_EXP	(5=water)
//modifier: TXSCALE		(wave size - 0.2)
//modifier: RIPPLEMAP		(s_t3 contains a ripplemap
//modifier: TINT_REFR		(some colour value)
//modifier: TINT_REFL		(some colour value)
//modifier: ALPHA		(mix in the normal water texture over the top)
//modifier: USEMODS		(use single-texture scrolling via tcmods - note, also forces the engine to actually use tcmod etc)

//a few notes on DP compat:
//'dpwater' makes numerous assumptions about DP internals
//by default there is a single pass that uses the pass's normal tcmods
//the fresnel has a user-supplied min+max rather than an exponent
//both parts are tinted individually
//if alpha is enabled, the regular water texture is blended over the top, again using the same crappy tcmods...

//legacy crap
"#ifndef FRESNEL\n"
"#define FRESNEL 5.0\n"
"#endif\n"
"#ifndef TINT\n"
"#define TINT 0.7,0.8,0.7\n"
"#endif\n"
"#ifndef STRENGTH\n"
"#define STRENGTH 0.1\n"
"#endif\n"
"#ifndef TXSCALE\n"
"#define TXSCALE 1\n"
"#endif\n"

//current values (referring to legacy defaults where needed)
"#ifndef FRESNEL_EXP\n"
"#define FRESNEL_EXP 4.0\n"
"#endif\n"
"#ifndef FRESNEL_MIN\n"
"#define FRESNEL_MIN 0.0\n"
"#endif\n"
"#ifndef FRESNEL_RANGE\n"
"#define FRESNEL_RANGE 1.0\n"
"#endif\n"
"#ifndef STRENGTH_REFL\n"
"#define STRENGTH_REFL STRENGTH\n"
"#endif\n"
"#ifndef STRENGTH_REFR\n"
"#define STRENGTH_REFR STRENGTH\n"
"#endif\n"
"#ifndef TXSCALE1\n"
"#define TXSCALE1 TXSCALE\n"
"#endif\n"
"#ifndef TXSCALE2\n"
"#define TXSCALE2 TXSCALE\n"
"#endif\n"
"#ifndef TINT_REFR\n"
"#define TINT_REFR TINT\n"
"#endif\n"
"#ifndef TINT_REFL\n"
"#define TINT_REFL 1.0,1.0,1.0\n"
"#endif\n"
"#ifndef FOGTINT\n"
"#define FOGTINT 0.2,0.3,0.2\n"
"#endif\n"

"varying vec2 tc;\n"
"varying vec4 tf;\n"
"varying vec3 norm;\n"
"varying vec3 eye;\n"
"#ifdef VERTEX_SHADER\n"
"void main (void)\n"
"{\n"
"tc = v_texcoord.st;\n"
"tf = ftetransform();\n"
"norm = v_normal;\n"
"eye = e_eyepos - v_position.xyz;\n"
"gl_Position = ftetransform();\n"
"}\n"
"#endif\n"
"#ifdef FRAGMENT_SHADER\n"
"#include \"sys/fog.h\"\n"


"void main (void)\n"
"{\n"
"vec2 stc; //screen tex coords\n"
"vec2 ntc; //normalmap/diffuse tex coords\n"
"vec3 n, refr, refl;\n"
"float fres;\n"
"float depth;\n"
"stc = (1.0 + (tf.xy / tf.w)) * 0.5;\n"
//hack the texture coords slightly so that there are less obvious gaps
"stc.t -= 1.5*norm.z/1080.0;\n"

"#if 0//def USEMODS\n"
"ntc = tc;\n"
"n = texture2D(s_normalmap, ntc).xyz - 0.5;\n"
"#else\n"
//apply q1-style warp, just for kicks
"ntc.s = tc.s + sin(tc.t+e_time)*0.125;\n"
"ntc.t = tc.t + sin(tc.s+e_time)*0.125;\n"

//generate the two wave patterns from the normalmap
"n = (texture2D(s_normalmap, vec2(TXSCALE1)*tc + vec2(e_time*0.1, 0.0)).xyz);\n"
"n += (texture2D(s_normalmap, vec2(TXSCALE2)*tc - vec2(0, e_time*0.097)).xyz);\n"
"n -= 1.0 - 4.0/256.0;\n"
"#endif\n"

"#ifdef RIPPLEMAP\n"
"n += texture2D(s_ripplemap, stc).rgb*3.0;\n"
"#endif\n"
"n = normalize(n);\n"

//the fresnel term decides how transparent the water should be
"fres = pow(1.0-abs(dot(n, normalize(eye))), float(FRESNEL_EXP)) * float(FRESNEL_RANGE) + float(FRESNEL_MIN);\n"

"#ifdef DEPTH\n"
"float far = #include \"cvar/gl_maxdist\";\n"
"float near = #include \"cvar/gl_mindist\";\n"
//get depth value at the surface
"float sdepth = gl_FragCoord.z;\n"
"sdepth = (2.0*near) / (far + near - sdepth * (far - near));\n"
"sdepth = mix(near, far, sdepth);\n"

//get depth value at the ground beyond the surface.
"float gdepth = texture2D(s_refractdepth, stc).x;\n"
"gdepth = (2.0*near) / (far + near - gdepth * (far - near));\n"
"if (gdepth >= 0.5)\n"
"{\n"
"gdepth = sdepth;\n"
"depth = 0.0;\n"
"}\n"
"else\n"
"{\n"
"gdepth = mix(near, far, gdepth);\n"
"depth = gdepth - sdepth;\n"
"}\n"

//reduce the normals in shallow water (near walls, reduces the pain of linear sampling)
"if (depth < 100.0)\n"
"n *= depth/100.0;\n"
"#else\n"
"depth = 1.0;\n"
"#endif \n"


//refraction image (and water fog, if possible)
"refr = texture2D(s_refract, stc + n.st*float(STRENGTH_REFR)*float(r_glsl_turbscale_refract)).rgb * vec3(TINT_REFR);\n"
"#ifdef DEPTH\n"
"refr = mix(refr, vec3(FOGTINT), min(depth/4096.0, 1.0));\n"
"#endif\n"

"#ifdef REFLECT\n"
//reflection/diffuse
"refl = texture2D(s_reflect, stc - n.st*float(STRENGTH_REFL)*float(r_glsl_turbscale_reflect)).rgb * vec3(TINT_REFL);\n"
"#else\n"
"refl = textureCube(s_reflectcube, n).rgb;// * vec3(TINT_REFL);\n"
"#endif\n"

//interplate by fresnel
"refr = mix(refr, refl, fres);\n"

"#ifdef ALPHA\n"
"vec4 ts = texture2D(s_diffuse, ntc);\n"
"vec4 surf = fog4blend(vec4(ts.rgb, float(ALPHA)*ts.a));\n"
"refr = mix(refr, surf.rgb, surf.a);\n"
"#else\n"
"refr = fog3(refr); \n"
"#endif\n"

//done
"gl_FragColor = vec4(refr, 1.0);\n"
"}\n"
"#endif\n"
},
#endif