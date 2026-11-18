//#pragma pack_matrix(row_major)

// globals
float4x4	GClipMat	: register(c0);
float4x4	GObjMat		: register(c4);
float4x4	GViewMat	: register(c40);
float4x4	GViewMatNeg	: register(c44);

float4		GTimeConst	: register(c4);
float4		GFogParams	: register(c9);

float3		GLightPos	: register(c13);
float3		GLightColor	: register(c12);
float3		GAmbient	: register(c11);
float4		GMaterial		: register(c10);

float3		GLightCol0		: register(c12);
float3		GLightDir0		: register(c13);
float3		GHalfDir0		: register(c14);

float3		GLightCol1		: register(c15);
float3		GLightDir1		: register(c16);
float3		GHalfDir1		: register(c17);

float4		GMoveSprite		: register(c8);
float4		GParticleColor	: register(c10);
float4		GAttenuation	: register(c14);
float4x2	GParticleUVs	: register(c16);
float4x3	GParticleCorners: register(c18);

// for pixel shader
float3		GLightWorldPos	: register(c0);
float3		GEyeWorldPos	: register(c1);
float2		GShadowMapSize	: register(c2);
float4x4	GLightMatrix	: register(c4);
float4x4	GWorldToScreen	: register(c4);

float4x2	GTexTilePan[4]	: register(c24);
float4		GZProj			: register(c14);

sampler DiffuseSampler	: register(s0);
sampler FBTexSampler	: register(s1);
sampler WarpSampler		: register(s2);

sampler Sampler0		: register(s0);
sampler Sampler1		: register(s1);
sampler Sampler2		: register(s2);
sampler Sampler3		: register(s3);

// ==================================================================================

struct VS_INPUT
{
	float4 Pos			: POSITION;
    float3 Normal		: NORMAL;
    float2 TexCoord		: TEXCOORD0;
};

struct VS_INPUT_3C2
{
	float4 Pos			: POSITION;
    float4 Diffuse		: COLOR0;
    float2 TexCoord		: TEXCOORD0;
};

struct VS_INPUT_31
{
	float3 Pos			: POSITION;
	float1 Index		: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position 
    float2 TexCoord	  : TEXCOORD0;
	float4 Diffuse	  : COLOR0;
};

struct VS_OUTPUT_FB
{
    float4 Position   : POSITION;   // vertex position 
    float2 Tex0		  : TEXCOORD0;
    float4 Tex1		  : TEXCOORD1;
};

struct VS_OUTPUT_3C2
{
    float4 Position   : POSITION;   // vertex position 
    float4 Diffuse	  : COLOR0;
    float2 Tex0		  : TEXCOORD0;
    float4 Tex1		  : TEXCOORD1;
    float1 Fog		  : FOG;
};

struct VS_OUTPUT_POINT
{
    float4 Pos			: POSITION;   // vertex position 
	float4 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float4 Color		: COLOR0;
	float1 Fog			: FOG;
};

VS_OUTPUT ConstColorVS(VS_INPUT In)
{
    VS_OUTPUT Out;

    Out.Position = mul(In.Pos, GClipMat);
    Out.Diffuse = float4(1,1,1,1);
    Out.TexCoord = In.TexCoord;
    
    return Out;
}

VS_OUTPUT_FB ConstColorFB_VS(VS_INPUT In)
{
    VS_OUTPUT_FB Out;

    Out.Position	= mul(In.Pos, GClipMat);
    Out.Tex0		= In.TexCoord;  
    Out.Tex1		= mul(In.Pos, GViewMat);
    
    return Out;
}

VS_OUTPUT_3C2 ConstColor3C2_VS(VS_INPUT_3C2 In)
{
    VS_OUTPUT_3C2 Out;

	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Position	= Pos;
    Out.Fog			= Pos.z * GFogParams.y + GFogParams.x;
    Out.Diffuse		= In.Diffuse;
    Out.Tex0		= In.TexCoord;  
    Out.Tex1		= mul(In.Pos, GViewMat);
    
    return Out;
}

