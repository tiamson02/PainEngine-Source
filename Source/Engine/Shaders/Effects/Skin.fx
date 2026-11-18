// globals
float4x4	GClipMat		: register(c0);
float4x4	GObjMat			: register(c4);
float4x4	GObjMatIT		: register(c16);

float4x4	GViewMat		: register(c4);

float4		GFogParams		: register(c9);

//skin
float4		GMagicConst		: register(c8);		// (24.5, 255*3, 1, 0.5)
float3		GAmbient		: register(c11);
float4		GMaterial		: register(c10);

float3		GLightCol0		: register(c12);
float3		GLightDir0		: register(c13);
float3		GHalfDir0		: register(c14);

float3		GLightCol1		: register(c15);
float3		GLightDir1		: register(c16);
float3		GHalfDir1		: register(c17);

float3		GLightCol2		: register(c18);
float3		GLightDir2		: register(c19);
float3		GHalfDir2		: register(c20);

float3		GLightCol3		: register(c21);
float3		GLightDir3		: register(c22);
float3		GHalfDir3		: register(c23);

float4x3	GSkinBones[23]	: register(c27);

// pixel shader
float4		GAmbientColor	: register(c0);
float4		GLightColor0	: register(c1);
float4		GLightColor1	: register(c2);

bool 		GLightEnable0 	: register(b0);
bool 		GLightEnable1 	: register(b1);

sampler	ColorSampler		: register (s0);
sampler NormalSampler		: register (s1);

sampler Sampler0			: register (s0);
sampler Sampler1			: register (s1);
sampler Sampler2			: register (s2);

// ================================================================================================

struct VS_INPUT
{
	float4 Pos			: POSITION;
    float2 Tex0			: TEXCOORD0;
    float3 Weights		: BLENDWEIGHT;
    float4 Indices		: BLENDINDICES;
};

struct PS_INPUT
{
    float4 Pos			: POSITION;   
    float1 Fog			: FOG;
    float2 DiffuseMap	: TEXCOORD0;
    float2 NormalMap	: TEXCOORD1;
    float3 LightDir0	: TEXCOORD2;
    float3 HalfDir0		: TEXCOORD3;
    float3 LightDir1	: TEXCOORD4;
    float3 HalfDir1		: TEXCOORD5;
};

// ================================================================================================

PS_INPUT SkinBump_VS11(VS_INPUT In)
{
	PS_INPUT Out;
	
	int4 bones = D3DCOLORtoUBYTE4(In.Indices);
	int bone0 = bones.x;
	int bone1 = bones.y;
	
	float3 Pos		= mul(In.Pos, GSkinBones[bone0]) * In.Weights.x
				    + mul(In.Pos, GSkinBones[bone1]) * (1-In.Weights.x);
	
	float4 PosH		= mul(float4(Pos, 1), GClipMat);
		
	Out.LightDir0	= mul(GSkinBones[bone0], GLightDir0);
	Out.HalfDir0	= mul(GSkinBones[bone0], GHalfDir0);
	Out.LightDir1	= mul(GSkinBones[bone0], GLightDir1);
	Out.HalfDir1	= mul(GSkinBones[bone0], GHalfDir1);
	
    Out.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	Out.Pos			= PosH;
	Out.DiffuseMap	= In.Tex0;
	Out.NormalMap	= In.Tex0;

	return Out;
}

half4 SkinBump_PS20(PS_INPUT In) : COLOR
{
	half4 Diff = tex2D(ColorSampler, In.DiffuseMap).rgba;	
	half3 Norm = tex2D(NormalSampler, In.NormalMap).rgb * 2 - 1;
	half  Spec = tex2D(NormalSampler, In.NormalMap).a;
	
	half3 Lght1 = normalize(In.LightDir0);
	half3 Half1 = normalize(In.HalfDir0);
	
	half3 NdotL1 = saturate(dot(Norm,Lght1)) * GLightColor0;
	half3 NdotH1 = pow(saturate(dot(Norm,Half1)),10) * NdotL1;
	
	half3 D =  NdotL1;
	half3 S =  NdotH1;
	

	half3 Lght2 = normalize(In.LightDir1);
	half3 Half2 = normalize(In.HalfDir1);

	half3 NdotL2 = saturate(dot(Norm,Lght2))  * GLightColor1;
	half3 NdotH2 = pow(saturate(dot(Norm,Half2)),10) * NdotL2;
	
	D +=  NdotL2;
	S +=  NdotH2;

	
	half3 Color = Spec * S + Diff * (D + GAmbientColor);
	
	return half4(Color,Diff.a);			

}

// ================================================================================================

half4 SkinBump_PS30(PS_INPUT In) : COLOR
{
	half4 Diff = tex2D(ColorSampler, In.DiffuseMap).rgba;
	
	if (GLightEnable0)
	{
		half3 Norm = tex2D(NormalSampler, In.NormalMap).rgb * 2 - 1;
		half  Spec = tex2D(NormalSampler, In.NormalMap).a;
		
		half3 Lght1 = normalize(In.LightDir0);
		half3 Half1 = normalize(In.HalfDir0);
		
		half3 NdotL1 = saturate(dot(Norm,Lght1)) * GLightColor0;
		half3 NdotH1 = pow(saturate(dot(Norm,Half1)),10) * NdotL1;
		
		half3 D =  NdotL1;
		half3 S =  NdotH1;
		
		if (GLightEnable1)
		{
			half3 Lght2 = normalize(In.LightDir1);
			half3 Half2 = normalize(In.HalfDir1);
		
			half3 NdotL2 = saturate(dot(Norm,Lght2))  * GLightColor1;
			half3 NdotH2 = pow(saturate(dot(Norm,Half2)),10) * NdotL2;
			
			D +=  NdotL2;
			S +=  NdotH2;
		}
		
		half3 Color = Spec * S + Diff * (D + GAmbientColor);
		
		return half4(Color,Diff.a);			
	}
	else 
	{
		return half4(Diff.rgb * GAmbientColor, Diff.a);
	}
}

