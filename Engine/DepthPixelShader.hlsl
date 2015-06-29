////////////////////////////////////////////////////////////////////////////////
// Filename: depth.ps
////////////////////////////////////////////////////////////////////////////////


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 DepthPixelShader(PixelInputType input) : SV_TARGET
{
	float depthValue;
	float4 color = { 0.0f, 0.0f, 0.0f, 0.0f, };


	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.depthPosition.z / input.depthPosition.w;

	// First 10% of the depth buffer color red.
	if (depthValue < 0.9f)
	{
		color = float4(1.0, 0.0f, 0.0f, 1.0f);
	}

	// The next 0.025% portion of the depth buffer color green.
	if (depthValue > 0.9f)
	{
		color = float4(0.0, 1.0f, 0.0f, 1.0f);
	}

	// The remainder of the depth buffer color blue.
	if (depthValue > 0.925f)
	{
		color = float4(0.0, 0.0f, 1.0f, 1.0f);
	}

	return color;
}