VS_OUTPUT SimpleLightVS(VS_INPUT In)
{
    VS_OUTPUT Out;

    Out.Position = mul(In.Pos, GClipMat);
    float3 LightVec = normalize(GLightPos - mul(In.Pos, GObjMat));
    Out.Diffuse = max(0,dot(In.Normal, LightVec));
    Out.TexCoord = In.TexCoord;
    
    return Out;
}

struct PS_OUTPUT
{
    float4 Color : COLOR0;  
};

PS_OUTPUT SimplePS(VS_OUTPUT In)
{
	PS_OUTPUT Out;
	
	Out.Color = In.Diffuse;
	
	return Out;
}

PS_OUTPUT ConstPS()
{
	PS_OUTPUT Out;
	Out.Color = float4(1,0,0,0);
	return Out;
}

float4 DecalPS(VS_OUTPUT In) : COLOR0
{
	return In.Diffuse * tex2D(DiffuseSampler, In.TexCoord);
}

float4 TestWarpDiffusePS(VS_OUTPUT_FB In) : COLOR0
{
	float4 D = tex2D(DiffuseSampler, In.Tex0);
	//float4 W = tex2D(WarpSampler, In.Tex0);
	//float4 F = tex2Dproj(FBTexSampler, In.Tex1);
	float4 TU = In.Tex1;
	TU.x += 0.05F * D.r;
	TU.y += 0.05F * D.b;
	float4 F = tex2Dproj(FBTexSampler, TU);
	
	return D*0.6F + F*0.4F;
	//return F;
}

float4 TestWarpPS(VS_OUTPUT_FB In) : COLOR0
{
	float4 D = tex2D(DiffuseSampler, In.Tex0);
	//float4 W = tex2D(WarpSampler, In.Tex0.xy + GTimeConst.xy);
	float4 W1 = tex2D(WarpSampler, In.Tex0.xy + GTimeConst.xy);
	float4 W2 = tex2D(WarpSampler, In.Tex0.xy + GTimeConst.zw);
	
	float4 W = (W1+W2)*0.5F;
	
	float4 TU = In.Tex1;
	TU.x += 0.025F * (W.g * 2 - 1);
	TU.y += 0.025F * (W.b * 2 - 1);
	
	float4 F = tex2Dproj(FBTexSampler, TU);

	return D*0.4F + F*0.6F;
}

float4 TestWarp3C2_PS(VS_OUTPUT_3C2 In) : COLOR0
{
	float4 D = tex2D(DiffuseSampler, In.Tex0);
	float4 W = tex2D(WarpSampler, In.Tex0.xy);

	float4 TU = In.Tex1;

	TU.x += 0.01F * (W.g * 2 - 1);
	TU.y += 0.01F * (W.b * 2 - 1);
	
	float4 F = tex2Dproj(FBTexSampler, TU);

	return F;
	
	/*	
	//return D.w * F;
	
	
	float4 Out;
	
	Out.xyz = F.xyz * D.w;
	Out.w = D.w * In.Diffuse.w;
	
	return Out;
	
	
	//return F;
	*/
}

float4 TestParticle3C2_PS(VS_OUTPUT_3C2 In) : COLOR0
{
	float4 D = tex2D(DiffuseSampler, In.Tex0.xy);
	half4 W = tex2D(WarpSampler, In.Tex0.xy);
	
	float4 TU = In.Tex1;

	TU.x += 0.05F * (W.r * 2 - 1);
	TU.y += 0.05F * (W.g * 2 - 1);
	//TU.y += 0.02F * W.g;
	
	float3 F = tex2Dproj(FBTexSampler, TU);
		
	//float4 F = tex2D(FBTexSampler, In.Tex0);
	float4 Out = float4(F,(1-D.a)*In.Diffuse.a);
	
	return Out;
}

// ==============================================================================================