// ================================================================================================

technique FXSkinBump_20
{
	pass P0
	{
		PixelShader		= compile ps_2_0 SkinBump_PS20();

		VertexShader	= compile vs_2_0 SkinBump_VS11();
	}
}

// ================================================================================================
	
technique FXSkinBump_30
{
	pass P0
	{
		PixelShader		= compile ps_3_0 SkinBump_PS30();

		VertexShader	= compile vs_3_0 SkinBump_VS11();
	}
}

// ================================================================================================

struct VS_SKIN_WARP_IN
{
	float4 Pos			: POSITION;
	float3 Normal		: NORMAL;
    float2 Tex0			: TEXCOORD0;
    float3 Weights		: BLENDWEIGHT;
    float4 Indices		: BLENDINDICES;
};

struct VS_SKIN_WARP_OUT 
{
	float4	Pos			: POSITION;
	float	Fog			: FOG;
	float2	TexCoord	: TEXCOORD0;
	float4	ViewProj	: TEXCOORD2;
	float3	Diffuse		: COLOR0;
	float3	Specular	: COLOR1;
};

VS_SKIN_WARP_OUT SkinWarp_VS(VS_SKIN_WARP_IN In)
{
	VS_SKIN_WARP_OUT Out;
	
	int4 bones = D3DCOLORtoUBYTE4(In.Indices);
	int bone0 = bones.x;
	int bone1 = bones.y;
	
	float3 Pos		= mul(In.Pos, GSkinBones[bone0]) * In.Weights.x
				    + mul(In.Pos, GSkinBones[bone1]) * (1-In.Weights.x);
				    
	float3 Normal	= mul(In.Normal, GSkinBones[bone0]) * In.Weights.x
					+ mul(In.Normal, GSkinBones[bone1]) * (1-In.Weights.x);

	float4 PosH		= mul(float4(Pos, 1), GClipMat);

    Out.Fog			= PosH.z * GFogParams.y + GFogParams.x;
	Out.Pos			= PosH;
	
	float4 T;
	T.xy = (float2(PosH.x,-PosH.y) + PosH.w) * 0.5;
	T.z = 0;
	T.w = PosH.w;

	Out.ViewProj = T;	
	
    //Out.ViewProj	= mul(float4(Pos,1), GViewMat);
    Out.TexCoord = In.Tex0;        
    
    float3 Diffuse = GAmbient;
	
	float3 L0 = lit(dot(Normal, GLightDir0), dot(Normal, GHalfDir0), GMaterial.w);
	float3 L1 = lit(dot(Normal, GLightDir1), dot(Normal, GHalfDir1), GMaterial.w);
	float3 L2 = lit(dot(Normal, GLightDir2), dot(Normal, GHalfDir2), GMaterial.w);
	float3 L3 = lit(dot(Normal, GLightDir3), dot(Normal, GHalfDir3), GMaterial.w);
			
	Out.Diffuse = Diffuse + (L0.y * GLightCol0) 
						  + (L1.y * GLightCol1) 
						  + (L2.y * GLightCol2) 
						  + (L3.y * GLightCol3);
	
	float3 Specular = (L0.z * GLightCol0) 
					+ (L1.z * GLightCol1) 
					+ (L2.z * GLightCol2) 
					+ (L3.z * GLightCol3);
	
	Out.Specular = Specular * (float3) GMaterial;

    //float Z = PosH.z/PosH.w;
    //float ZScale = 54 * saturate(Z - 0.95); // this gives nice curve!

	//Out.Depth = saturate(ZScale);    
		
	return Out;
}

half4 SkinWarp_PS(VS_SKIN_WARP_OUT In) : COLOR
{
	/*
	half3 D = tex2D(Sampler0, In.TexCoord);
	half3 W = tex2D(Sampler2, In.TexCoord);

	half4 TU = In.ViewProj;
	TU.x += (W.r * 2 - 1) * 0.01F;
	TU.y += (W.g * 2 - 1) * 0.01F;
	
	half3 F = tex2Dproj(Sampler1, TU);
	
	return half4(F*In.Diffuse + In.Specular,1);	
	*/
	/*
	half4 D = tex2D(Sampler0, In.TexCoord);
	
	half3 F = tex2Dproj(Sampler1, In.ViewProj);
	return half4(F*D.a + (half3)D*(1-D.a),1);
	*/

	//half S = 0.15 * In.Depth + 0.01;
	//half S = 0.01F;

	half4 D = tex2D(Sampler0, In.TexCoord);
	half3 W = tex2D(Sampler2, In.TexCoord*2);

	half4 TU = In.ViewProj;
	TU.x += (W.r * 2 - 1) * 0.05;
	TU.y += (W.g * 2 - 1) * 0.05;
	
	half3 F = tex2Dproj(Sampler1, TU);
	F = F * In.Diffuse + In.Specular;
		
	return half4(F*(1-D.a) + (half3)D*D.a,1);	
}

technique FXSkinWarp
{
	pass P0
	{
		VertexShader	= compile vs_2_0 SkinWarp_VS();
		PixelShader		= compile ps_2_0 SkinWarp_PS();
	}
}
