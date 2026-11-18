
float4x4	GClipMat	: register(c0);
float4x4	GObjMat		: register(c4);
float4		GFogParams	: register(c9);

float4x2	GTexTilePan[8]	: register(c40);
float4		GZProj[8]		: register(c60);

sampler Sampler0		: register(s0);
sampler Sampler1		: register(s1);
sampler Sampler2		: register(s2);
sampler Sampler3		: register(s3);

sampler	ShadowMaps[7]	: register(s1);

/*
// ==============================================================================================

void ShadowVS(in	float4 Pos		: POSITION,
			  in	float3 Normal	: NORMAL,
			  out	float4 OutPos	: POSITION,
			  out	float4 OutColor	: COLOR0,
			  out	float  Fog		: FOG,
			  out	float2 OutT0	: TEXCOORD0,
			  out	float2 OutT1	: TEXCOORD1)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;
		
	OutT0.x = dot(Pos, GZProj[0]);
	OutT0.y = 0.5;
	
	OutT1 = mul(Pos, GTexTilePan[0]);
	
	OutColor = dot(Normal,GZProj[0]) >= 0 ? 0 : 1;
}

float4 ShadowPS(float4 Color: COLOR0,
			    float2 T0	: TEXCOORD0,
			    float2 T1	: TEXCOORD1) : COLOR
{
	float4 Z = tex2D(Sampler0, T0);
	float4 S = tex2D(Sampler1, T1);
	
	return S*Color*Z;
}

// ==============================================================================================

void Shadow2VS(in	float4 Pos		: POSITION,
			   in	float3 Normal	: NORMAL,
			   out	float4 OutPos	: POSITION,
			   out	float  Fog		: FOG,
			   out	float3 OutZ[2]	: TEXCOORD0,
			   out	float2 OutP[2]	: TEXCOORD2)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;

	for (int i=0; i<2; ++i) {		
		OutZ[i].x = dot(Pos, GZProj[i]);
		OutZ[i].y = 0.5;
		OutZ[i].z = dot(Normal,GZProj[i]) >= 0 ? 0 : 1;
		
		OutP[i] = mul(Pos, GTexTilePan[i]);
	}		
}

float4 Shadow2PS(float4 Color	: COLOR0,
			     float2 Z[2]	: TEXCOORD0,
			     float2 P[2]	: TEXCOORD2) : COLOR
{
	float4 C = 0;
	
	for (int i=0; i<2; ++i) {
		float4 Z = tex2D(Sampler0, Z[i].xy);
		float4 S = tex2D(Samplers[i], P[i]);
	
		C += S * Z * Z.z;
	}
}

// ==============================================================================================

void Shadow3VS(in	float4 Pos		: POSITION,
			   in	float3 Normal	: NORMAL,
			   out	float4 OutPos	: POSITION,
			   out	float  Fog		: FOG,
			   out	float3 OutZ[3]	: TEXCOORD0,
			   out	float2 OutP[3]	: TEXCOORD3)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;

	for (int i=0; i<3; ++i) {		
		OutZ[i].x = dot(Pos, GZProj[i]);
		OutZ[i].y = 0.5;
		OutZ[i].z = dot(Normal,GZProj[i]) >= 0 ? 0 : 1;
		
		OutP[i] = mul(Pos, GTexTilePan[i]);
	}		
}

float4 Shadow3PS(float4 Color	: COLOR0,
			     float2 Z[3]	: TEXCOORD0,
			     float2 P[3]	: TEXCOORD3) : COLOR
{
	float4 C = 0;
	
	for (int i=0; i<3; ++i) {
		float4 Z = tex2D(Sampler0, Z[i].xy);
		float4 S = tex2D(Samplers[i], P[i]);
	
		C += S * Z * Z.z;
	}
}


// ==============================================================================================

void Shadow4VS(in	float4 Pos		: POSITION,
			   in	float3 Normal	: NORMAL,
			   out	float4 OutPos	: POSITION,
			   out	float  Fog		: FOG,
			   out	float3 OutZ[4]	: TEXCOORD0,
			   out	float2 OutP[4]	: TEXCOORD4)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;

	for (int i=0; i<4; ++i) {		
		OutZ[i].x = dot(Pos, GZProj[i]);
		OutZ[i].y = 0.5;
		OutZ[i].z = dot(Normal,GZProj[i]) >= 0 ? 0 : 1;
		
		OutP[i] = mul(Pos, GTexTilePan[i]);
	}		
}

float4 Shadow4PS(float4 Color	: COLOR0,
			     float2 Z[4]	: TEXCOORD0,
			     float2 P[4]	: TEXCOORD4) : COLOR
{
	float4 C = 0;
	
	for (int i=0; i<4; ++i) {
		float4 Z = tex2D(Sampler0, Z[i].xy);
		float4 S = tex2D(Samplers[i], P[i]);
	
		C += S * Z * Z.z;
	}
}

/*
// ==============================================================================================

void Shadow2VS(in	float4 Pos		: POSITION,
			   in	float3 Normal	: NORMAL,
			   out	float4 OutPos	: POSITION,
			   out	float4 OutWPos	: TEXCOORD0,
			   out	float  Fog		: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;
		
	OutWPos = mul(Pos, GObjMat);
}

float4 Shadow2PS(float4 Pos : TEXCOORD0) : COLOR
{
	float2 T0,T1;
	
	T0.x = dot(Pos, GZProj[0]);
	T0.y = 0.5;
	
	T1 = mul(Pos, GTexTilePan[0]);
	
	
	return float4(1,0,0,0);
}
*/

