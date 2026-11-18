// globals
float4x4	GClipMat	: register(c0);
float4x4	GObjMat		: register(c4);
float4		GMagicConst	: register(c8);
float4		GFogParams	: register(c9);
float4		GEye		: register(c10);
float4x4	GViewMat	: register(c40);
float4x4	GViewMatNeg	: register(c44);
float4		GAspect		: register(c45);

float4		GEyePos		: register(c14);

// pixel shader
float4		GFresnel	: register(c0);
float4		GShallowCol	: register(c1);
float4		GDeepCol	: register(c2);
float4		GDirLight	: register(c3);

sampler Sampler0		: register(s0);
sampler Sampler1		: register(s1);
sampler Sampler2		: register(s2);
sampler Sampler3		: register(s3);
sampler Sampler4		: register(s4);

struct Wave
{
	float	freq;  // 2*PI / wavelength
	float	amp;   // amplitude
	float	phase; // speed * 2*PI / wavelength
	float	time;
};

float4	GWaveParams[2]	: register(c50);
float4	GWaveDir[2]		: register(c52);


//water
float4x3	GBumpSpace	: register(c4);
float4x2	GTexForm0	: register(c24);
float4x2	GTexForm1	: register(c27);

float4		GRefParams	: register(c11);	//(1,1,reflectionAmount,FresnelExponent)
float4		GFresBias	: register(c12);	//(1-fresnelBias, fresnelBias, 1, 1)
float4x2	GDirs		: register(c13);
float4		GPhase		: register(c15);
float4		GFreq		: register(c16);
float4		GAmpli		: register(c17);
float4		GRefrTint	: register(c22);
float4		GReflTint	: register(c21);

// pixel shader
float4		GRefScales	: register(c1);

// water samplers

sampler NmapSampler0	: register(s0);
sampler NmapSampler1	: register(s1);
sampler CubeSampler		: register(s2);
sampler LmapSampler		: register(s3);

sampler ReflSampler		: register(s4);
sampler RefrSampler		: register(s5);

// ==============================================================================================

float EvaluateWave(float4 w, float2 dir, float2 pos)
{
	return w.y * sin( dot(dir, pos)* w.x + w.w * w.z);
}

float EvaluateWaveDeriv(float4 w, float2 dir, float2 pos)
{
	return w.x * w.y * cos( dot(dir, pos)*w.x + w.w * w.z);
}

// ==============================================================================================

void WaterVS(in		float4 Pos			: POSITION,
		     in		float2 InTex		: TEXCOORD0,
		     out	float4 OutPos		: POSITION,
		     out	float  Fog			: FOG,
		     out	float4 OutTex1		: TEXCOORD0,
		     out	float4 OutTex2		: TEXCOORD1,
		     out	float2 OutTex3		: TEXCOORD2,
		     out	float3 OutEye		: TEXCOORD3,
		     out	float3 OutTan[3]	: TEXCOORD4)
{
	/*
    float4 P = Pos;

	// sum waves	
	//P.y = 0.0;
	float ddx = 0.0, ddy = 0.0;
	for (int i=0; i<NWAVES; i++) 
	{
    	P.y += EvaluateWave(wave[i], P.xz, GWaveParams.w);
    	float deriv = EvaluateWaveDeriv(wave[i], P.xz, GWaveParams.w);
    	ddx += deriv * wave[i].dir.x;
    	ddy += deriv * wave[i].dir.y;
    }
    */
    
    float4 P = Pos;
    
    //P.y += EvaluateWave(GWaveParams[0], GWaveDir[0], P.xz);
    //P.y += EvaluateWave(GWaveParams[1], GWaveDir[1], P.xz);

    float ddx = 0.0, ddy = 0.0;

	for (int i=0; i<2; ++i)
	{
		P.y += EvaluateWave(GWaveParams[i], GWaveDir[i], P.xz);
    	float deriv = EvaluateWaveDeriv(GWaveParams[i], GWaveDir[i], P.xz);
    	ddx += deriv * GWaveDir[i].x;
    	ddy += deriv * GWaveDir[i].y;
    }
    
	// compute tangent basis
    float3 B = float3(1, ddx, 0);
    float3 T = float3(0, ddy, 1);
    float3 N = float3(-ddx, 1, -ddy);
    
    float BumpScale = 2;
	OutTan[0] = BumpScale * normalize(T);
	OutTan[1] = BumpScale * normalize(B);
	OutTan[2] = normalize(N);
    
    //P.y += EvaluateWave(wave[0], P.xz, GWaveParams.w);
    //P.y += sin(1 + GWaveParams.w *0.5);

	OutPos		= mul(P, GClipMat);
	OutTex1		= mul(P, GViewMat);
	OutTex2		= mul(P, GViewMatNeg);
	OutTex3		= InTex;
	
	OutEye		= P - GEyePos;
	
	Fog = 1;
}
			  
