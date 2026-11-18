float4x4	GClipMat	: register(c0);
float4x4	GObjMat		: register(c4);
float4x4	GViewMat	: register(c40);
float4		GFogParams	: register(c9);

float3x3	GRotationMatrix;
float3		GEyePos;
float3		GUpVec[4];
float3		GRightVec[4];
float2		GTexCoord[4];

sampler DiffuseSampler	: register(s0);
sampler FBTexSampler	: register(s1);
sampler WarpSampler		: register(s2);
sampler FalloffSampler	: register(s3);

sampler Sampler0		: register(s0);
sampler Sampler1		: register(s1);
sampler Sampler2		: register(s2);
sampler Sampler3		: register(s3);

// ====================================================================================================

struct VS_INPUT
{
	float4	Position	: POSITION;
	float4	Color		: COLOR0;
	float2	TexCoord	: TEXCOORD0;	// x - index into const mem, y - particle size
};

struct VS_INPUT_Spark
{
	float4	Position	: POSITION;
	float4	Color		: COLOR0;
	float4	TexCoord	: TEXCOORD0;	// x - index into const mem, y - particle velocity
										// z - thickness, w - length
};

struct VS_OUTPUT
{
	float4	Position	: POSITION;
	float4	Color		: COLOR0;
	float2	TexCoord	: TEXCOORD0;
};

// ====================================================================================================

VS_OUTPUT SpriteVS(VS_INPUT In)
{
	VS_OUTPUT Out;
	
	float3 Up		= GUpVec[In.TexCoord.x]		* In.TexCoord.y;
	float3 Right	= GRightVec[In.TexCoord.x]	* In.TexCoord.y;
	
	Up		= mul(Up, GRotationMatrix);
	Right	= mul(Right, GRotationMatrix);
	
	Out.Position	= mul(In.Position + float4(Up,1) + float4(Right,1), GClipMat);
	Out.Color		= In.Color;
	Out.TexCoord	= GTexCoord[In.TexCoord.x];
	
	return Out;
}

VS_OUTPUT SparkVS(VS_INPUT_Spark In)
{
	VS_OUTPUT Out;
	
	float3 End = In.Position + In.TexCoord.y;	// velocity
	float3 Right = End - In.Position;
	float3 Up = normalize(cross(Right, In.Position - GEyePos));
	
	Up		*= In.TexCoord.z;	// thickness
	Right	*= In.TexCoord.w;	// length
	
	Out.Position	= mul(In.Position + float4(Up,1) + float4(Right,1), GClipMat);
	Out.Color		= In.Color;
	Out.TexCoord	= GTexCoord[In.TexCoord.x];

	return Out;	
}

// ====================================================================================================

float4 SimplePS(VS_OUTPUT In) : COLOR
{
	return In.Color;
}

technique SpriteEmitter
{
    pass P0
    {          		
        VertexShader = compile vs_1_1 SpriteVS();
        PixelShader  = compile ps_1_1 SimplePS();
    }
}

// ====================================================================================================

struct WarpInput
{
	float4 Pos			: POSITION;
    float4 Diffuse		: COLOR0;
    float2 TexCoord		: TEXCOORD0;
};

struct WarpOutput
{
    float4 Position		: POSITION;   // vertex position 
    float4 Diffuse		: COLOR0;
    float2 Tex0			: TEXCOORD0;
    float4 Tex1			: TEXCOORD1;
    float1 Depth		: TEXCOORD2;
    float1 Fog			: FOG;
};

WarpOutput ParticleWarpVS(WarpInput In)
{
    WarpOutput Out;

	float4 Pos		= mul(In.Pos, GClipMat);
    Out.Position	= Pos;
    Out.Fog			= 1;			// fog already in FB
    Out.Diffuse		= In.Diffuse;
    Out.Tex0		= In.TexCoord;  
    //Out.Tex1		= mul(In.Pos, GViewMat);

	float4 T;
	T.xy = (float2(Pos.x,-Pos.y) + Pos.w) * 0.5;
	T.z = 0;
	T.w = Pos.w;

	Out.Tex1 = T;    
            
    float Z = Pos.z/Pos.w;
    float ZScale = 32 * saturate(Z - 0.96); // this gives nice curve!

	Out.Depth = saturate(ZScale);    
    
    return Out;
}

