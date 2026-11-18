// globals
float4x4	GClipMat		: register(c0);
float4x4	GObjMat			: register(c4);
float4x4	GObjMatIT		: register(c16);

float4		GMagicConst		: register(c8);		// (24.5, 255*3, 1, 0.5)
float4		GFogParams		: register(c9);

float4		GAmbientColor	: register(c11);
float4		GLightColor[4]	: register(c12);
float4		GLightPos[4]	: register(c16);

// single light pass
float4		GLightParams	: register(c12);
float4		GObjectLightPos	: register(c13);
float4		GObjectEyePos	: register(c14);
float3		GWorldLightPos	: register(c20);
float3		GWorldEyePos	: register(c21);
float3		GWorldLight2Pos	: register(c22);

float4		GTexTilePan[2]	: register(c24);
float4		GTexTilePan3[2]	: register(c33);

// pixel shader
float4		GPointLight			: register(c0);
float4		GPointLightColor	: register(c1);

float4		GPointLight2		: register(c2);
float4		GPointLightColor2	: register(c3);

// ---------------------------
// uber shader
// ---------------------------

// vertex
float3		GV_UberEyePos			: register(c40);
float3		GV_UberLightPos[4]		: register(c41);
float4		GV_UberFogColor			: register(c39);
float4		GV_UberFogHack			: register(c38);

// pixel
float4		GP_UberLight[4]			: register(c0);
float4		GP_UberLightColor[4]	: register(c4);
bool		GP_UberLightSwitch[4]	: register(b8);

// all shaders
float4		GP_Overbright			: register(c10);

// ------------------------------------------------------------------------------------------------

sampler	ColorSampler	: register (s0);
sampler GlossMapSampler	: register (s1);
sampler LightMapSampler	: register (s2);
sampler BlendSampler	: register (s3);
sampler AlphaSampler	: register (s4);

// ================================================================================================

struct VS_INPUT_NTU
{
	float4 Pos			: POSITION;
    float3 Normal		: NORMAL;
    float2 TexCoord		: TEXCOORD0;
};

struct VS_INPUT_TU2
{
	float4 Pos			: POSITION;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
};

struct VS_INPUT_T2L
{
	float4 Pos			: POSITION;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
    float3 Normal		: NORMAL;
};

struct VS_OUTPUT
{
    float4 Pos			: POSITION;   // vertex position 
    float4 Diffuse		: COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float1 Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
};

struct VS_OUTPUT_NTU
{
    float4 Pos			: POSITION;   // vertex position 
    float1 Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
    float3 WorldNormal	: TEXCOORD1;
    float3 LightVec		: TEXCOORD2;
    float3 EyeVec		: TEXCOORD3;
};

struct VS_OUTPUT_TU2
{
    float4 Pos			: POSITION;   // vertex position 
    float1 Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
    float3 WorldNormal	: TEXCOORD2;
    float3 LightVec		: TEXCOORD3;
    float3 EyeVec		: TEXCOORD4;
};

// ==========================================================================================================

VS_OUTPUT_TU2 PointPassTU2_VS(VS_INPUT_T2L In)
{
	VS_OUTPUT_TU2 Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
    Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;
	Out.Tex0.x		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[0]);
	Out.Tex0.y		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[1]);
	Out.Tex1		= In.Tex1;		
	Out.WorldNormal	= In.Normal;
	Out.LightVec	= GWorldLightPos - In.Pos;
	Out.EyeVec		= In.Pos - GWorldEyePos;

	return Out;
}

half4 PointPassTU2_PS(VS_OUTPUT_TU2 In) : COLOR
{
	half3 Normal = normalize(In.WorldNormal);		
	half3 EyeVec = normalize(In.EyeVec);
	
	half3 L = normalize(In.LightVec);
	
	half3 R = reflect(L, Normal);
	half S = pow(saturate(dot(R, EyeVec)), GPointLightColor.w);
	
	half NdotL = saturate(dot(L, Normal));

	float len = length(In.LightVec);
	float A = saturate(1 - len * GPointLight.w);

	float  DiffuseA		= tex2D(ColorSampler, In.Tex0).a;
	half3  GlossMask	= tex2D(GlossMapSampler, In.Tex0);
	half3  LMap			= tex2D(LightMapSampler, In.Tex1);

	half3 Final = (half3) GPointLightColor * GlossMask * NdotL * LMap * A * S * 4;
	
	return half4(Final, A) * DiffuseA;
}

