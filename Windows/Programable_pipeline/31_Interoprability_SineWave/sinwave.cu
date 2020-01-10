

__global__ void sinwave_vbo_kernel(float4* pos, unsigned int width, unsigned int height, float animtype)
{

	unsigned int Y = blockIdx.y * blockDim.y + threadIdx.y;
	unsigned int X = blockIdx.x * blockDim.x + threadIdx.x;
	
	
	
	float u = X/float(width);
	float v = Y/float(height);
	
	u = ( u * 5.0) - 3.0;
	v = (v * 5.0) - 3.0;
	
	float frequency = 0.5f;
	
	float w = sinf(frequency * u + animtype) * cosf(frequency * v + animtype) ;
	          
	
	pos[Y * width + X] = make_float4(u,w,v,1.0);
	
}


void launchCudaKernel(float4 *pPos, int meshWidth, int meshHeight, float type)
{
 
  dim3 DimBlock=dim3(8,8,1);
  dim3 DimGrid=dim3(meshWidth / DimBlock.x, meshHeight/DimBlock.y, 1);
  
  sinwave_vbo_kernel<<<DimGrid, DimBlock>>>(pPos, meshWidth, meshHeight, type);
  
}


  
	