/*
struct ShadowOutput
{
	float4	Position	: POSITION;
	//float	ScreenZ		: TEXCOORD0;
	float4	WorldPos	: TEXCOORD0;
};

ShadowOutput ShadowMapVS(float4 Position : POSITION)
{
	ShadowOutput Out;
	
	float4 ScreenPos	= mul(Position, GClipMat);
	Out.Position		= ScreenPos;
	//Out.ScreenZ			= ScreenPos.z;
	Out.WorldPos		= mul(Position, GObjMat);
	
	return Out;
}

float4 ShadowMapPS(ShadowOutput In) : COLOR0
{
	//return float4(In.ScreenZ, In.ScreenZ, In.ScreenZ, 0);
	//float A = In.ScreenZ * 0.05F;
	//return float4(A,A,A,0);
	
	float4 WorldPos = In.WorldPos;
	float4 LightPos = mul(WorldPos, GLightMatrix);
	
	float S = LightPos.z;
	
	return float4(S,S,S,0);
}
*/
/*
void ShadowMapVS(float4		Position	: POSITION,
				 out float4 ClipPos		: POSITION,
				 out float2 Depth		: TEXCOORD0,
				 out float3 WorldPos	: TEXCOORD1)
{
	ClipPos = mul(Position, GClipMat);
	WorldPos = mul(Position, GObjMat);
	Depth = float2(ClipPos.z, ClipPos.w);
}

float4 ShadowMapPS(float2 Depth : TEXCOORD0,
				   float3 WorldPos : TEXCOORD1) : COLOR
{
	//float Z = Depth.x / Depth.y;
	float Z = distance(WorldPos, GLightWorldPos)/256.0F;
	return float4(Z,Z,Z,Depth.y);
}
*/

void ShadowMapVS(float4		Position	: POSITION,
				 out float4 ClipPos		: POSITION,
				 out float2 Depth		: TEXCOORD0)
{
	ClipPos		= mul(Position, GClipMat);
	Depth		= float2(ClipPos.z, ClipPos.w);
}

float4 ShadowMapPS(float2 Depth : TEXCOORD0) : COLOR
{
	float Z = Depth.x / Depth.y;
	return float4(Z,Z,Z,1);
}
				
// -------------------------

struct VertexInput
{
	float4	Position	: POSITION;
	float3	Normal		: NORMAL;
};

struct VertexOutput
{
	float4	Position	: POSITION;
	float4	ScreenPos	: TEXCOORD0;
	float3	WorldPos	: TEXCOORD1;
	float3	WorldNormal	: TEXCOORD2;
};

struct PixelOutput
{
	float4	Color0		: COLOR0;
	float4	Color1		: COLOR1;
};

VertexOutput ZPassVS(VertexInput In)
{
	VertexOutput Out;
	
	float4 ScreenPos	= mul(In.Position, GClipMat);
	Out.Position		= ScreenPos;
	Out.ScreenPos		= ScreenPos;
	Out.WorldPos		= mul(In.Position, GObjMat);
	//Out.WorldNormal		= mul(GObjMatIT, In.Normal);
	Out.WorldNormal		= mul(In.Normal, (float3x3) GObjMat);
	
	return Out;
}

/*
PixelOutput ZPassPS(VertexOutput In)
{
	PixelOutput Out;
	
	float Z = In.ScreenPos.z;
	//float A = saturate(abs(Z-0.5) * 0.2F);
	//float A = saturate(max(0,Z-0.25) * 0.1);
	float A = Z*0.05;
	
	//Out.Color0 = float4(In.WorldPos, In.ScreenPos.z);
	//Out.Color1 = float4(In.WorldNormal, 1.0F);
		
	Out.Color0 = float4(A,A,A,0);
	Out.Color1 = float4(A,A,A,0);
	
	return Out;
}
*/

PixelOutput ZPassPS(VertexOutput In)
{
	PixelOutput Out;
		
	Out.Color0 = float4(In.WorldPos, In.ScreenPos.w);
	Out.Color1 = float4(normalize(In.WorldNormal), 1.0F);
			
	return Out;
}

struct ScreenInput
{
	float4	Position	: POSITION;
	float2	TexCoord	: TEXCOORD0;
};

struct ScreenOutput
{
	float4	Position	: POSITION;
	float2	TexCoord	: TEXCOORD0;
};

ScreenOutput ComposeVS(ScreenInput In)
{
	ScreenOutput Out;
	
	Out.Position	= In.Position;
	Out.TexCoord	= In.TexCoord;
	//Out.TexCoord	= float2(In.Position.x*0.5f+0.5f, 1.0f - (In.Position.y*0.5f+0.5f));
	
	return Out;
}