// ==========================================================================================================

struct VS_OUT_TU2SPEC
{
    float4 Pos			: POSITION;   // vertex position 
    float1 Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
    float3 WorldNormal	: TEXCOORD2;
    float3 LightVec		: TEXCOORD3;
    float3 LightVec2	: TEXCOORD4;
    float3 EyeVec		: TEXCOORD5;
};

VS_OUT_TU2SPEC SpecularTU2_VS(VS_INPUT_T2L In)
{
	VS_OUT_TU2SPEC Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
    Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;
	Out.Tex0.x		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[0]);
	Out.Tex0.y		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[1]);
	Out.Tex1		= In.Tex1;		
	Out.WorldNormal	= In.Normal;
	Out.LightVec	= GWorldLightPos - In.Pos;
	Out.LightVec2	= GWorldLight2Pos - In.Pos;
	Out.EyeVec		= In.Pos - GWorldEyePos;

	return Out;
}

half4 SpecularTU2_PS(VS_OUT_TU2SPEC In) : COLOR
{	
	half3 Normal = normalize(In.WorldNormal);		
	half3 EyeVec = normalize(In.EyeVec);

	half3 R = reflect(EyeVec, Normal);
	
	// first light	
	half3 L = normalize(In.LightVec);
	half S = pow(saturate(dot(R, L)), GPointLightColor.w);
	
	half NdotL = saturate(dot(L, Normal));

	// 2nd light
	half3 L2 = normalize(In.LightVec2);
	
	//half3 R2 = reflect(L2, Normal);
	half S2 = pow(saturate(dot(R, L2)), GPointLightColor2.w);
	
	half NdotL2 = saturate(dot(L2, Normal));


	half4  Diffuse		= tex2D(ColorSampler, In.Tex0);
	half3  GlossMask	= tex2D(GlossMapSampler, In.Tex0);
		
	half SN1 = 4 * NdotL * S;
	half SN2 = 4 * NdotL2 * S2;
		
	//half3 F = Diffuse.xyz;
	half3 F = Diffuse.xyz * GP_Overbright.x;
	
	half3 P1 = (half3) GPointLightColor * SN1;
	half3 P2 = (half3) GPointLightColor2 * SN2;
	
	half3  LMap			= tex2D(LightMapSampler, In.Tex1);
	
	return half4((F + GlossMask*(P1 + P2)) * LMap, Diffuse.a);
}

// ==========================================================================================================

struct VS_OUT_TU2SPECBLEND
{
    float4 Pos			: POSITION;   // vertex position 
    float1 Fog			: FOG;
    float2 ColorTex		: TEXCOORD0;
    float2 BlendTex		: TEXCOORD1;
    float2 LmapTex		: TEXCOORD2;
    float3 WorldNormal	: TEXCOORD3;
    float3 LightVec		: TEXCOORD4;
    float3 LightVec2	: TEXCOORD5;
    float3 EyeVec		: TEXCOORD6;
};

VS_OUT_TU2SPECBLEND SpecularTU2Blend_VS(VS_INPUT_T2L In)
{
	VS_OUT_TU2SPECBLEND Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
    Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;

	Out.ColorTex.x	= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[0]);
	Out.ColorTex.y	= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[1]);

	Out.BlendTex.x	= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan3[0]);
	Out.BlendTex.y	= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan3[1]);

	Out.LmapTex		= In.Tex1;	
		
	Out.WorldNormal	= In.Normal;
	Out.LightVec	= GWorldLightPos - In.Pos;
	Out.LightVec2	= GWorldLight2Pos - In.Pos;
	Out.EyeVec		= In.Pos - GWorldEyePos;

	return Out;
}