float4 WaterPS(in float4 Tex1	: TEXCOORD0,
			   in float4 Tex2	: TEXCOORD1,
			   in float2 Tex3	: TEXCOORD2,
			   in float3 Eye	: TEXCOORD3,
			   in float3 Tan[3]	: TEXCOORD4) : COLOR
{
	float3 E = normalize(Eye);
	
	//float4 W = tex2D(Sampler2, Tex3*2);
	float4 B = tex2D(Sampler3, Tex3*4)*2 - 1;
	
	float3x3 m; // tangent to world matrix
	m[0] = Tan[1];
	m[1] = Tan[0];
	m[2] = Tan[2];
	float3 N = mul(m, B.xyz);	
	//float3 E = normalize(mul(m, Eye));
	//float3 L = mul(m, (float3) GDirLight);

	
	float4 Refract = tex2Dproj(Sampler1, Tex2 + 0.05F * float4(N.x,N.z,0,0));
	float4 Reflect = tex2Dproj(Sampler0, Tex1 + 0.05F * float4(N.x,N.y,0,0));

	// fresnel	
    float Facing = 1.0 - max(dot(-E, N), 0);
    float Fresnel = GFresnel.x + (1.0-GFresnel.x) * pow(Facing, GFresnel.y);

    //float4 WaterColor = lerp(GDeepCol, GShallowCol, Facing);
	
	//return WaterColor * Refract + Fresnel * Reflect;
	return (1-Fresnel)*Refract + Fresnel * Reflect;
	
	/*
	float3 X = (1-Fresnel)*N;
	float4 Refract = tex2Dproj(Sampler1, Tex2 + 0.1F * float4(N.x,N.y,0,0));
	//return WaterColor * Refract;

	return Reflect;	
	*/

	/*
	float4 Refract = tex2Dproj(Sampler1, Tex2 + 0.05F * float4(N.x,N.y,0,0));
	float4 Reflect = tex2Dproj(Sampler0, Tex1 + 0.05F * float4(N.x,N.y,0,0));
	
	float Facing = 1 - max(dot(-E, N), 0);
	float F = pow(Facing, GFresnel.y);

    float4 WaterColor = lerp(GDeepCol, GShallowCol, F);
    
    return WaterColor * Refract;
    */
}

// ==============================================================================================

technique water_fx
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 WaterVS();
        PixelShader  = compile ps_2_0 WaterPS();        
    }
}

// ================================================================================================

struct VS_INPUT
{
	float4 Pos			: POSITION;
    float4 Tex0			: TEXCOORD0;
    float4 Tex1			: TEXCOORD1;
};

struct PS_INPUT
{
    float4 Pos			: POSITION;   
    float1 Fog			: FOG;
    float2 LightMap		: TEXCOORD0;
    float2 NormalMap0	: TEXCOORD1;
    float2 NormalMap1	: TEXCOORD2;
    float3 Eye			: TEXCOORD3;
    float3 BumpScale	: TEXCOORD4;
    float4 Params		: TexCOORD5;
    float3 RefrTint		: COLOR0;
    float3 ReflTint		: COLOR1;
};

// ================================================================================================