/*
float4 ComposePS(ScreenOutput In) : COLOR
{
	float3 WorldPos		= tex2D(Sampler0, In.TexCoord);
	float3 WorldNormal	= tex2D(Sampler1, In.TexCoord);
		
	float3 L = normalize(GLightWorldPos - WorldPos);
	float NdotL = dot(WorldNormal,L);
	
	float3 EyeVec = WorldPos - GEyeWorldPos;
	float3 R = reflect(normalize(EyeVec), WorldNormal);	
	float S = pow(saturate(dot(R, WorldNormal)), 16);
	
	float I = NdotL + NdotL*S;
	
	//return WorldPos;
	return float4(I,I,I,0);
}
*/


#define SHADOW_EPSILON 0.0001f
//#define SHADOW_EPSILON 0.0001f

/*
float4 ComposePS(ScreenOutput In) : COLOR
{
	float4 WorldPos		= tex2D(Sampler0, In.TexCoord);
	float3 WorldNormal	= tex2D(Sampler1, In.TexCoord);

	// transform world pos to light space
	float4 L = mul(float4((float3)WorldPos,1), GLightMatrix);

	// compute shadow map texcoords	
    float2 STC = 0.5 * L.xy / L.w + float2( 0.5, 0.5 );
    STC.y = 1.0f - STC.y;
		
	// z in light space
	float Z = L.z/L.w;
		
	float A = L.z >= 0 ? 1 : 0;
	float3 S = tex2D(Sampler2, STC);
	
	float F = S.x + SHADOW_EPSILON < Z ? 0 : 1;
	
	return float4(F,F,F,0);	
}
*/

float4 ComposePS(ScreenOutput In) : COLOR
{
	float4 WorldPos		= tex2D(Sampler0, In.TexCoord);
	float3 WorldNormal	= tex2D(Sampler1, In.TexCoord);

	// transform world pos to light space
	float4 L = mul(float4((float3)WorldPos,1), GLightMatrix);

	// compute shadow map texcoords	
    float2 STC = 0.5 * L.xy / L.w + float2(0.5, 0.5);
    STC.y = 1.0f - STC.y;
		
	// z in light space
	float Z = L.z/L.w;
	//float Z = distance(GLightWorldPos, (float3) WorldPos)/256.0F;
		
	/*
    float S[4];
    S[0] = (tex2D(Sampler2, STC) + SHADOW_EPSILON < Z) ? 0.0f: 1.0f;  
    S[1] = (tex2D(Sampler2, STC  + float2(1.0/GShadowMapSize.x, 0)) + SHADOW_EPSILON < Z) ? 0.0f: 1.0f;  
    S[2] = (tex2D(Sampler2, STC  + float2(0, 1.0/GShadowMapSize.y)) + SHADOW_EPSILON < Z) ? 0.0f: 1.0f;  
    S[3] = (tex2D(Sampler2, STC  + float2(1.0/GShadowMapSize.x, 1.0/GShadowMapSize.y)) + SHADOW_EPSILON < Z) ? 0.0f: 1.0f;  
	

    // transform to texel space
    float2 TPos = STC * GShadowMapSize;//float2(SMAP_SIZE_X * STC.x, SMAP_SIZE_Y * STC.y);
    
    // Determine the lerp amounts           
    float2 Lerps = frac(TPos);
	
    // lerp between the shadow values to calculate our light amount
    float F = lerp(lerp( S[0], S[1], Lerps.x),
                   lerp( S[2], S[3], Lerps.x),
                   Lerps.y);
	
	*/
	float F = (tex2D(Sampler2, STC).x + SHADOW_EPSILON < Z) ? 0.0f: 1.0f;
		
	// lighting
	float3 LVec = normalize(GLightWorldPos - WorldPos);
	float NdotL = dot(WorldNormal,LVec);
	
	//float A = L.z >= 0 ? 1 : 0;
	
	//F = saturate(F+0.1)*NdotL;
	F = F * NdotL;
		
	return float4(F,F,F,0);	
}

// ==============================================================================================

struct InputNTU
{
	float4	Position	: POSITION;
	float3	Normal		: NORMAL;
};