half4 SpecularTU2Blend_PS(VS_OUT_TU2SPECBLEND In) : COLOR
{	
	half3 Normal = normalize(In.WorldNormal);		
	half3 EyeVec = normalize(In.EyeVec);

	half3 R = reflect(EyeVec, Normal);
	
	// first light	
	half3 L = normalize(In.LightVec);
	half S = pow(saturate(dot(R, L)), GPointLightColor.w);
	
	half NdotL = saturate(dot(L, Normal));

	// 2nd light
	half3 L2 = normalize(In.LightVec2);
	
	//half3 R2 = reflect(L2, Normal);
	half S2 = pow(saturate(dot(R, L2)), GPointLightColor2.w);
	
	half NdotL2 = saturate(dot(L2, Normal));


	// -----------------------------------------------------
	half4	Diffuse		= tex2D(ColorSampler, In.ColorTex);
	half4	Blend		= tex2D(BlendSampler, In.BlendTex);
	half4	Alpha		= tex2D(AlphaSampler, In.LmapTex);
	
	half4	Fa = Diffuse * (1-Alpha) + Blend * Alpha;

	half3	LMap		= tex2D(LightMapSampler, In.LmapTex);
	half3	GlossMask	= tex2D(GlossMapSampler, In.ColorTex);
	
	//half3	F = (half3) Fa * 2;
	half3	F = (half3) Fa * GP_Overbright.x;

	half SN1 = 4 * NdotL * S;
	half SN2 = 4 * NdotL2 * S2;

	half3 P1 = (half3) GPointLightColor * SN1;
	half3 P2 = (half3) GPointLightColor2 * SN2;

	return half4((F + GlossMask*(P1 + P2)) * LMap, Fa.a);
}

// =======================================================================================================

VS_OUTPUT_NTU PointPassNTU_VS(VS_INPUT_NTU In)
{
	VS_OUTPUT_NTU Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
    Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;
	//Out.Tex0		= In.TexCoord;	
	//Out.Tex0		= mul(In.TexCoord, GTexTilePan);
	Out.Tex0.x		= dot(float4(In.TexCoord.x, In.TexCoord.y, 0,0), GTexTilePan[0]);
	Out.Tex0.y		= dot(float4(In.TexCoord.x, In.TexCoord.y, 0,0), GTexTilePan[1]);
	
	float4 WorldPos	= mul(In.Pos, GObjMat);
	
	Out.WorldNormal	= mul(GObjMatIT, In.Normal);
	Out.LightVec	= GWorldLightPos - WorldPos;
	Out.EyeVec		= WorldPos - GWorldEyePos;
	
	return Out;
}

half4 PointPassNTU_PS(VS_OUTPUT_NTU In) : COLOR
{
	half3 Normal = normalize(In.WorldNormal);
	half3 EyeVec = normalize(In.EyeVec);
	
	half3 L = normalize(In.LightVec);
	
	half3 R = reflect(L, Normal);
	half S = pow(saturate(dot(R, EyeVec)), GPointLightColor.w);
	
	half NdotL = max(0,dot(L, Normal));
	
	float len = length(In.LightVec);
	float A = saturate(1 - len * GPointLight.w);
	
	half3 Color = (half3) GPointLightColor * A * NdotL;
	
	half4 Diffuse	= tex2D(ColorSampler,	 In.Tex0);
	half3 GlossMask	= tex2D(GlossMapSampler, In.Tex0);
	half3 Final		= Color * (half3) Diffuse + Color * GlossMask * 2 * S;
	
	return half4(Final, A) * Diffuse.a;		
}

// ==================================================================================================

struct VS_UBER_OUTPUT_NTU
{
    float4 Pos			: POSITION;   // vertex position 
    float  Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
    float3 WorldNormal	: TEXCOORD1;
    float3 EyeVec		: TEXCOORD2;
    float3 LightVec[4]	: TEXCOORD3;
};

VS_UBER_OUTPUT_NTU UberPointPassNTU_VS(VS_INPUT_NTU In)
{
	VS_UBER_OUTPUT_NTU Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
    //Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;
	Out.Tex0.x		= dot(float4(In.TexCoord.x, In.TexCoord.y, 0,0), GTexTilePan[0]);
	Out.Tex0.y		= dot(float4(In.TexCoord.x, In.TexCoord.y, 0,0), GTexTilePan[1]);
	//Out.Tex0		= In.TexCoord;	
	
	float3 WorldPos	= mul(In.Pos, GObjMat);
	Out.WorldNormal	= mul(GObjMatIT, In.Normal);
	Out.EyeVec		= WorldPos - GV_UberEyePos;

	for (int i=0; i<4; ++i)
		Out.LightVec[i]	= GV_UberLightPos[i] - WorldPos;
	
	//float Fog = saturate(Pos.z * GFogParams.y + GFogParams.x + GV_UberFogColor.w);
	//Out.Fog = float4((float3) GV_UberFogColor * Fog, 1);
	
	//float Fog = saturate(Pos.z * GFogParams.y + GFogParams.x + GV_UberFogHack.x);
	//Out.Fog = GV_UberFogColor * Fog;

    Out.Fog	= saturate(Pos.z * GFogParams.y + GFogParams.x + GV_UberFogHack.x);
		
	return Out;
}

