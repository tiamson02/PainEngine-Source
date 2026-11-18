float4 Pan1_2	: register(c0);
float4 Pan3_4	: register(c1);

float4 Weight1	: register(c2);
float4 Weight2	: register(c3);

float2 Offsets[16]	: register(c0);
float4 Weights[16]	: register(c16);

float4	Blur4Weights[4]	: register(c0);

float4	PanTex0		: register(c24);
float4	PanTex1		: register(c27);
float4	PanTex2		: register(c30);
float4	PanTex3		: register(c33);

float4	LumThreshold	: register(c0);

// for vertex shader (taps 1-3)
// for PS20 it's 3 inner taps (1,2,3)
// for PS11 it's 4 inner taps (1,2,3,4)

float4		GTapOffset[4]		: register(c20);	// inner taps only
float4		GTexelOffsetVS[4]	: register(c40);	// outer taps only

// for pixel shader
float4		GTexelWeight[7]		: register(c0);		// gauss weights 0-6
float4		GTexelOffset[4]		: register(c7);		// outer taps only

sampler Sampler0	: register(s0);
sampler Sampler1	: register(s1);
sampler Sampler2	: register(s2);
sampler Sampler3	: register(s3);

sampler LinearSampler0	: register(s0);
sampler PointSampler1	: register(s1);

// =============================================================================================================

struct VS_INPUT
{
	float4 Position		: POSITION;
    float4 Diffuse		: COLOR0;     // vertex diffuse color (note that COLOR0 is clamped from 0..1)
    float2 TexCoord		: TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Position		: POSITION;
    float2 TexCoord		: TEXCOORD0;
};

struct VS_OUTPUT_PS11
{
    float4 Position		: POSITION;
    float2 Tex0			: TEXCOORD0;
    float2 Tex1			: TEXCOORD1;
    float2 Tex2			: TEXCOORD2;
    float2 Tex3			: TEXCOORD3;    
};

struct VS_GaussOutput
{
	float4	Position	: POSITION;
	float2	Tap0		: TEXCOORD0;	// center tap
	float4	Tap1Plus	: TEXCOORD1;	// +1
	float4	Tap2Plus	: TEXCOORD2;	// +2
	float4	Tap3Plus	: TEXCOORD3;	// +3
	float4	Tap1Minus	: TEXCOORD4;	// -1
	float4	Tap2Minus	: TEXCOORD5;	// -2
	float4	Tap3Minus	: TEXCOORD6;	// -3
};

struct VS_GaussOutput_PS11
{
	float4	Position	: POSITION;
	float2	Tap1		: TEXCOORD0;	// 1
	float2	Tap2		: TEXCOORD1;	// 2
	float2	Tap3		: TEXCOORD2;	// 3
	float2	Tap4		: TEXCOORD3;	// 3
};

// =============================================================================================================

VS_OUTPUT PassThruVS(VS_INPUT In)
{
    VS_OUTPUT Out;

	// pass through
    Out.Position = In.Position;
    Out.TexCoord = In.TexCoord;
    
    return Out;
}

VS_OUTPUT_PS11 BlurVS(VS_INPUT In)
{
    VS_OUTPUT_PS11 Out;
    
    Out.Position	= In.Position;
    Out.Tex0		= In.TexCoord + PanTex0;
    Out.Tex1		= In.TexCoord + PanTex1;
    Out.Tex2		= In.TexCoord + PanTex2;
    Out.Tex3		= In.TexCoord + PanTex3;
    
    return Out;
}

VS_GaussOutput GaussVS(VS_INPUT In)
{
	VS_GaussOutput Out;
	
	Out.Position	= In.Position;
    Out.Tap0		= In.TexCoord;
	
    Out.Tap1Plus.xy		= In.TexCoord + GTapOffset[0];
    Out.Tap2Plus.xy		= In.TexCoord + GTapOffset[1];
    Out.Tap3Plus.xy		= In.TexCoord + GTapOffset[2];
    Out.Tap1Minus.xy	= In.TexCoord - GTapOffset[0];
    Out.Tap2Minus.xy	= In.TexCoord - GTapOffset[1];
    Out.Tap3Minus.xy	= In.TexCoord - GTapOffset[2];

	Out.Tap1Plus.zw		= In.TexCoord + GTexelOffsetVS[0];
	Out.Tap2Plus.zw		= In.TexCoord + GTexelOffsetVS[1];
	Out.Tap3Plus.zw		= In.TexCoord + GTexelOffsetVS[2];
	Out.Tap1Minus.zw	= In.TexCoord - GTexelOffsetVS[0];
	Out.Tap2Minus.zw	= In.TexCoord - GTexelOffsetVS[1];
	Out.Tap3Minus.zw	= In.TexCoord - GTexelOffsetVS[2];
    
    return Out;
}

