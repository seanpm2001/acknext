#ifndef _UNIFORM
#error _UNIFORM must be defined!
#endif

// Common Variables:
_UNIFORM(vecViewPort, GL_FLOAT_VEC4, VECVIEWPORT_VAR, VECTOR4)
_UNIFORM(fGamma, GL_FLOAT, FGAMMA_VAR, float)
_UNIFORM(vecTime, GL_FLOAT_VEC2, VECTIME_VAR, VECTOR2)
_UNIFORM(iDebugMode, GL_INT, IDEBUGMODE_VAR, int)
_UNIFORM(texNoise, GL_SAMPLER_2D, TEXNOISE_VAR, BITMAP*)

// Object Shader

_UNIFORM(matWorld, GL_FLOAT_MAT4, MATWORLD_VAR, MATRIX)
_UNIFORM(matWorldView, GL_FLOAT_MAT4, MATWORLDVIEW_VAR, MATRIX)
_UNIFORM(matWorldViewProj, GL_FLOAT_MAT4, MATWORLDVIEWPROJ_VAR, MATRIX)
_UNIFORM(matView, GL_FLOAT_MAT4, MATVIEW_VAR, MATRIX)
_UNIFORM(matViewProj, GL_FLOAT_MAT4, MATVIEWPROJ_VAR, MATRIX)
_UNIFORM(matProj, GL_FLOAT_MAT4, MATPROJ_VAR, MATRIX)

_UNIFORM(vecViewPos, GL_FLOAT_VEC3, VECVIEWPOS_VAR, VECTOR)
_UNIFORM(vecViewDir, GL_FLOAT_VEC3, VECVIEWDIR_VAR, VECTOR)
_UNIFORM(fArc, GL_FLOAT, FARC_VAR, float)

_UNIFORM(vecAlbedo, GL_FLOAT_VEC4, VECALBEDO_VAR, COLOR)
_UNIFORM(vecEmission, GL_FLOAT_VEC4, VECEMISSION_VAR, COLOR)
_UNIFORM(vecAttributes, GL_FLOAT_VEC3, VECATTRIBUTES_VAR, VECTOR)

_UNIFORM(texColor, GL_SAMPLER_2D, TEXCOLOR_VAR, int)
_UNIFORM(texAttributes, GL_SAMPLER_2D, TEXATTRIBUTES_VAR, int)
_UNIFORM(texEmission, GL_SAMPLER_2D, TEXEMISSION_VAR, int)
_UNIFORM(texNormalMap, GL_SAMPLER_2D, TEXNORMALMAP_VAR, int)


_UNIFORM(vecFogColor, GL_FLOAT_VEC4, VECFOGCOLOR_VAR, COLOR)

_UNIFORM(iLightCount, GL_INT, ILIGHTCOUNT_VAR, int)

_UNIFORM(useInstancing, GL_BOOL, USEINSTANCING_VAR, int)
_UNIFORM(useBones, GL_BOOL, USEBONES_VAR, int)
_UNIFORM(useNormalMapping, GL_BOOL, USENORMALMAPPING_VAR, int)

// Post Processing:
_UNIFORM(texInput, GL_SAMPLER_2D, TEXINPUT_VAR, BITMAP*)
_UNIFORM(texBloom, GL_SAMPLER_2D, TEXBLOOM_VAR, BITMAP*)
_UNIFORM(fExposure, GL_FLOAT, FEXPOSURE_VAR, float)
_UNIFORM(fCutoff, GL_FLOAT, FCUTOFF_VAR, float)

_UNIFORM(texPosition, GL_SAMPLER_2D, TEXPOSITION_VAR, BITMAP*)
_UNIFORM(texNormal, GL_SAMPLER_2D, TEXNORMAL_VAR, BITMAP*)
_UNIFORM(texOcclusion, GL_SAMPLER_2D, TEXOCCLUSION_VAR, BITMAP*)

_UNIFORM(vecBlurScale, GL_FLOAT_VEC2, VECBLURSCALE_VAR, VECTOR2)

/*
UNIFORM(iLightType, GL_INT, ILIGHTTYPE_VAR)
UNIFORM(fLightIntensity, GL_FLOAT, FLIGHTINTENSITY_VAR)
UNIFORM(fLightArc, GL_FLOAT, FLIGHTARC_VAR)
UNIFORM(vecLightPos, GL_FLOAT_VEC3, VECLIGHTPOS_VAR)
UNIFORM(vecLightDir, GL_FLOAT_VEC3, VECLIGHTDIR_VAR)
UNIFORM(vecLightColor, GL_FLOAT_VEC3, VECLIGHTCOLOR_VAR)
*/