half4 UberPointPassNTU_PS(VS_UBER_OUTPUT_NTU In) : COLOR
{
	half3 Normal = normalize(In.WorldNormal);
	half3 EyeVec = normalize(In.EyeVec);

	half4 Diffuse		= tex2D(ColorSampler,	 In.Tex0);
	half3 GlossMask		= tex2D(GlossMapSampler, In.Tex0);

	float4 I = 0;
	float  S = 0;

	// accumulate lights	
	if (GP_UberLightSwitch[0])
	{
		half3 L = normalize(In.LightVec[0]);
		
		float3 R = reflect(L, Normal);
		float Z = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[0].w);
		
		half NdotL = max(0,dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;
		
		float len = length(In.LightVec[0]);
		float A = saturate(1 - len * GP_UberLight[0].w);
		
		I += float4((float3) GP_UberLightColor[0] * A * NdotL, A * T);
		S += Z;
	}

	if (GP_UberLightSwitch[1])
	{
		half3 L = normalize(In.LightVec[1]);
		
		float3 R = reflect(L, Normal);
		float Z = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[1].w);
		
		half NdotL = max(0,dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;
		
		float len = length(In.LightVec[1]);
		float A = saturate(1 - len * GP_UberLight[1].w);
		
		I += float4((float3) GP_UberLightColor[1] * A * NdotL, A * T);
		S += Z;
	}
	
	if (GP_UberLightSwitch[2])
	{
		half3 L = normalize(In.LightVec[2]);
		
		float3 R = reflect(L, Normal);
		float Z = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[2].w);
		
		half NdotL = max(0,dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;
		
		float len = length(In.LightVec[2]);
		float A = saturate(1 - len * GP_UberLight[2].w);
		
		I += float4((float3) GP_UberLightColor[2] * A * NdotL, A * T);
		S += Z;
	}
	
	if (GP_UberLightSwitch[3])
	{
		half3 L = normalize(In.LightVec[3]);
		
		float3 R = reflect(L, Normal);
		float Z = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[3].w);
		
		half NdotL = max(0,dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;
		
		float len = length(In.LightVec[3]);
		float A = saturate(1 - len * GP_UberLight[3].w);
		
		I += float4((float3) GP_UberLightColor[3] * A * NdotL, A * T);
		S += Z;
	}
	
	half3 Final = (half3) Diffuse * (half3) I + GlossMask * 2 * S * (half3) I;
	
	//return half4(Final, I.a) * Diffuse.a;
	
	half4 F = half4(Final, I.a) * Diffuse.a;
	
	return F * In.Fog;
}


// =======================================================================================================

struct VS_UBER_OUTPUT_TU2
{
    float4 Pos			: POSITION;   // vertex position 
    float  Fog			: FOG;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
    float3 WorldNormal	: TEXCOORD2;
    float3 EyeVec		: TEXCOORD3;
    float3 LightVec[4]	: TEXCOORD4;
};


VS_UBER_OUTPUT_TU2 UberPointPassTU2_VS(VS_INPUT_T2L In)
{
	VS_UBER_OUTPUT_TU2 Out;
	
	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Pos			= Pos;
	Out.Tex0.x		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[0]);
	Out.Tex0.y		= dot(float4(In.Tex0.x, In.Tex0.y, 0,0), GTexTilePan[1]);	
	Out.Tex1		= In.Tex1;		
	Out.WorldNormal	= In.Normal;
	Out.EyeVec		= In.Pos - GV_UberEyePos;
		
	for (int i=0; i<4; ++i)
		Out.LightVec[i]	= GV_UberLightPos[i] - In.Pos;

    Out.Fog	= saturate(Pos.z * GFogParams.y + GFogParams.x + GV_UberFogHack.x);

	return Out;
}