VS_GaussOutput_PS11 GaussAccumVS(VS_INPUT In)
{
	VS_GaussOutput_PS11 Out;
	
	Out.Position	= In.Position;
    Out.Tap1		= In.TexCoord + GTapOffset[0];
    Out.Tap2		= In.TexCoord + GTapOffset[1];
    Out.Tap3		= In.TexCoord + GTapOffset[2];
    Out.Tap4		= In.TexCoord + GTapOffset[3];
    
    return Out;
}

/*
VS_GaussOutput_PS11 GaussVS_Minus(VS_INPUT In)
{
	VS_GaussOutput_PS11 Out;
	
	Out.Position	= In.Position;
    Out.Tap1		= In.TexCoord - GTapOffset[0];
    Out.Tap2		= In.TexCoord - GTapOffset[1];
    Out.Tap2		= In.TexCoord - GTapOffset[2];
    Out.Tap2		= In.TexCoord - GTapOffset[3];
    
    return Out;
}
*/

float4 GaussAccumPS(VS_GaussOutput_PS11 In) : COLOR
{
	float4 Sum = 0.0F;
	
	Sum += tex2D(Sampler0, In.Tap1.xy) * GTexelWeight[1];
	Sum += tex2D(Sampler1, In.Tap2.xy) * GTexelWeight[2];
	Sum += tex2D(Sampler2, In.Tap3.xy) * GTexelWeight[3];
	Sum += tex2D(Sampler3, In.Tap4.xy) * GTexelWeight[4];
	
	return Sum;
}

float4 GaussCenterPS(VS_OUTPUT In) : COLOR
{
	return tex2D(Sampler0, In.TexCoord.xy) * GTexelWeight[0];
}

float4 CopyQuadPS(VS_OUTPUT In) : COLOR0
{
	return tex2D(Sampler0, In.TexCoord.xy);
}

/*
float4 BrightPass4x4PS(VS_OUTPUT In) : COLOR0
{
	float4 sum = 0.0F;
	for (int i=0; i<16; ++i)
		sum += tex2D(Sampler0, In.TexCoord.xy + Offsets[i]);
	
	float4 C = sum/16.0F;
		
	float lum = dot(C, float4(0.3, 0.59, 0.11, 0));
	float mask = 2 * saturate(lum-0.25F);
	
	return C * mask;
	
}
*/


float4 BrightPassPS(VS_OUTPUT In) : COLOR0
{		
	float4 C = tex2D(Sampler0, In.TexCoord.xy);	
	
	float lum = dot((float3) C, (float3) LumThreshold);
	float mask = 2 * saturate(lum - LumThreshold.w);
	
	return C * mask;	
	
	/*	
	float3 C = tex2D(Sampler0, In.TexCoord.xy);	
	
	//float lum = dot(C, float4(0.3, 0.59, 0.11, 0));
	//float mask = saturate(lum - 0.25);

	float lum = dot(C, (float3)Luminance);
	float mask = 2 * saturate(lum - Luminance.w);
	
	//return float4(mask,mask,mask,0);	
	return float4(C * mask, 0);
	*/
}

float4 Gauss15PS(VS_OUTPUT In) : COLOR0
{
	float4 sum = 0.0F;
	for (int i=0; i<15; ++i)
		sum += Weights[i] * tex2D(Sampler0, In.TexCoord.xy + Offsets[i]);
		
	return sum;		
}

float4 Blur4PS(VS_OUTPUT_PS11 In) : COLOR0
{
	float4 sum = 0.0F;
	
	sum += tex2D(Sampler0, In.Tex0.xy) * Blur4Weights[0];
	sum += tex2D(Sampler1, In.Tex1.xy) * Blur4Weights[1];
	sum += tex2D(Sampler2, In.Tex2.xy) * Blur4Weights[2];
	sum += tex2D(Sampler3, In.Tex3.xy) * Blur4Weights[3];
	
	//sum = tex2D(Sampler0, In.Tex0.xy) * Blur4Weights[0];
	
	return sum;
}