// ==============================================================================================

void DepthMapVS(float4 Position		: POSITION,
				out float4 OutPos	: POSITION,
				out float2 Depth	: TEXCOORD0)
{
	OutPos = mul(Position, GClipMat);
	Depth = float2(OutPos.z, OutPos.w);	
}

float4 DepthMapPS(float2 Depth : TEXCOORD0) : COLOR0
{
	//float Z = saturate(abs(Depth.x - 10) * 0.1);

	float Z = saturate(abs(Depth.x - 0) * 0.025);
	//float Z = Depth.x / Depth.y;
	//Z = saturate(Z-0.75)*4;
	//float Z = Depth.x * 0.01;
	return float4(Z,Z,Z,0);
}

/*
// ==============================================================================================

void WarpNTU_VS(in	float4 Pos		: POSITION,
				in	float2 TexCoord	: TEXCOORD0,
				out float4 OutPos	: POSITION,
				out float2 Tex0		: TEXCOORD0,
				out float4 Tex1		: TEXCOORD1,
				out float  Depth	: TEXCOORD2)
{
	OutPos = mul(Pos, GClipMat);
	Tex0 = TexCoord;
		
	//Tex1 = TexCoord;
	
	//float4 CamPos = mul(Pos, GViewMat);
	
	Tex1 = mul(Pos, GViewMat);
	
    //Tex1 = 0.5 * OutPos.xy + float2(0.5,0.5);
    //Tex1.y = 1.0 - Tex1.y;	
    
    float Z = OutPos.z/OutPos.w;
    float ZScale = 50 * saturate(Z - 0.97); // this gives nice curve!
    
    Depth = ZScale;
}				

float4 WarpNTU_PS(float2 Tex0	: TEXCOORD0,
				  float4 Tex1	: TEXCOORD1,
				  float  Depth	: TEXCOORD2) : COLOR
{
	float4 D = tex2D(Sampler0, Tex0);
	float4 F = tex2D(Sampler1, Tex1);

	float S = Depth;
	
	return S * F + (1-S) * D;
}

void WarpFinal_VS(in	float4 Pos		: POSITION,
				  out	float4 OutPos	: POSITION,
				  out	float4 Tex		: TEXCOORD0)				  
{
	OutPos = mul(Pos, GClipMat);
	Tex = mul(Pos, GViewMat);
}

float4 WarpFinal_PS(float4 Tex : TEXCOORD0) : COLOR
{
	float4 D = tex2D(Sampler0, Tex);
	return D;
}
*/
// ==============================================================================================

void WarpNTU_VS(in	float4 Pos		: POSITION,
				in	float2 TexCoord	: TEXCOORD0,
				out float4 OutPos	: POSITION,
				out float2 Tex		: TEXCOORD0,
				out float  Depth	: TEXCOORD1)
{
	OutPos	= mul(Pos, GClipMat);
	Tex		= TexCoord;
	    
    float Z = OutPos.z/OutPos.w;
    float ZScale = 50 * saturate(Z - 0.97); // this gives nice curve!
    
    Depth = saturate(ZScale);
}				

float4 WarpNTU_PS(float2 Tex	: TEXCOORD0,
				  float  Depth	: TEXCOORD1) : COLOR
{
	float4 D = tex2D(Sampler0, Tex);
	
	// z=1 so that we can dot it later
	return float4(D.x * Depth, D.y * Depth, 1, 0);
}

void WarpFinal_VS(in	float4 Pos		: POSITION,
				  in	float2 TexCoord	: TEXCOORD0,
				  out	float4 OutPos	: POSITION,
				  out	float2 Tex0		: TEXCOORD0,
				  out	float3 Tex1		: TEXCOORD1,
				  out	float3 Tex2		: TEXCOORD2)
{
	OutPos = mul(Pos, GClipMat);

	float2 T;
    T	= 0.5 * OutPos.xy / OutPos.w + float2(0.5,0.5);
    T.y	= 1.0 - T.y;
	
	Tex0 = T;
	
	float S = 0.1;
	
	Tex1 = float3(S,0,T.x);
	Tex2 = float3(0,S,T.y);
}