half4 UberPointPassTU2_PS(VS_UBER_OUTPUT_TU2 In) : COLOR
{
	half3 Normal = normalize(In.WorldNormal);	
	half3 EyeVec = normalize(In.EyeVec);

	half4  Diffuse		= tex2D(ColorSampler, In.Tex0);
	half3  GlossMask	= tex2D(GlossMapSampler, In.Tex0);
	half3  LMap			= tex2D(LightMapSampler, In.Tex1);
				
	float4 DiffuseColor = 0;		// sum atten in A
	float3 SpecularColor = 0;
					
	// accumulate lights	
	if (GP_UberLightSwitch[0])
	{
		half3 L = normalize(In.LightVec[0]);
		
		float3 R = reflect(L, Normal);
		float  S = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[0].w);
		
		half NdotL = saturate(dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;

		float len = length(In.LightVec[0]);
		float A = saturate(1 - len * GP_UberLight[0].w);

		float3 I = GP_UberLightColor[0] * NdotL * A;
		
		DiffuseColor	+= float4(GP_UberLight[0].x * I, A * T);
		SpecularColor	+= float3(saturate(LMap + GP_UberLight[0].y) * I * S * GP_UberLight[0].z);		
	}
	
	if (GP_UberLightSwitch[1])
	{
		half3 L = normalize(In.LightVec[1]);
		
		float3 R = reflect(L, Normal);
		float  S = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[1].w);
		
		half NdotL = saturate(dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;

		float len = length(In.LightVec[1]);
		float A = saturate(1 - len * GP_UberLight[1].w);

		float3 I = GP_UberLightColor[1] * NdotL * A;
		
		DiffuseColor	+= float4(GP_UberLight[1].x * I, A * T);
		SpecularColor	+= float3(saturate(LMap + GP_UberLight[1].y) * I * S * GP_UberLight[1].z);		
	}
	
	if (GP_UberLightSwitch[2])
	{
		half3 L = normalize(In.LightVec[2]);
		
		float3 R = reflect(L, Normal);
		float  S = pow(saturate(dot(R, EyeVec)), GP_UberLightColor[2].w);
		
		half NdotL = saturate(dot(L, Normal));
		half T = NdotL > 0 ? 1 : 0;

		float len = length(In.LightVec[2]);
		float A = saturate(1 - len * GP_UberLight[2].w);

		float3 I = GP_UberLightColor[2] * NdotL * A;
		
		DiffuseColor	+= float4(GP_UberLight[2].x * I, A * T);
		SpecularColor	+= float3(saturate(LMap + GP_UberLight[2].y) * I * S * GP_UberLight[2].z);		
	}

	half3 Final = (half3) DiffuseColor * (half3) Diffuse + SpecularColor * GlossMask;
	
	half4 F = half4(Final, DiffuseColor.a) * Diffuse.a;
	
	return F * In.Fog;	
}

// ==========================================================================================================

technique FXPointPassNTU
{
	pass P0
	{
		VertexShader	= compile vs_1_1 PointPassNTU_VS();
		PixelShader		= compile ps_2_0 PointPassNTU_PS();
	}
}

technique FXPointPassTU2Gloss
{
	pass P0
	{
		VertexShader	= compile vs_1_1 PointPassTU2_VS();
		PixelShader		= compile ps_2_0 PointPassTU2_PS();
	}
}

technique FXTU2Gloss
{
	pass P0
	{
		VertexShader	= compile vs_1_1 SpecularTU2_VS();
		PixelShader		= compile ps_2_0 SpecularTU2_PS();
	}
}

technique FXTU2GlossBlend
{
	pass P0
	{
		VertexShader	= compile vs_1_1 SpecularTU2Blend_VS();
		PixelShader		= compile ps_2_0 SpecularTU2Blend_PS();
	}
}

technique FXUberPointPassNTU
{
	pass P0
	{
		VertexShader	= compile vs_3_0 UberPointPassNTU_VS();
		PixelShader		= compile ps_3_0 UberPointPassNTU_PS();
	}
}

technique FXUberPointPassTU2
{
	pass P0
	{
		VertexShader	= compile vs_3_0 UberPointPassTU2_VS();
		PixelShader		= compile ps_3_0 UberPointPassTU2_PS();
	}
}