half4 ParticleWarpPS(WarpOutput In) : COLOR
{
	half4 D	= tex2D(DiffuseSampler, In.Tex0.xy);
	half4 W	= tex2D(WarpSampler, In.Tex0.xy);
			
	half4 TU = In.Tex1;
		
	half S = 0.1 * In.Depth;
	
	//TU.x += S * (W.r * 2 - 1);
	//TU.y += S * (W.g * 2 - 1);
	TU.xy += S * (W.rg * 2 - 1);
	
	half3 F = tex2Dproj(FBTexSampler, TU);
		
	return half4(F,(1-D.a)*In.Diffuse.a);
}

/*
// ====================================================================================================

void ParticleDistortVS_PS11(in		float4 Pos		: POSITION,
							in		float2 TexCoord	: TEXCOORD0,
							out		float4 OutPos	: POSITION,
							out		float2 Tex		: TEXCOORD0,
							out		float  Depth	: TEXCOORD1)
{
	OutPos	= mul(Pos, GClipMat);
	Tex		= TexCoord;
	    
    float Z = OutPos.z/OutPos.w;
    float ZScale = 50 * saturate(Z - 0.97); // this gives nice curve!
    
    Depth = saturate(ZScale);
}				

float4 ParticleDistortPS_PS11(float2 Tex	: TEXCOORD0,
							  float  Depth	: TEXCOORD1) : COLOR
{
	
	float4 D = tex2D(Sampler0, Tex);
	
	// z=1 so that we can dot it later
	return float4(D.x * Depth, D.y * Depth, 1, 0);
	//return float4(0,0,1,1);
}
						 
// ====================================================================================================

void ParticleWarpVS_PS11(in		float4 Pos		: POSITION,
						 in		float4 Color	: COLOR0,
						 in		float2 TexCoord	: TEXCOORD0,
						 out	float4 OutPos	: POSITION,
						 out	float4 Diffuse	: COLOR0,
						 out	float2 Tex0		: TEXCOORD0,
						 out	float3 Tex1		: TEXCOORD1,
						 out	float3 Tex2		: TEXCOORD2,
						 out	float2 Tex3		: TEXCOORD3)
{
	OutPos = mul(Pos, GClipMat);
	Diffuse = Color;

	float2 T;
    T	= 0.5 * OutPos.xy / OutPos.w + float2(0.5,0.5);
    T.y	= 1.0 - T.y;
	
	Tex0 = T;
	//Tex0 = TexCoord;
	
	float S = 0.01;
	
	Tex1 = float3(S,0,T.x);
	Tex2 = float3(0,S,T.y);
	//Tex2 = float3(T.x,T.y,0);
	
	Tex3 = TexCoord;
}

float4 ParticleWarpPS_PS11(float4 Diffuse	: COLOR0,
						   float2 Tex0		: TEXCOORD0,
						   float3 Tex1		: TEXCOORD1,
						   float3 Tex2		: TEXCOORD2,
						   float2 Tex3		: TEXCOORD3) : COLOR
{	
	// sample distortion texture (z scaled)
	float3 W = tex2D(Sampler0, Tex0);
	
	float2 TU;
	TU.x = dot(W*2-1,Tex1);
	TU.y = dot(W*2-1,Tex2);
	//TU.x = dot(D,Tex1);
	//TU.y = dot(D,Tex2);
	//float2 TU = Tex2;
	
	float3 F = tex2D(Sampler2, TU);
	float4 D = tex2D(Sampler3, Tex3);
	
	return float4(F,(1-D.a)*Diffuse.a);
	//return float4(F,1);
}
*/

// ====================================================================================================

technique SparkEmitter
{
    pass P0
    {          		
        VertexShader = compile vs_1_1 SparkVS();
        PixelShader  = compile ps_1_1 SimplePS();
    }
}

technique particle_warp
{
    pass P0
    {          		
        VertexShader = compile vs_1_1 ParticleWarpVS();
        PixelShader  = compile ps_2_0 ParticleWarpPS();
    }
}

/*
technique particle_distort_ps11
{
    pass P0
    {          		
        VertexShader = compile vs_1_1 ParticleDistortVS_PS11();
        PixelShader  = compile ps_1_1 ParticleDistortPS_PS11();
    }
}

technique particle_warp_ps11
{
    pass P0
    {          		
        VertexShader = compile vs_1_1 ParticleWarpVS_PS11();
        PixelShader  = compile ps_1_1 ParticleWarpPS_PS11();
    }
}
*/