float4 WarpFinal_PS(float2 Tex0 : TEXCOORD0,
					float3 Tex1 : TEXCOORD1,
					float3 Tex2 : TEXCOORD2) : COLOR
{	
	// sample distortion texture (z scaled)
	float3 D = tex2D(Sampler0, Tex0);
	
	float2 TU;
	TU.x = dot(D * 2 - 1,Tex1);
	TU.y = dot(D * 2 - 1,Tex2);
	
	float4 F = tex2D(Sampler2, TU);
	
	return F;	
}

// ==============================================================================================

void TestNTU_VS(in	float4 Pos		: POSITION,
				out float4 OutPos	: POSITION,
				out float2 Tex0		: TEXCOORD0)
{
	OutPos	= mul(Pos, GClipMat);
	Tex0	= 0.5 * OutPos.xy/OutPos.w + float2(0.5,0.5);
	Tex0.y	= 1.0 - Tex0.y;
	//Tex0 = OutPos;
}				

float4 TestNTU_PS(float2 Tex0	: TEXCOORD0) : COLOR
{
	return tex2D(Sampler0, Tex0);
	//return float4(1,0,0,1);
}

// ==============================================================================================

void DOF_VS(in	float4 Pos		: POSITION,
			in	float2 TexCoord	: TEXCOORD0,
			out	float4 OutPos	: POSITION,
			out float2 Tex0		: TEXCOORD0)
{
	OutPos = Pos;
	Tex0 = TexCoord;
}			
		
float4 DOF_PS(in float2 Tex0	: TEXCOORD) : COLOR
{	
	float4 F = tex2D(Sampler0, Tex0);
	float4 D = tex2D(Sampler1, Tex0);
	
	float4 C = float4((float3) F, D.r);	
	//float4 C = float4((float3) F, 1);
	
	return C;
}		
		
// ==============================================================================================

void PolygonVS(in	float4 Pos		: POSITION,
			   in	float2 InTex	: TEXCOORD0,
			   out	float4 OutPos	: POSITION,
			   out	float  Fog		: FOG,
			   out	float4 OutTex1	: TEXCOORD0,
			   out	float4 OutTex2	: TEXCOORD1,
			   out	float2 OutTex3	: TEXCOORD2)
{
	OutPos		= mul(Pos, GClipMat);
	OutTex1		= mul(Pos, GViewMat);
	OutTex2		= mul(Pos, GViewMatNeg);
	OutTex3		= InTex;
	
	Fog = 1;
}
			  
float4 PolygonPS(in float4 Tex1 : TEXCOORD0,
				 in float4 Tex2 : TEXCOORD1,
				 in float2 Tex3 : TEXCOORD2) : COLOR
{
	//float4 D = tex2Dproj(Sampler0, Tex1);
	//return 0.2 + 0.6 * D;
	
	//float4 Reflect = tex2Dproj(Sampler0, Tex1);
	//float4 Refract = tex2Dproj(Sampler1, Tex2);
	
	/*
	float4 TU = Tex2;
	TU.x += 0.05 * (Reflect.r * 2 - 1);
	TU.y += 0.05 * (Reflect.g * 2 - 1);

	float4 Refract = tex2Dproj(Sampler1, TU);
	*/
	
	//return 0.75 * Reflect + 0.25 * Refract;
	//return Refract;

/*
	float4 W = tex2D(Sampler2, Tex3*4);

	float4 TU = Tex2;
	TU.x += 0.025 * (W.r * 2 - 1);
	TU.y += 0.025 * (W.g * 2 - 1);

	float4 Refract = tex2Dproj(Sampler1, TU);
	float4 Reflect = tex2Dproj(Sampler0, Tex1);
	
	return 0.5 * Reflect + 0.5 * Refract;
*/

	return float4(1,0,0,0);
}			
	
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
	
	//OutT0.x = dot(Pos, GTexTilePan[0]);
	//OutT0.y = dot(Pos, GTexTilePan[1]);	
	OutT0 = mul(Pos, GTexTilePan[0]);
	
	OutT1.x = dot(Pos, GZProj);
	OutT1.y = 0.5;
	
	OutColor = dot(Normal,GZProj) > 0 ? 0 : 1;
}