/*
void ShadowTestVS(in	float4 Pos			: POSITION,
				  in	float3 Normal		: NORMAL,
				  out	float4 OutPos		: POSITION,
				  out	float4 WorldPos		: TEXCOORD0,
				  out	float3 WorldNormal	: TEXCOORD1,
				  out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;
			
	WorldPos	= Pos;
	WorldNormal = Normal;
}

half4 ShadowTestPS(float4 WorldPos		: TEXCOORD0,
				   float3 WorldNormal	: TEXCOORD1,
				   uniform int NumShadows) : COLOR
{
	half4 F = 0;
	
	for (int i=0; i<NumShadows; ++i)
	{
		float2 T0, T1;
		
		T0.x = dot(WorldPos, GZProjPS[i]);
		T0.y = 0.5;
		
		T1 = mul(WorldPos, GTexTilePanPS[i]);
		
		half C = dot(WorldNormal,GZProjPS[i]) >= 0 ? 0 : 1;

		half4 Z = tex2D(Sampler0, T0);
		half4 S = tex2D(Samplers[i], T1);
		
		F += S*C*Z;
	}
	
	return F;
}				
*/

// ==============================================================================================

/*
void ShadowProj1VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[1]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;

	for (int i=0; i<1; ++i) {
		float4 T;
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, GZProj[i]) >= 0 ? 0 : 1;
		
		Proj[i] = T;
	}			
}

half4 ShadowProjPS(float4 Proj[8] : TEXCOORD0,
				   uniform int NumShadows) : COLOR
{
	float4 F = 0;
	
	for (int i=0; i<NumShadows; ++i) {
		float4 Z = tex2D(Sampler0, float2(Proj[i].z, 0.5));
		float4 S = tex2D(Sampler1, Proj[i].xy);
		
		F += S*Z*Proj[i].w;
	}
	
	return F;
}
*/


void ShadowProj1VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[1]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= Pos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<1; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
				
		Proj[i] = T;	
	}
}

void ShadowProj2VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[2]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<2; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

void ShadowProj3VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[3]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<3; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

void ShadowProj4VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[4]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<4; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

void ShadowProj5VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[5]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<5; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

void ShadowProj6VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[6]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<6; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

void ShadowProj7VS(in	float4 Pos			: POSITION,
			       in	float3 Normal		: NORMAL,
				   out	float4 OutPos		: POSITION,
				   out	float4 Proj[7]		: TEXCOORD0,
				   out	float  Fog			: FOG)
{
	OutPos = mul(Pos, GClipMat);
    Fog	= OutPos.z * GFogParams.y + GFogParams.x;
	
	for (int i=0; i<7; ++i) {
		float4 T;
		
		T.xy	= mul(Pos, GTexTilePan[i]);
		T.z		= dot(Pos, GZProj[i]);
		T.w		= dot(Normal, -GZProj[i]) >= 0 ? 1 : 0;
		
		Proj[i] = T;	
	}
}

half4 ShadowProjPS(float4 Proj[8] : TEXCOORD0,
				   uniform int NumShadows) : COLOR
{
	half4 F = 0;
	
	for (int i=0; i<NumShadows; ++i) {
		
		half4 Z = tex2D(Sampler0, float2(Proj[i].z, 0.5));
		half4 S = tex2D(ShadowMaps[i], Proj[i].xy);
		
		F += S*Z*Proj[i].w;		
	}
	
	return F;
}

// ==============================================================================================

technique fx_shadow_pass1
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj1VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(1);        
    }
}

technique fx_shadow_pass2
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj2VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(2);        
    }
}

technique fx_shadow_pass3
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj3VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(3);        
    }
}

technique fx_shadow_pass4
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj4VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(4);        
    }
}

technique fx_shadow_pass5
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj5VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(5);        
    }
}

technique fx_shadow_pass6
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj6VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(6);        
    }
}

technique fx_shadow_pass7
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ShadowProj7VS();
        PixelShader  = compile ps_2_0 ShadowProjPS(7);        
    }
}