half4 GaussPS(VS_GaussOutput In) : COLOR
{
	half4 InnerTaps[7];
	
	// sample center tap
	InnerTaps[0] = tex2D(LinearSampler0, In.Tap0);
		
	// get 3 surrounding taps using interpolated texcoords (non-dependant reads)
	InnerTaps[1] = tex2D(LinearSampler0, In.Tap1Plus.xy	);
	InnerTaps[2] = tex2D(LinearSampler0, In.Tap2Plus.xy	);
	InnerTaps[3] = tex2D(LinearSampler0, In.Tap3Plus.xy	);
	InnerTaps[4] = tex2D(LinearSampler0, In.Tap1Minus.xy);
	InnerTaps[5] = tex2D(LinearSampler0, In.Tap2Minus.xy);
	InnerTaps[6] = tex2D(LinearSampler0, In.Tap3Minus.xy);
	
	// starting accumulating - center tap
	half4 Sum = InnerTaps[0] * GTexelWeight[0];

	// inner taps
	Sum	+= InnerTaps[1] * GTexelWeight[1];
	Sum	+= InnerTaps[2] * GTexelWeight[2];
	Sum	+= InnerTaps[3] * GTexelWeight[3];

	Sum	+= InnerTaps[4] * GTexelWeight[1];
	Sum	+= InnerTaps[5] * GTexelWeight[2];
	Sum	+= InnerTaps[6] * GTexelWeight[3];
	
	// sample remaining taps using dependant reads
	half4 OuterTaps[6];
	
	OuterTaps[0] = tex2D(LinearSampler0, In.Tap1Plus.zw	 );
	OuterTaps[1] = tex2D(LinearSampler0, In.Tap2Plus.zw	 );
	OuterTaps[2] = tex2D(LinearSampler0, In.Tap3Plus.zw	 );
	OuterTaps[3] = tex2D(LinearSampler0, In.Tap1Minus.zw );
	OuterTaps[4] = tex2D(LinearSampler0, In.Tap2Minus.zw );
	OuterTaps[5] = tex2D(LinearSampler0, In.Tap3Minus.zw );
	
	// accumulate outer taps
	Sum += OuterTaps[0] * GTexelWeight[4];
	Sum += OuterTaps[1] * GTexelWeight[5];
	Sum += OuterTaps[2] * GTexelWeight[6];

	Sum += OuterTaps[3] * GTexelWeight[4];
	Sum += OuterTaps[4] * GTexelWeight[5];
	Sum += OuterTaps[5] * GTexelWeight[6];	
	
	return Sum;
}

// ===============================================================================================

technique copy_quad	// testing
<
	string Material		= "filter_fb";
	string StreamDef	= "3C2";
	string HWClass		= "NV20";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 PassThruVS();
        PixelShader  = compile ps_1_1 CopyQuadPS(); 
	}
}

technique bright_pass
<
	string Material		= "filter_fb";
	string StreamDef	= "3C2";
	string HWClass		= "NV20";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 PassThruVS();
        PixelShader  = compile ps_1_1 BrightPassPS(); 
	}
}

technique gauss_fb1
<
	string Material		= "filter_fb1";
	string StreamDef	= "3C2";
	string HWClass		= "NV30";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 GaussVS();
        PixelShader  = compile ps_2_0 GaussPS(); 
	}
}

technique gauss_fb2
<
	string Material		= "filter_fb2";
	string StreamDef	= "3C2";
	string HWClass		= "NV30";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 GaussVS();
        PixelShader  = compile ps_2_0 GaussPS(); 
	}
}

// =====================================================================================

technique gauss_center_fb1
<
	string Material		= "blur_center_fb1_ps11";
	string StreamDef	= "3C2";
	string HWClass		= "NV20-R200";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 PassThruVS();
        PixelShader  = compile ps_1_1 GaussCenterPS(); 
	}
}

technique gauss_accum_fb1
<
	string Material		= "blur_fb1_ps11";
	string StreamDef	= "3C2";
	string HWClass		= "NV20-R200";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 GaussAccumVS();
        PixelShader  = compile ps_1_1 GaussAccumPS(); 
	}
}

technique gauss_center_fb2
<
	string Material		= "blur_center_fb2_ps11";
	string StreamDef	= "3C2";
	string HWClass		= "NV20-R200";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 PassThruVS();
        PixelShader  = compile ps_1_1 GaussCenterPS(); 
	}
}

technique gauss_accum_fb2
<
	string Material		= "blur_fb2_ps11";
	string StreamDef	= "3C2";
	string HWClass		= "NV20-R200";
>
{
	pass P0
	{
        VertexShader = compile vs_1_1 GaussAccumVS();
        PixelShader  = compile ps_1_1 GaussAccumPS(); 
	}
}