float4 ShadowPS(float4 Color: COLOR0,
			    float2 T0	: TEXCOORD0,
			    float2 T1	: TEXCOORD1) : COLOR
{
	float4 S = tex2D(Sampler0, T0);
	float4 Z = tex2D(Sampler1, T1);

	return S*Color*Z;
}

// ==============================================================================================
		  
float4 PointPS(VS_OUTPUT_POINT In) : COLOR
{
	half3 col = tex2Dproj(Sampler0, In.Tex0);
	col = col * tex2D(Sampler1, In.Tex1);
	col = col * tex2D(Sampler2, In.Tex2);
	col = col * In.Color * 0.2;
	//float a = col.x+col.y+col.z;
	//return float4(0.1,0.1,0.1,0.1);
	return half4(col,1);
}

VS_OUTPUT_POINT PointVS(VS_INPUT_31 In)
{
	VS_OUTPUT_POINT Out;
	In.Pos		= In.Pos + GMoveSprite;
	In.Pos		= float4(frac(float3(In.Pos.x,In.Pos.y,In.Pos.z)),1);
	Out.Tex2	= GParticleUVs[In.Index.x];
	float4 pos	= float4(In.Pos + GParticleCorners[In.Index.x], 1);
	Out.Tex0	= mul(pos, GLightMatrix);
	Out.Tex1	= mul(pos, GAttenuation);
	Out.Pos		= mul(pos, GClipMat);
	Out.Color	= GParticleColor;
	Out.Fog		= Out.Pos.z * GFogParams.y + GFogParams.x;
	//Out.PSize	= 320/Out.Pos.w;
	
	return Out;
}


technique particle_vol_proj_FX
{
	pass p0
	{
        VertexShader = compile vs_1_1 PointVS();
        PixelShader  = compile ps_2_0 PointPS();   
   	}
}

// ==============================================================================================

/*
technique shadow_test
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 ShadowVS();
        PixelShader  = compile ps_2_0 ShadowPS();        
    }
}

technique zpass_test
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 ZPassVS();
        PixelShader  = compile ps_2_0 ZPassPS();        
    }
}

technique shadowmap_test
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 ShadowMapVS();
        PixelShader  = compile ps_2_0 ShadowMapPS();        
    }
}

technique compose_stuff
{
	pass P0
	{
        VertexShader = compile vs_2_0 ComposeVS();
        PixelShader  = compile ps_2_0 ComposePS();        
	}
}

technique depthmap_test
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 DepthMapVS();
        PixelShader  = compile ps_2_0 DepthMapPS();        
    }
}

technique warp_ntu
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 WarpNTU_VS();
        PixelShader  = compile ps_1_1 WarpNTU_PS();        
    }
}

technique warp_ntu_final
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 WarpFinal_VS();
        PixelShader  = compile ps_1_1 WarpFinal_PS();        
    }
}

technique test_crap
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 TestNTU_VS();
        PixelShader  = compile ps_1_1 TestNTU_PS();        
    }
}

technique dof_overlay
{
    pass P0 
    {          
        VertexShader = compile vs_2_0 DOF_VS();
        PixelShader  = compile ps_2_0 DOF_PS();        
    }
}

technique polygon
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 PolygonVS();
        PixelShader  = compile ps_2_0 PolygonPS();        
    }
}

*/

// ==============================================================================================
/*
technique ConstColor
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ConstColorVS();
        PixelShader  = compile ps_1_1 SimplePS();        
    }
}

technique ConstColorNTU < string Material = "defaultNTU"; > 
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ConstColorVS();
        PixelShader  = compile ps_1_1 SimplePS();        
    }
}

technique DecalNTU < string Material = "defaultNTU"; > 
{
    pass P0 
    {          
        VertexShader = compile vs_1_1 ConstColorVS();
        PixelShader  = compile ps_1_1 DecalPS();        
    }
}

technique SimpleLight
{
    pass P0
    {          
        VertexShader = compile vs_1_1 SimpleLightVS();
        PixelShader  = compile ps_3_0 SimplePS();
    }
}

technique TestWarpDiffuse
{
    pass P0
    {          
        VertexShader = compile vs_1_1 ConstColorFB_VS();
        PixelShader  = compile ps_2_0 TestWarpDiffusePS();
    }
}

technique TestWarpTex
{
    pass P0
    {          
        VertexShader = compile vs_1_1 ConstColorFB_VS();
        PixelShader  = compile ps_2_0 TestWarpPS();
    }
}

technique TestWarp3C2
{
    pass P0
    {          
        VertexShader = compile vs_1_1 ConstColor3C2_VS();
        PixelShader  = compile ps_2_0 TestWarp3C2_PS();
    }
}
*/