PS_INPUT Water_VS11(VS_INPUT In)
{
	PS_INPUT Out;	
	
	Out.BumpScale	= float3(GBumpSpace[0].x-1, GBumpSpace[1].y-1, GBumpSpace[2].z-1);
	
	float2 Wave		= mul(In.Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	In.Pos.y		+= Ampli;
	
	float4 PosH		= mul(In.Pos, GClipMat);		
    Out.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	Out.Pos			= PosH;
	Out.NormalMap0	= mul(In.Tex0,GTexForm0);
	Out.NormalMap1	= mul(In.Tex0,GTexForm1);
	Out.LightMap	= In.Tex1;
	Out.Eye			= GEye-In.Pos;
	Out.RefrTint	= GRefrTint;
	Out.ReflTint	= GReflTint+GRefrTint;
	Out.Params.x	= GRefParams.z;	// reflectionAmount
	Out.Params.y	= GRefParams.w; // FresnelExponent
	Out.Params.z	= GFresBias.x;	// 1 - fresnel bias
	Out.Params.w	= GFresBias.y;	// fresnel bias

	return Out;
}

half4 Water_PS20(PS_INPUT In) : COLOR
{
	float3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	float3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	float3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	Normal *= In.BumpScale;
	
	float3 Eye = -In.Eye + 2 * Normal * dot(Normal,In.Eye);
	
	half3 CubeMap = texCUBE(CubeSampler, Eye);
	
	float Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	float3 Refl = CubeMap*In.Params.x + In.ReflTint*(1-In.Params.x);
	
	float3 Color = (1-Fresnel)*In.RefrTint + Fresnel*Refl;
	
	//return half4(Color, 1);
	
	return half4(Color*LMap*2,1);	
}

// ================================================================================================

technique FXWater_20
{
	pass P0
	{
		PixelShader		= compile ps_2_0 Water_PS20();
		VertexShader	= compile vs_2_0 Water_VS11();
	}
}

// ================================================================================================

struct VS_INPUT2
{
	float4 Pos			: POSITION;
    float4 Tex0			: TEXCOORD0;
    float4 Tex1			: TEXCOORD1;
};

struct PS_INPUT2
{
    float4 Pos			: POSITION;   
    float1 Fog			: FOG;
    float2 LightMap		: TEXCOORD0;
    float2 NormalMap0	: TEXCOORD1;
    float2 NormalMap1	: TEXCOORD2;
    float3 Eye			: TEXCOORD3;
    float3 BumpScale	: TEXCOORD4;
    float4 Params		: TEXCOORD5;
    float4 ReflProj		: TEXCOORD6;
    float4 RefrProj		: TEXCOORD7;
    float3 RefrTint		: COLOR0;
    float3 ReflTint		: COLOR1;
};

// ================================================================================================

PS_INPUT2 Water2_VS11(VS_INPUT2 In)
{
	PS_INPUT2 Out;	
	
	Out.BumpScale	= float3(GBumpSpace[0].x-1, GBumpSpace[1].y-1, GBumpSpace[2].z-1);
	
	float2 Wave		= mul(In.Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	In.Pos.y		+= Ampli;
	
	float4 PosH		= mul(In.Pos, GClipMat);		
    Out.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	Out.Pos			= PosH;
	Out.NormalMap0	= mul(In.Tex0,GTexForm0);
	Out.NormalMap1	= mul(In.Tex0,GTexForm1);
	Out.LightMap	= In.Tex1;
	Out.Eye			= GEye-In.Pos;
	Out.RefrTint	= GRefrTint;
	Out.ReflTint	= GReflTint+GRefrTint;
	Out.Params.x	= GRefParams.z;	// reflectionAmount
	Out.Params.y	= GRefParams.w; // FresnelExponent
	Out.Params.z	= GFresBias.x;	// 1 - fresnel bias
	Out.Params.w	= GFresBias.y;	// fresnel bias

	//Out.ReflProj	= mul(In.Pos, GViewMat);
	//Out.RefrProj	= mul(In.Pos, GViewMatNeg);

	float4 T;
	T.xy = (PosH.xy + PosH.w) * 0.5;
	T.z = 0;
	T.w = PosH.w;
	
	Out.ReflProj = T;
	
	T.xy = (float2(PosH.x,-PosH.y) + PosH.w) * 0.5;
	Out.RefrProj = T;

	return Out;
}

half4 Water2_Refl_PS20(PS_INPUT2 In) : COLOR
{	
	half3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	half4 D = half4(Normal.x, Normal.y, 0, 0) * GRefScales.x;
	half4 Reflect = tex2Dproj(ReflSampler, In.ReflProj + D);
	
	Normal *= In.BumpScale;

	//half4 Reflect = tex2Dproj(ReflSampler, In.ReflProj + half4(Normal.x,Normal.y,0,0));
	
	float3 Eye = -In.Eye + 2 * Normal * dot(Normal,In.Eye);
	
	half3 CubeMap = texCUBE(CubeSampler, Eye);
	
	float Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 R = /*CubeMap */ Reflect;
	half3 Refl = R * In.Params.x + In.ReflTint*(1-In.Params.x);
	
	half3 Color = (1-Fresnel)*In.RefrTint + Fresnel*Refl;
		
	//return half4(Color*LMap*2,1);	
	return half4(Color,1);

	/*
	half3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	half4 D = half4(Normal.x, Normal.y, 0, 0) * 0.25;
	
	return tex2Dproj(ReflSampler, In.ReflProj + D);
	*/
}

half4 Water2_Refr_PS20(PS_INPUT2 In) : COLOR
{
	half3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	half4 D = half4(Normal.z, Normal.x, 0, 0) * GRefScales.y;
	
	half4 Refract = tex2Dproj(Sampler4, In.RefrProj + D);
	
	Normal *= In.BumpScale;

	//half4 Refract = tex2Dproj(Sampler4, In.RefrProj + half4(Normal.x,Normal.y,0,0));
	
	float3 Eye = -In.Eye + 2 * Normal * dot(Normal,In.Eye);
	
	half3 CubeMap = texCUBE(CubeSampler, Eye);
	
	float Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 Refl = CubeMap*In.Params.x + In.ReflTint*(1-In.Params.x);
	//half3 Refl = In.ReflTint;
	
	half3 Color = (1-Fresnel)*Refract + Fresnel*Refl;
		
	//return half4(Color*LMap*2,1);	
	
	return half4(Color,1);

	/*
	half3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	half4 D = half4(Normal.x, Normal.y, 0, 0) * 0.1;
	
	return tex2Dproj(Sampler4, In.RefrProj + D);
	*/
}

half4 Water2_Refl_Refr_PS20(PS_INPUT2 In) : COLOR
{
	half3 LMap  = tex2D(LmapSampler,  In.LightMap);	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;

	//half4 D = half4(Normal.x, Normal.y, 0, 0) * 0.25;
	//half4 Reflect = tex2Dproj(ReflSampler, In.ReflProj + D);
	//half4 Refract = tex2Dproj(RefrSampler, In.RefrProj - D);

	half4 Reflect = tex2Dproj(ReflSampler, In.ReflProj + half4(Normal.x, Normal.y, 0, 0) * GRefScales.x);
	half4 Refract = tex2Dproj(RefrSampler, In.RefrProj - half4(Normal.x, Normal.y, 0, 0) * GRefScales.y);
	
	Normal *= In.BumpScale;
	
	float3 Eye = -In.Eye + 2 * Normal * dot(Normal,In.Eye);
	
	half3 CubeMap = texCUBE(CubeSampler, Eye);
	
	float Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;

	half3 R = CubeMap * Reflect;
	half3 Refl = R * In.Params.x + In.ReflTint*(1-In.Params.x);
		
	half3 Color = (1-Fresnel)*Refract + Fresnel*Refl;
		
	return half4(Color*LMap*2,1);	
}

// ================================================================================================

technique FXWater2_Refl
{
	pass P0
	{
		PixelShader		= compile ps_2_0 Water2_Refl_PS20();
		VertexShader	= compile vs_2_0 Water2_VS11();
	}
}

technique FXWater2_Refr
{
	pass P0
	{
		PixelShader		= compile ps_2_0 Water2_Refr_PS20();
		VertexShader	= compile vs_2_0 Water2_VS11();
	}
}

technique FXWater2_Refl_Refr
{
	pass P0
	{
		PixelShader		= compile ps_2_0 Water2_Refl_Refr_PS20();
		VertexShader	= compile vs_2_0 Water2_VS11();
	}
}


// ================================================================================================

struct VS_INPUT_NTU
{
	float4 Pos			: POSITION;
    float4 TexCoord		: TEXCOORD0;
};

struct PS_INPUT_NTU
{
    float4 Pos			: POSITION;   
    float1 Fog			: FOG;
    float2 NormalMap0	: TEXCOORD0;
    float2 NormalMap1	: TEXCOORD1;
    half3  Eye			: TEXCOORD2;
    float3 BumpScale	: TEXCOORD3;
    float4 Params		: TEXCOORD4;
    float4 ReflProj		: TEXCOORD5;
    float4 RefrProj		: TEXCOORD6;
    float3 RefrTint		: COLOR0;
    float3 ReflTint		: COLOR1;
};

// ================================================================================================

PS_INPUT_NTU WaterNTU_VS(VS_INPUT_NTU In)
{
	PS_INPUT_NTU Out;	
	
	Out.BumpScale	= float3(GBumpSpace[0].x-1, GBumpSpace[1].y-1, GBumpSpace[2].z-1);
	
	float2 Wave		= mul(In.Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	In.Pos.y		+= Ampli;
	
	float4 PosH		= mul(In.Pos, GClipMat);		
    Out.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	Out.Pos			= PosH;
	Out.NormalMap0	= mul(In.TexCoord,GTexForm0);
	Out.NormalMap1	= mul(In.TexCoord,GTexForm1);
	Out.Eye			= GEye-In.Pos;
	Out.RefrTint	= GRefrTint;
	Out.ReflTint	= GReflTint+GRefrTint;
	Out.Params.x	= GRefParams.z;	// reflectionAmount
	Out.Params.y	= GRefParams.w; // FresnelExponent
	Out.Params.z	= GFresBias.x;	// 1 - fresnel bias
	Out.Params.w	= GFresBias.y;	// fresnel bias
	
	float4 T;
	//T.xy = (PosH.xy + PosH.w) * 0.5;
	T.xy = (PosH.xy + PosH.w) * float2(GAspect.x,GAspect.y);
	T.z = 0;
	T.w = PosH.w;
	
	Out.ReflProj = T;
	
	//T.xy = (float2(PosH.x,-PosH.y) + PosH.w) * 0.5;
	T.xy = (float2(PosH.x,-PosH.y) + PosH.w) * float2(GAspect.x,GAspect.y);
	Out.RefrProj = T;
	
	return Out;
}

half4 WaterNTU_Refl_PS(PS_INPUT_NTU In) : COLOR
{	
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	half4 Reflect = tex2Dproj(Sampler2, In.ReflProj + half4(Normal.x, Normal.z, 0, 0) * (half) GRefScales.x);
	
	Normal *= In.BumpScale;
		
	half Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 Refl = Reflect * In.Params.x + In.ReflTint*(1-In.Params.x);

	half F = 4 * (Fresnel - 0.75);
	half3 R = saturate(Reflect * F);
	
	half3 Color = (1-Fresnel)*In.RefrTint + Fresnel*(Refl+R)*In.ReflTint;
		
	return half4(Color,1);
}

half4 WaterNTU_RR_PS(PS_INPUT_NTU In) : COLOR
{	
	/*
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	half4 Reflect = tex2Dproj(Sampler2, In.ReflProj + half4(Normal.x, Normal.y, 0, 0) * GRefScales.x);
	half4 Refract = tex2Dproj(Sampler3, In.RefrProj - half4(Normal.x, Normal.y, 0, 0) * GRefScales.y);
	
	Normal *= In.BumpScale;
		
	float Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 Refl = Reflect * In.Params.x + In.ReflTint*(1-In.Params.x);
	
	half3 Color = (1-Fresnel)*Refract + Fresnel*Refl;
		
	return half4(Color,1);
	*/

	/*
	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	half4 Reflect = tex2Dproj(Sampler2, In.ReflProj + half4(Normal.x, Normal.y, 0, 0) * (half) GRefScales.x);
	half4 Refract = tex2Dproj(Sampler3, In.RefrProj - half4(Normal.x, Normal.y, 0, 0) * (half) GRefScales.y);
	
	Normal *= In.BumpScale;
		
	half Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 Refl = Reflect * In.Params.x + In.ReflTint*(1-In.Params.x);
	
	half3 Color = (1-Fresnel)*Refract*In.RefrTint + Fresnel*Refl*In.ReflTint;
		
	return half4(Color,1);
	*/


	half3 NMap0 = tex2D(NmapSampler0, In.NormalMap0);	
	half3 NMap1 = tex2D(NmapSampler1, In.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	half4 Reflect = tex2Dproj(Sampler2, In.ReflProj + half4(Normal.x, Normal.z, 0, 0) * (half) GRefScales.x);
	half4 Refract = tex2Dproj(Sampler3, In.RefrProj + half4(Normal.x, Normal.z, 0, 0) * (half) GRefScales.y);
	
	Normal *= In.BumpScale;
		
	half Fresnel = pow((1 - dot(Normal,normalize(In.Eye))), In.Params.y);
	Fresnel = In.Params.z * Fresnel + In.Params.w;
	
	half3 Refl = Reflect * In.Params.x + In.ReflTint*(1-In.Params.x);
	
	half F = 4 * (Fresnel - 0.75);
	half3 R = saturate(Reflect * F);
	
	half3 Color = (1-Fresnel)*Refract*In.RefrTint + Fresnel*(Refl+R)*In.ReflTint;
		
	return half4(Color,1);
}

technique FXWaterNTU_Refl
{
	pass P0
	{
		PixelShader		= compile ps_2_0 WaterNTU_Refl_PS();
		VertexShader	= compile vs_2_0 WaterNTU_VS();
	}
}

technique FXWaterNTU_RR
{
	pass P0
	{
		PixelShader		= compile ps_2_0 WaterNTU_RR_PS();
		VertexShader	= compile vs_2_0 WaterNTU_VS();
	}
}

// ================================================================================================


struct SimpleWaterNTU_INPUT
{
	float4 Pos			: POSITION;
    float4 TexCoord		: TEXCOORD0;
};

struct SimpleWaterNTU_OUTPUT
{
    float4 Pos			: POSITION;   
    float1 Fog			: FOG;
    float2 NormalMap0	: TEXCOORD0;
    float2 NormalMap1	: TEXCOORD1;
    half3  Eye			: TEXCOORD2;
    float3 BumpScale	: TEXCOORD3;
    float4 Params		: TEXCOORD4;
    float3 RefrTint		: COLOR0;
    float3 ReflTint		: COLOR1;
};

SimpleWaterNTU_OUTPUT SimpleWaterNTU_VS(SimpleWaterNTU_INPUT IN)
{
	SimpleWaterNTU_OUTPUT OUT;	
	
	OUT.BumpScale	= float3(GBumpSpace[0].x-1, GBumpSpace[1].y-1, GBumpSpace[2].z-1);
	
	float2 Wave		= mul(IN.Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	IN.Pos.y		+= Ampli;
	
	float4 PosH		= mul(IN.Pos, GClipMat);		
    OUT.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	OUT.Pos			= PosH;
	OUT.NormalMap0	= mul(IN.TexCoord,GTexForm0);
	OUT.NormalMap1	= mul(IN.TexCoord,GTexForm1);
	OUT.Eye			= GEye-IN.Pos;
	OUT.RefrTint	= GRefrTint;
	OUT.ReflTint	= GReflTint+GRefrTint;
	OUT.Params.x	= GRefParams.z;	// reflectionAmount
	OUT.Params.y	= GRefParams.w; // FresnelExponent
	OUT.Params.z	= GFresBias.x;	// 1 - fresnel bias
	OUT.Params.w	= GFresBias.y;	// fresnel bias
		
	return OUT;
}

half4 SimpleWaterNTU_PS(SimpleWaterNTU_OUTPUT IN) : COLOR
{
	float3 NMap0 = tex2D(NmapSampler0, IN.NormalMap0);	
	float3 NMap1 = tex2D(NmapSampler1, IN.NormalMap1);	
	
	half3 Normal = (NMap0+NMap1)-1;
	
	Normal *= IN.BumpScale;
	
	float3 Eye = -IN.Eye + 2 * Normal * dot(Normal,IN.Eye);
	
	half3 CubeMap = texCUBE(CubeSampler, Eye);
	
	half Fresnel = pow((1 - dot(Normal,normalize(IN.Eye))), IN.Params.y);
	Fresnel = IN.Params.z * Fresnel + IN.Params.w;
	
	half3 Refl = CubeMap*IN.Params.x + IN.ReflTint*(1-IN.Params.x);
	
	half3 Color = (1-Fresnel)*IN.RefrTint + Fresnel*Refl;
		
	return half4(Color,1);	
}

technique SimpleWaterNTU
{
	pass P0
	{
		PixelShader		= compile ps_2_0 SimpleWaterNTU_PS();
		VertexShader	= compile vs_2_0 SimpleWaterNTU_VS();
	}
}

// ================================================================================================

void Reflect11_VS(in	float4 Pos		: POSITION,
				  in	float4 TexCoord	: TEXCOORD0,
				  out	float4 OutPos	: POSITION,
				  out	float4 Color	: COLOR0,
				  out	float  OutFog	: FOG,
				  out	float2 OutTC0	: TEXCOORD0,
				  out	float4 OutProj	: TEXCOORD1,
				  out	float3 OutEye	: TEXCOORD2,
				  out	float2 OutTC3	: TEXCOORD3)
{	
	float2 Wave		= mul(Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	Pos.y			+= Ampli;

	OutPos	= mul(Pos, GClipMat);
    OutFog	= OutPos.z * GFogParams.y + GFogParams.x;
	OutTC0	= mul(TexCoord,GTexForm0);
	OutTC3	= mul(TexCoord,GTexForm1);
	OutEye	= GEye - Pos;
	
	float4 T;
	//T.xy = (OutPos.xy + OutPos.w) * 0.5;
	T.xy = (OutPos.xy + OutPos.w) * float2(GAspect.x,GAspect.y);
	T.z = 0;
	T.w = OutPos.w;
	
	OutProj = T;	
	
	Color = GReflTint+GRefrTint;
}

void Refract11_VS(in	float4 Pos		: POSITION,
				  in	float4 TexCoord	: TEXCOORD0,
				  out	float4 OutPos	: POSITION,
				  out	float4 Color	: COLOR0,
				  out	float  OutFog	: FOG,
				  out	float2 OutTC0	: TEXCOORD0,
				  out	float4 OutProj	: TEXCOORD1)				  
{
	float2 Wave		= mul(Pos,GDirs);
	Wave			= (float4(Wave.x,Wave.y,0,0) * GFreq) - GPhase;
	
	float Ampli		= sin(Wave.x)*GAmpli.x + sin(Wave.y)*GAmpli.y;
	
	Pos.y			+= Ampli;

	OutPos	= mul(Pos, GClipMat);
    OutFog	= OutPos.z * GFogParams.y + GFogParams.x;
	OutTC0	= mul(TexCoord,GTexForm0);
	
	float4 T;
	//T.xy = (float2(OutPos.x,-OutPos.y) + OutPos.w) * 0.5;
	T.xy = (float2(OutPos.x,-OutPos.y) + OutPos.w) * float2(GAspect.x,GAspect.y);
	T.z = 0;
	T.w = OutPos.w;
	
	OutProj = T;

	Color = GRefrTint;
}

technique FXReflectNTU_11
{
	pass P0
	{
		VertexShader	= compile vs_1_1 Reflect11_VS();
		
		PixelShader = asm
		{
			ps.1.1
			def c0, 1.0, 0.2, 0.3, 0.7
			//def c1, 4,4, 4, 1
			tex t0
			texbem t1, t0
			tex t2
			tex t3
			
			//mul r0, t3_bx2, c1
			//dp3_sat	r1.rgba, t2_bx2, r0
			dp3_sat	r1.rgba, t2_bx2, t3_bx2		
			
			mul r0.a, 1-r1.a, 1-r1.a			// squared
			mul r0.a, r0.a, r0.a				// quartic
			//mov r0.rgb, t1
			mul r0.rgb, t1, v0
			+mul_sat r0.a, r0.a, 1-r1.a			
			
			//mov r0.a, c1.a
			//add_sat r0.a, r0.a, c0.a
			mad_sat r0.a, r0.a, c0.z, c0.w
			//mad_sat r0.a, r0.a, c4.z, c4.w
		};
	}
}

technique FXRefractNTU_11
{
	pass P0
	{
		VertexShader	= compile vs_1_1 Refract11_VS();
		
		PixelShader = asm
		{
			ps.1.1
			def c0, 1, 1, 1, 1
			tex t0
			texbem t1, t0

			//mov r0.rgb, t1
			mul r0.rgb,t1,v0
			+mov r0.a, c0.a
		};
	}
}

// ================================================================================================