void TestWarpNTU_VS(in	float4 InPos	: POSITION,
					in	float2 InTC		: TEXCOORD0,
					out	float4 OutPos	: POSITION,
					out float2 OutTC	: TEXCOORD0,
					out	float4 OutProj	: TEXCOORD1)
{
	OutPos		= mul(InPos, GClipMat);
	OutTC		= InTC;
    OutProj		= mul(InPos, GViewMat);
}
				 
half4 TestWarpNTU_PS(float4 TexCoord	: TEXCOORD0,
					 float4 ViewProj	: TEXCOORD1) : COLOR
{
	float4 TC = ViewProj;
	
	half3 W = tex2D(Sampler0, TexCoord);
	
	TC.x += (W.r * 2 - 1) * 0.05;
	TC.y += (W.g * 2 - 1) * 0.05;
	
	half3 F = tex2Dproj(Sampler1, TC);
	
	return half4(F,1);
}

technique TestWarpNTU
{
    pass P0
    {          
        VertexShader = compile vs_1_1 TestWarpNTU_VS();
        PixelShader  = compile ps_2_0 TestWarpNTU_PS();
    }
}

// ==============================================================================================

void TransWarp_VS(in	float4 Pos		: POSITION,
				  in	float3 Normal	: NORMAL,
				  in	float2 TC		: TEXCOORD0,
				  out	float4 OutPos	: POSITION,
				  out	float  Fog		: FOG,
				  out	float3 OutColor	: COLOR0,
				  out	float2 OutTC	: TEXCOORD0,
				  out	float4 OutProj	: TEXCOORD1,
				  out	float  OutDepth	: TEXCOORD2)
{
	OutPos	= mul(Pos, GClipMat);
	OutTC	= TC;
	
	float4 T;
	T.xy = (float2(OutPos.x,-OutPos.y) + OutPos.w) * 0.5;
	T.z = 0;
	T.w = OutPos.w;

	OutProj = T;	
	
    float Z = OutPos.z/OutPos.w;
    float ZScale = 54 * saturate(Z - 0.95); // this gives nice curve!

	OutDepth = saturate(ZScale);    
	Fog = 1;
	
    float3 Diffuse = GAmbient;
	
	float3 L0 = lit(dot(Normal, GLightDir0), dot(Normal, GHalfDir0), GMaterial.w);
	float3 L1 = lit(dot(Normal, GLightDir1), dot(Normal, GHalfDir1), GMaterial.w);
	
	OutColor = Diffuse + (L0.y * GLightCol0) 
				       + (L1.y * GLightCol1); 
}


half4 TransWarp_PS(float3 Color		: COLOR0,
				   float2 TC		: TEXCOORD0,
				   float4 TCProj	: TEXCOORD1,
				   float  Depth		: TEXCOORD2) : COLOR
{
	half4 D = tex2D(Sampler0, TC);
	
	half S = 0.12 * Depth + 0.02;
	
	float4 TU;

	//TU.x = TCProj.x + (D.g * 2 - 1) * S;
	//TU.y = TCProj.y + (D.b * 2 - 1) * S;
	TU.xy = TCProj.xy + (D.gb * 2 - 1) * S;
	TU.wz = TCProj.wz;
	
	half3 F = tex2Dproj(Sampler1, TU);
			
	half A = D.a;
	
	return half4((half3) D * Color * A + (1-A) * F, 1);
}
					
technique FXTransWarpNTU
{
    pass P0
    {          
        VertexShader = compile vs_1_1 TransWarp_VS();
        PixelShader  = compile ps_2_0 TransWarp_PS();
    }
}
					